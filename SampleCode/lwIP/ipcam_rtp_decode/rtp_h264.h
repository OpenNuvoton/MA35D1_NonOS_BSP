// rtp_h264.h
#ifndef __RTP_H264_H__
#define __RTP_H264_H__

#include <stdint.h>
#include "rtp_ring.h"

/* Convert one RTP/H.264 packet into an Annex-B NALU and push it into the ring.
 * Currently only handles:
 *   - Single NAL Unit (nal_type 1~23)
 *   - FU-A (nal_type 28)
 * Returns 0 = OK, negative value = error/discard.
 */
int h264_rtp_to_annexb(rtp_ringbuf_t *ringbuf, const uint8_t *payload, uint32_t payload_len);

#ifdef HAVE_AVTP
/* Convert one AVTP/CVF H.264 payload into Annex-B and push it into the ring. */
extern rtp_ringbuf_t *g_avtp_ringbuf;

int h264_cvf_to_annexb(rtp_ringbuf_t *ringbuf,
					   const uint8_t *payload,
					   uint32_t payload_len,
					   uint8_t sequence_number);
#endif

#endif /* __RTP_H264_H__ */
