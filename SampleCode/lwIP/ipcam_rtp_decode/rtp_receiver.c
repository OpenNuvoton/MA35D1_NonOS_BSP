#include "lwip/api.h"
#include "lwip/inet.h"
#include "rtp_ring.h"
#include "FreeRTOS.h"
#include "task.h"
#include "NuMicro.h"
#include "rtp_h264.h"


#define RTP_RECV_STACKSIZE  1024
#define RTP_RECV_PRIO       ( tskIDLE_PRIORITY + 2UL )
#define VC8K_DECODE_STACKSIZE  1024
#define VC8K_DECODE_PRIO    ( tskIDLE_PRIORITY + 2UL )

/* Maximum supported RTP UDP payload length (1500 based on typical MTU) */
#define RTP_MAX_UDP_PAYLOAD 1500

#ifdef HAVE_AVTP
rtp_ringbuf_t *g_avtp_ringbuf;
#endif

static uint8_t payload_buf[RTP_MAX_UDP_PAYLOAD];
static void rtp_udp_recv_thread(void *arg)
{
    struct netconn *conn;
    struct netbuf  *buf;
    err_t           err;
    rtp_ringbuf_t *ringbuf = (rtp_ringbuf_t *)arg;

    /* Fixed receive buffer: avoid malloc/free on each packet consuming FreeRTOS heap */

    sysprintf("RTP UDP recv thread start...\n");

    conn = netconn_new(NETCONN_UDP);
    if (conn == NULL)
    {
        sysprintf("netconn_new failed\n");
        vTaskDelete(NULL);
    }

    err = netconn_bind(conn, NULL, RTP_UDP_PORT);
    if (err != ERR_OK)
    {
        sysprintf("netconn_bind failed: %d\n", err);
        netconn_delete(conn);
        vTaskDelete(NULL);
    }

    sysprintf("Listening RTP UDP on port %d\n", RTP_UDP_PORT);
    // sysprintf("Producer ring obj=%p, buf=%p size=%lu\n",
    //           (void *)&g_rtp_ring,
    //           (void *)g_rtp_ring.buf,
    //           (unsigned long)g_rtp_ring.size);


    static uint32_t pkt_cnt = 0;

    while (1)
    {
        /* Blocking wait for UDP packet; will not busy-loop */
        err = netconn_recv(conn, &buf);
        if (err != ERR_OK)
        {
            sysprintf("netconn_recv error: %d\n", err);
            continue;
        }

        u16_t payload_len = netbuf_len(buf);

        /* Safety check: if camera payload is too large, drop it */
        if (payload_len > RTP_MAX_UDP_PAYLOAD)
        {
            sysprintf("RTP packet too large (%u), drop\n", (unsigned)payload_len);
            netbuf_delete(buf);
            continue;
        }

        /* Copy entire UDP payload into fixed buffer */
        netbuf_copy(buf, payload_buf, payload_len);

        pkt_cnt++;
        if ((pkt_cnt % 1000U) == 0U)
        {
            // sysprintf("Got UDP packet #%lu, pool used %lu, w=%lu\n",
            //           (unsigned long)pkt_cnt, (unsigned long)rtp_ringbuf_used(ringbuf), (unsigned long)ringbuf->w);
        }

        /* Send to H.264 RTP to Annex-B and ring buffer */
        int ret = h264_rtp_to_annexb(ringbuf, payload_buf, payload_len);
        if (ret < 0)
        {
            // packet dropped due to queue full
            sysprintf("h264_rtp_to_annexb error: %d\n", ret);
        }

        netbuf_delete(buf);
    }
}

extern void vc8k_decode_task(void *arg);
TaskHandle_t xVc8kDecodeTaskHandle;
void udp_echoserver_netconn_init(void)
{
    rtp_ringbuf_t *ringbuf = rtp_ringbuf_init();

#ifdef HAVE_AVTP
    g_avtp_ringbuf = ringbuf;
#endif

    sys_thread_new("RTP_RECV",
                   rtp_udp_recv_thread,
                   ringbuf,
                   RTP_RECV_STACKSIZE,
                   RTP_RECV_PRIO);

    BaseType_t rc = xTaskCreate(vc8k_decode_task,
                            "VC8KDec",
                            VC8K_DECODE_STACKSIZE,
                            ringbuf,
                            VC8K_DECODE_PRIO,
                            &xVc8kDecodeTaskHandle);

    if (rc != pdPASS)
        sysprintf("xTaskCreate VC8KDec FAILED! rc=%d\n", (int)rc);
    else
        sysprintf("xTaskCreate VC8KDec OK\n");
}
