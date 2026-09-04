/* rtp_ring.c */

#include "rtp_ring.h"
#include <string.h>
#include "NuMicro.h"
#include "FreeRTOS.h"

/* pre-allocated buffer for reconstruct NAL */
static uint8_t stream_ring_buf[PREALLOC_BACKUP_SIZE + PREALLOC_RING_SIZE];

rtp_ringbuf_t *rtp_ringbuf_init(void)
{
    rtp_ringbuf_t *rb;

    rb = pvPortMalloc(sizeof(rtp_ringbuf_t));
    if (!rb) {
        sysprintf("RTP ringbuf malloc failed!\n");
        return NULL;
    }

    rb->buf = stream_ring_buf;
    rb->size = PREALLOC_RING_SIZE;
    rb->reserved = PREALLOC_BACKUP_SIZE;
    rb->w = rb->r = rb->reserved;

    rb->current_ts = ~0U;
    rb->fua_s = 0;

    sysprintf("RTP ringbuf initialized, size=%luMB\n", (unsigned long)rb->size >> 20);

    return rb;
}

static inline size_t rtp_ringbuf_end(rtp_ringbuf_t *rb) {
    return rb->reserved + rb->size;
}

void rtp_ringbuf_push(rtp_ringbuf_t *rb, const uint8_t *data, size_t n)
{
    size_t head = rb->w;
    size_t ring_end = rtp_ringbuf_end(rb);

    // head is always >= rb->reserved
    size_t space_at_end = ring_end - head;
    size_t first = (n > space_at_end) ? space_at_end : n;
    size_t second = n - first;

    // First chunk: Write to end of ring
    memcpy(&rb->buf[head], data, first);
    dcache_clean_invalidate_by_mva(&rb->buf[head], first);

    // Second chunk: Wrap around to the START of the ring (rb->reserved)
    if (second > 0)
    {
        memcpy(&rb->buf[rb->reserved], data + first, second);
        dcache_clean_invalidate_by_mva(&rb->buf[rb->reserved], second);
    }

    // Update Write Pointer
    rb->w = ((head - rb->reserved + n) % rb->size) + rb->reserved;
}

size_t rtp_ringbuf_peek(rtp_ringbuf_t *rb, uint8_t **ptr)
{
    // Case 1: Standard contiguous block
    if (rb->w >= rb->r)
    {
        *ptr = &rb->buf[rb->r];
        return rb->w - rb->r;
    }

    // Case 2: Wrapped, data exists at [r ... end] & [reserved ... w]
    size_t tail_len = rtp_ringbuf_end(rb) - rb->r;

    // Can we linearize?
    if (tail_len <= PREALLOC_BACKUP_SIZE)
    {
        size_t backup_start = rb->reserved - tail_len;

        memcpy(&rb->buf[backup_start], &rb->buf[rb->r], tail_len);
        dcache_clean_invalidate_by_mva(&rb->buf[backup_start], tail_len);

        rb->r = backup_start;
        *ptr = &rb->buf[rb->r];

        return tail_len + (rb->w - rb->reserved);
    }

    *ptr = &rb->buf[rb->r];
    return tail_len;
}

void rtp_ringbuf_consume(rtp_ringbuf_t *rb, size_t n)
{
    size_t ring_end = rtp_ringbuf_end(rb);

    rb->r += n;

    if (rb->r >= ring_end)
        rb->r = rb->reserved + (rb->r - ring_end);
}

size_t rtp_ringbuf_used(rtp_ringbuf_t *rb)
{
    if (rb->w >= rb->r)
        return rb->w - rb->r;
    return rb->size - (rb->r - rb->w);
}

size_t rtp_ringbuf_space(rtp_ringbuf_t *rb)
{
    return rb->size - rtp_ringbuf_used(rb) - 1;
}

