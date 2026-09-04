/* rtp_ring.h - simple byte-stream ring buffer */

#ifndef __RTP_RING_H__
#define __RTP_RING_H__

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "semphr.h"

// #define IPCAM_RTP
#define RTP_UDP_PORT        50004

#define PREALLOC_RING_SIZE   (1024 * 1024 * 16)
#define TRIGGER_LEVEL_BYTES  (1024 * 128) // make sure there is enough data to decode a frame
#define HIGH_WATERMARK_ALERT (PREALLOC_RING_SIZE / 2)
#define PREALLOC_BACKUP_SIZE (TRIGGER_LEVEL_BYTES)

enum nal_type_e {
    NAL_TYPE_SINGLE = 0,
    NAL_TYPE_STAP_A,
    NAL_TYPE_FU_A,
};

typedef struct {
    uint8_t *buf;
    size_t size;

    size_t w;   // write index
    size_t r;   // read index
    int reset_flag;
    size_t reserved;

    uint32_t current_ts; // ts to recognize new frame
    enum nal_type_e type;
    int fua_s;

} rtp_ringbuf_t;

rtp_ringbuf_t *rtp_ringbuf_init(void);
void rtp_ringbuf_push(rtp_ringbuf_t *rb, const uint8_t *data, size_t n);
size_t rtp_ringbuf_peek(rtp_ringbuf_t *rb, uint8_t **ptr);
void rtp_ringbuf_consume(rtp_ringbuf_t *rb, size_t n);
size_t rtp_ringbuf_used(rtp_ringbuf_t *rb);
size_t rtp_ringbuf_space(rtp_ringbuf_t *rb);

#endif /* __RTP_RING_H__ */
