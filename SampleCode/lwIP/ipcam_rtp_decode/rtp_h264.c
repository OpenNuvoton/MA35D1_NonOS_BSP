/* h264_rtp.c
 *
 * Convert H.264 over RTP (RFC 6184) to Annex-B stream and push into RTP_RING.
 *
 * Supports:
 *   - Single NAL unit packet (NAL type 1-23)
 *   - FU-A (fragmentation unit, type 28)
 *   - STAP-A (type 24) is also supported to avoid problems when SPS/PPS are aggregated
 *
 * All lengths and buffer space are checked first. If not enough, the packet is dropped
 * to avoid corrupting memory.
 */

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"   /* for configASSERT (optional) */
#include "NuMicro.h"    /* for sysprintf (optional) */
#include "rtp_ring.h"
#include "rtp_h264.h"   /* header that declares h264_rtp_to_annexb() */

#define H264_NAL_TYPE(nal)   ((nal) & 0x1F)
#define H264_NAL_F(nal)      ((nal) & 0x80)
#define H264_NAL_NRI(nal)    ((nal) & 0x60)

/* 00 00 00 01 start code */
static const uint8_t s_h264_start_code[4] = {0x00, 0x00, 0x00, 0x01};

/* Handle Single NAL unit (type 1-23) such as SPS/PPS, etc. */
static int handle_single_nal(rtp_ringbuf_t *ringbuf, const uint8_t *nal, uint32_t nal_len, u_int32_t timestamp)
{
    size_t space;

    if (nal_len == 0U)
        return 0;

    ringbuf->current_ts = timestamp;
    ringbuf->type = NAL_TYPE_SINGLE;

    space = rtp_ringbuf_space(ringbuf);
    if(nal_len + 4U > space)
    {
        return -1; // not enough space
    }

    /* 4 bytes start code + nal_len */
    rtp_ringbuf_push(ringbuf, s_h264_start_code, 4);
    rtp_ringbuf_push(ringbuf, nal, nal_len);

    return 0;
}

/* Handle STAP-A (Aggregation Packet, NAL type 24) */
static int handle_stap_a(rtp_ringbuf_t *ringbuf, const uint8_t *payload, uint32_t payload_len, u_int32_t timestamp)
{
    size_t space;
    /* payload[0] is STAP-A NAL header, then follow:
       [size][NAL][size][NAL]...
     */
    if (payload_len < 1U)
        return -1;

    uint32_t offset = 1U;  /* skip STAP-A header */
    // uint32_t total_need = 0U;

    ringbuf->current_ts = timestamp;
    ringbuf->type = NAL_TYPE_STAP_A;

    while (offset + 2U <= payload_len)
    {
        uint16_t nal_size = (uint16_t)((payload[offset] << 8) | payload[offset + 1]);
        offset += 2U;

        if (nal_size == 0U)
            break;

        if (offset + nal_size > payload_len)
        {
            /* Broken packet; stop parsing and drop this STAP-A completely */
            // sysprintf("STAP-A malformed (size beyond packet)\n");
            return -1;
        }

        space = rtp_ringbuf_space(ringbuf);
        if(nal_size + 4U > space)
        {
            offset += nal_size;
            continue; // not enough space
        }

        /* 4 bytes start code + nal_len */
        rtp_ringbuf_push(ringbuf, s_h264_start_code, 4);
        rtp_ringbuf_push(ringbuf, &payload[offset], nal_size);

        /* Each NAL needs one start code + nal_size bytes */
        // total_need += 4U + nal_size;
        offset += nal_size;
    }

    return 0;
}

u32 fua_cnt = 0;
extern TaskHandle_t xVc8kDecodeTaskHandle;
/* Handle FU-A (Fragmentation Unit, NAL type 28) */
static int handle_fu_a(rtp_ringbuf_t *ringbuf, const uint8_t *payload, uint32_t payload_len, u_int32_t timestamp)
{
    size_t space;
    /* payload[0] = FU indicator
       payload[1] = FU header
       payload[2..] = fragment data
     */

    if (payload_len < 2U)
        return -1;

    uint8_t fu_indicator = payload[0];
    uint8_t fu_header    = payload[1];

    uint8_t S = (uint8_t)(fu_header & 0x80U);
    uint8_t E = (uint8_t)(fu_header & 0x40U);
    // (void)E;  /* E is not specially handled; it only indicates "end" */

    uint8_t nal_type = (uint8_t)(fu_header & 0x1FU);
    uint8_t F        = H264_NAL_F(fu_indicator);
    uint8_t NRI      = H264_NAL_NRI(fu_indicator);

    const uint8_t *frag_data = payload + 2U;
    uint32_t frag_len = payload_len - 2U;

    if (frag_len == 0U)
        return 0;  /* nothing to do */

    if (S)
    {
        // new frame, request new buffer
        ringbuf->current_ts = timestamp;
        ringbuf->type = NAL_TYPE_FU_A;
        fua_cnt = frag_len;

        space = rtp_ringbuf_space(ringbuf);
        if (frag_len + 5U > space)
        {
            return -1; // not enough space
        }

        uint8_t fu_nal_header = (uint8_t)(F | NRI | nal_type);
        // use fu_buf for fragmented NAL
        rtp_ringbuf_push(ringbuf, s_h264_start_code, 4);
        rtp_ringbuf_push(ringbuf, &fu_nal_header, 1U);
        rtp_ringbuf_push(ringbuf, frag_data, frag_len);
        ringbuf->fua_s = 1;
    }
    else if (ringbuf->fua_s)
    {
        /* Middle or end fragment: append fragment data to previous NAL */
        space = rtp_ringbuf_space(ringbuf);
        if (frag_len > space)
        {
            ringbuf->fua_s = 0;
            return -1; // not enough space
        }

        rtp_ringbuf_push(ringbuf, frag_data, frag_len);
        fua_cnt += frag_len;

        /* frame complete, issue decode */
        if (E)
        {
            ringbuf->fua_s = 0;
            // xTaskNotifyGive(xVc8kDecodeTaskHandle);
            // sysprintf("FU-A complete, total length=%lu\n", fua_cnt);
        }
    }

    return 0;
}
static uint16_t last_seq_num = 0;
/* Main entry: payload is the entire RTP UDP payload (including RTP header) */
int h264_rtp_to_annexb(rtp_ringbuf_t *ringbuf, const uint8_t *payload, uint32_t payload_len)
{
    configASSERT(ringbuf != NULL);
    configASSERT(payload != NULL);

    if (payload_len < 12U)
    {
        /* Not enough bytes for a basic RTP header */
        // sysprintf("RTP packet too short: %lu\n", payload_len);
        return -1;
    }

    /* Simplified RTP header parsing */
    uint8_t b0 = payload[0];
    uint8_t b1 = payload[1];
    uint16_t seq_num = (uint16_t)((payload[2] << 8) | payload[3]);
    uint32_t timestamp = (uint32_t)((payload[4] << 24) | (payload[5] << 16) |
                                   (payload[6] << 8) | (payload[7]));

    uint8_t version = (uint8_t)(b0 >> 6);
    uint8_t padding = (uint8_t)((b0 >> 5) & 0x01U);
    uint8_t extension = (uint8_t)((b0 >> 4) & 0x01U);
    uint8_t csrc_count = (uint8_t)(b0 & 0x0FU);
    (void)padding;   /* We do not handle trailing padding; we just require valid length */

    /* M bit and PT */
    uint8_t marker = (uint8_t)(b1 >> 7);
    uint8_t payload_type = (uint8_t)(b1 & 0x7FU);
    (void)marker; // TODO: check frame finished for I frame detection
    (void)payload_type;

    /* Base header length */
    uint32_t hdr_len = 12U + (uint32_t)csrc_count * 4U;

    /* RTP Header Extension (if X bit is set) */
    if (extension)
    {
        if (payload_len < hdr_len + 4U)
            return -1;

        uint16_t ext_len_words = (uint16_t)((payload[hdr_len + 2] << 8) | payload[hdr_len + 3]);
        uint32_t ext_total_len = 4U + ((uint32_t)ext_len_words * 4U);

        if (payload_len < hdr_len + ext_total_len)
            return -1;

        hdr_len += ext_total_len;
    }

    if (payload_len <= hdr_len)
        return 0;   /* No H.264 payload */

    const uint8_t *h264_payload = payload + hdr_len;
    uint32_t       h264_len     = payload_len - hdr_len;

    if (version != 2U)
    {
        // sysprintf("RTP version != 2, drop\n");
        return -1;
    }

    uint8_t nal0 = h264_payload[0];
    uint8_t nal_type = H264_NAL_TYPE(nal0);

    int ret = 0;

    // sysprintf("ts : %lu, seq : %d, type : %d, len : %d\n", timestamp, seq_num, nal_type, h264_len);
    if ((last_seq_num != 0) && (seq_num != (uint16_t)(last_seq_num + 1U)))
    {
        sysprintf("RTP packet loss detected! last_seq = %d, curr_seq = %d\n", last_seq_num, seq_num);
    }
    last_seq_num = seq_num;

    switch (nal_type)
    {
        case 1 ... 23:
            /* Single NAL Unit Packet */
            ret = handle_single_nal(ringbuf, h264_payload, h264_len, timestamp);
            break;

        case 24:
            /* STAP-A (Aggregation Packet) */
            ret = handle_stap_a(ringbuf, h264_payload, h264_len, timestamp);
            break;

        case 28:
            /* FU-A (Fragmentation Unit A) */
            ret = handle_fu_a(ringbuf, h264_payload, h264_len, timestamp);
            break;

        default:
            /* Unsupported types (STAP-B, MTAP, FU-B, etc.) are ignored */
            // sysprintf("Unsupported NAL type %u, drop\n", nal_type);
            ret = 0;
            break;
    }

    return ret;
}

#ifdef HAVE_AVTP
static uint8_t cvf_fua_active;
static uint8_t cvf_fua_last_sequence;

int h264_cvf_to_annexb(rtp_ringbuf_t *ringbuf,
                       const uint8_t *payload,
                       uint32_t payload_len,
                       uint8_t sequence_number)
{
    uint8_t nal_type;
    uint32_t fragment_length;
    size_t space;

    configASSERT(ringbuf != NULL);
    configASSERT(payload != NULL);

    if (payload_len == 0U)
        return -1;

    nal_type = H264_NAL_TYPE(payload[0]);
    if (nal_type == 9U)
        return 0;

    if (nal_type >= 1U && nal_type <= 23U)
    {
        space = rtp_ringbuf_space(ringbuf);
        if (payload_len + sizeof(s_h264_start_code) > space)
            return -1;

        cvf_fua_active = 0U;
        rtp_ringbuf_push(ringbuf, s_h264_start_code,
                         sizeof(s_h264_start_code));
        rtp_ringbuf_push(ringbuf, payload, payload_len);
        return 0;
    }

    if (nal_type != 28U || payload_len < 2U)
        return -1;

    uint8_t fu_indicator = payload[0];
    uint8_t fu_header = payload[1];
    uint8_t start = (uint8_t)(fu_header & 0x80U);
    uint8_t end = (uint8_t)(fu_header & 0x40U);
    fragment_length = payload_len - 2U;

    if (fragment_length == 0U)
        return -1;

    if (start)
    {
        uint8_t nal_header = (uint8_t)((fu_indicator & 0xE0U) |
                                       (fu_header & 0x1FU));

        space = rtp_ringbuf_space(ringbuf);
        if (fragment_length + sizeof(s_h264_start_code) + 1U > space)
            return -1;

        rtp_ringbuf_push(ringbuf, s_h264_start_code,
                         sizeof(s_h264_start_code));
        rtp_ringbuf_push(ringbuf, &nal_header, 1U);
        rtp_ringbuf_push(ringbuf, payload + 2U, fragment_length);
        cvf_fua_active = end ? 0U : 1U;
        cvf_fua_last_sequence = sequence_number;
        return 0;
    }

    if (!cvf_fua_active ||
        sequence_number != (uint8_t)(cvf_fua_last_sequence + 1U))
    {
        cvf_fua_active = 0U;
        return -1;
    }

    space = rtp_ringbuf_space(ringbuf);
    if (fragment_length > space)
    {
        cvf_fua_active = 0U;
        return -1;
    }

    rtp_ringbuf_push(ringbuf, payload + 2U, fragment_length);
    cvf_fua_last_sequence = sequence_number;
    if (end)
        cvf_fua_active = 0U;

    return 0;
}
#endif
