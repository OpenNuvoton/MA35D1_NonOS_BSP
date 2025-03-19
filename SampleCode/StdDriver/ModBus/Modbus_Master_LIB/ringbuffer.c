#include "ringbuffer.h"

void rbInitialize(RingBuffer *pRingBuff, uint8_t *buff, uint16_t length)
{
    pRingBuff->pBuff = buff;
    pRingBuff->pEnd  = buff + length;
    pRingBuff->wp = buff;
    pRingBuff->rp = buff;
    pRingBuff->length = length;
    pRingBuff->flagOverflow = 0;
}



void rbClear(RingBuffer *pRingBuff)
{
    pRingBuff->wp = pRingBuff->pBuff;
    pRingBuff->rp = pRingBuff->pBuff;
    pRingBuff->flagOverflow = 0;
}


void rbPush(RingBuffer *pRingBuff, uint8_t value)
{
    uint8_t *wp_next = pRingBuff->wp + 1;

    if (wp_next == pRingBuff->pEnd)
    {
        wp_next -= pRingBuff->length; // Rewind pointer when exceeds bound
    }

    if (wp_next != pRingBuff->rp)
    {
        *pRingBuff->wp = value;
        pRingBuff->wp = wp_next;
    }
    else
    {
        pRingBuff->flagOverflow = 1;
    }
}


uint8_t rbPop(RingBuffer *pRingBuff)
{
    if (pRingBuff->rp == pRingBuff->wp) return 0;  // empty

    uint8_t ret = *(pRingBuff->rp++);

    if (pRingBuff->rp == pRingBuff->pEnd)
    {
        pRingBuff->rp -= pRingBuff->length; // Rewind pointer when exceeds bound
    }

    return ret;
}

uint16_t rbGetCount(const RingBuffer *pRingBuff)
{
    return (pRingBuff->wp - pRingBuff->rp + pRingBuff->length) % pRingBuff->length;
}


int8_t rbIsEmpty(const RingBuffer *pRingBuff)
{
    return pRingBuff->wp == pRingBuff->rp;
}


int8_t rbIsFull(const RingBuffer *pRingBuff)
{
    return (pRingBuff->rp - pRingBuff->wp + pRingBuff->length - 1) % pRingBuff->length == 0;
}
