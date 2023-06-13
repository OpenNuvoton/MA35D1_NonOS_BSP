/*************************************************************************//**
 * @file     ma35d1_mac.c
 * @brief    GMAC hardware init helper APIs.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "netif/ma35d1_mac.h"

GMACdevice GMACdev[GMAC_CNT];
static DmaDesc tx_desc[GMAC_CNT][TRANSMIT_DESC_SIZE] __attribute__ ((aligned (64)));
static DmaDesc rx_desc[GMAC_CNT][RECEIVE_DESC_SIZE] __attribute__ ((aligned (64)));

static struct sk_buff rx_buf[GMAC_CNT][RECEIVE_DESC_SIZE] __attribute__ ((aligned (64)));

// These 2 are accessable from application
struct sk_buff txbuf[GMAC_CNT] __attribute__ ((aligned (64))); // set align to separate cacheable and non-cacheable data to different cache line.
struct sk_buff rxbuf[GMAC_CNT] __attribute__ ((aligned (64)));

u8 mac_addr0[6] = DEFAULT_MAC0_ADDRESS;
u8 mac_addr1[6] = DEFAULT_MAC1_ADDRESS;

static u32 GMAC_Power_down; // This global variable is used to indicate the ISR whether the interrupts occured in the process of powering down the mac or not

/**
 * @brief This sets up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures for ring mode or chain mode.
 * This function depends on the pcidev structure for allocation consistent dma-able memory in case of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 *  - Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] gmacdev pointer to GMACdevice.
 * @param[in] no_of_desc number of descriptor expected in tx descriptor queue.
 * @param[in] desc_mode whether descriptors to be created in RING mode or CHAIN mode.
 * @return 0 upon success. Error code upon failure.
 * @note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -GMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->TxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
s32 GMAC_setup_tx_desc_queue(GMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode)
{
    s32 i;
    DmaDesc *first_desc = &tx_desc[gmacdev->Intf][0];
    gmacdev->TxDescCount = 0;

    TR("Total size of memory required for Tx Descriptors in Ring Mode = 0x%08x\n",((sizeof(DmaDesc) * no_of_desc)));

    gmacdev->TxDescCount = no_of_desc;
    gmacdev->TxDesc      = first_desc;
#ifdef CACHE_ON
    gmacdev->TxDescDma   = (DmaDesc *)((u64)first_desc | NON_CACHE);
#else
    gmacdev->TxDescDma   = (DmaDesc *)((u64)first_desc);
#endif
    for(i = 0; i < gmacdev->TxDescCount; i++) {
        GMAC_tx_desc_init_ring(gmacdev->TxDescDma + i, i == (gmacdev->TxDescCount - 1));
        TR("%02d %08x \n",i, (unsigned int)(gmacdev->TxDesc + i) );
    }

    gmacdev->TxNext = 0;
    gmacdev->TxBusy = 0;
    gmacdev->TxNextDesc = gmacdev->TxDesc;
    gmacdev->TxBusyDesc = gmacdev->TxDesc;
    gmacdev->BusyTxDesc  = 0;

    return 0;
}

/**
 * @brief This sets up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures in ring mode or chain mode.
 * This function depends on the pcidev structure for allocation of consistent dma-able memory in case of linux.
 * This limitation is due to the fact that linux uses pci structure to allocate a dmable memory
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 *  - Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] gmacdev pointer to GMACdevice.
 * @param[in] no_of_desc number of descriptor expected in rx descriptor queue.
 * @param[in] desc_mode whether descriptors to be created in RING mode or CHAIN mode.
 * @return 0 upon success. Error code upon failure.
 * @note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -GMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->RxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
s32 GMAC_setup_rx_desc_queue(GMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode)
{
    s32 i;
    DmaDesc *first_desc = &rx_desc[gmacdev->Intf][0];
    gmacdev->RxDescCount = 0;

    TR("total size of memory required for Rx Descriptors in Ring Mode = 0x%08x\n",((sizeof(DmaDesc) * no_of_desc)));

    gmacdev->RxDescCount = no_of_desc;
    gmacdev->RxDesc      = first_desc;
#ifdef CACHE_ON
    gmacdev->RxDescDma   = (DmaDesc *)((u64)first_desc | NON_CACHE);
#else
    gmacdev->RxDescDma   = (DmaDesc *)((u64)first_desc);
#endif
    for(i = 0; i < gmacdev->RxDescCount; i++) {
        GMAC_rx_desc_init_ring(gmacdev->RxDescDma + i, i == (gmacdev->RxDescCount - 1));
        TR("%02d %08x \n",i, (unsigned int)(gmacdev->RxDesc + i));
    }

    gmacdev->RxNext = 0;
    gmacdev->RxBusy = 0;
    gmacdev->RxNextDesc = gmacdev->RxDesc;
    gmacdev->RxBusyDesc = gmacdev->RxDesc;
    gmacdev->BusyRxDesc   = 0;

    return 0;
}

/**
 * @brief Set RGMII/RMII mode 1000/100/10M.
 * This function is called after partner determined.
 * @param[in] gmacdev pointer to GMACdevice.
 * @return Always returns 0.
 */
s32 GMAC_set_mode(GMACdevice *gmacdev)
{
    // Must stop Tx/Rx before change speed/mode
    GMAC_TX_DISABLE(gmacdev);
    GMAC_RX_DISABLE(gmacdev);

    GMAC_set_mii_speed(gmacdev);

    GMAC_TX_ENABLE(gmacdev);
    GMAC_RX_ENABLE(gmacdev);

    return 0;
}

/**
 * @brief Function used when the interface is opened for use.
 * We register GMAC_linux_open function to linux open(). Basically this
 * function prepares the the device for operation . This function is called whenever ifconfig (in Linux)
 * activates the device (for example "ifconfig eth0 up"). This function registers
 * system resources needed
 * - Attaches device to device specific structure
 * - Programs the MDC clock for PHY configuration
 * - Check and initialize the PHY interface
 * - ISR registration
 * - Setup and initialize Tx and Rx descriptors
 * - Initialize MAC and DMA
 * - Allocate Memory for RX descriptors (The should be DMAable)
 * - Initialize one second timer to detect cable plug/unplug
 * - Configure and Enable Interrupts
 * - Enable Tx and Rx
 * - start the Linux network queue interface
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @param[in] mode MII mode
 *          - \ref RGMII_1G
 *          - \ref RMII_100M
 *          - \ref RMII_10M
 * @return Returns 0 on success and error status upon failure.
 */
s32 GMAC_open(int intf, int mode)
{
    s32 i;
    s32 status = 0;
    struct sk_buff *skb;
    GMACdevice *gmacdev = &GMACdev[intf];

    /* Enable module clock and MFP */
    if(intf == GMACINTF0) {
        CLK->SYSCLK0 |= CLK_SYSCLK0_GMAC0EN_Msk; // Enable GMAC0 clock

        if(mode == RGMII_1G) {
            SYS->GPE_MFPL = 0x88888888;
            SYS->GPE_MFPH = (SYS->GPE_MFPH & ~0x00FFFFFF) | 0x00888888;
            SYS->GMAC0MISCR &= ~SYS_GMAC0MISCR_RMIIEN_Msk;
        }
        else {
            SYS->GPE_MFPL = 0x99999999;
            SYS->GPE_MFPH = (SYS->GPE_MFPH & ~0x000000FF) | 0x00000099;
            SYS->GMAC0MISCR |= SYS_GMAC0MISCR_RMIIEN_Msk;
        }
    } else {
        CLK->SYSCLK0 |= CLK_SYSCLK0_GMAC1EN_Msk; // Enable GMAC1 clock

        if(mode == RGMII_1G) {
            SYS->GPF_MFPL = 0x88888888;
            SYS->GPF_MFPH = (SYS->GPF_MFPH & ~0x00FFFFFF) | 0x00888888;
            SYS->GMAC1MISCR &= ~SYS_GMAC1MISCR_RMIIEN_Msk;
        }
        else {
            SYS->GPF_MFPL = 0x99999999;
            SYS->GPF_MFPH = (SYS->GPF_MFPH & ~0x000000FF) | 0x00000099;
            SYS->GMAC1MISCR |= SYS_GMAC1MISCR_RMIIEN_Msk;
        }
    }

    /*Attach the device to MAC struct This will configure all the required base addresses
      such as Mac base, configuration base, phy base address(out of 32 possible phys )*/
    GMAC_attach(gmacdev, intf, DEFAULT_PHY_BASE);

    GMAC_reset(gmacdev); // Reset to make RGMII/RMII setting take affect --ya

    /*Lets read the version of ip in to device structure*/
    GMAC_read_version(gmacdev);

    /*Check for Phy initialization*/
    GMAC_set_mdc_clk_div(gmacdev, GmiiCsrClk4);

    gmacdev->ClockDivMdc = GMAC_get_mdc_clk_div(gmacdev);

    status = GMAC_check_phy_init(gmacdev, mode);
    if(status < 0)
        sysprintf("PHY init fail\n");

    /*Set up the tx and rx descriptor queue/ring*/
    GMAC_setup_tx_desc_queue(gmacdev, TRANSMIT_DESC_SIZE, RINGMODE);
    GMAC_init_tx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr

    GMAC_setup_rx_desc_queue(gmacdev, RECEIVE_DESC_SIZE, RINGMODE);
    GMAC_init_rx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr

    GMAC_DMA_BUSMODE_INIT(gmacdev, DmaBurstLength32 | DmaDescriptorSkip0 | GMAC_DmaBusMode_ATDS_Msk); //pbl32 incr with rxthreshold 128 and Desc is 8 Words
    GMAC_DMA_OPMODE_INIT(gmacdev, GMAC_DmaOpMode_TSF_Msk | GMAC_DmaOpMode_OSF_Msk | DmaRxThreshCtrl128);

    /*Initialize the mac interface*/
    status = GMAC_init(gmacdev);

    GMAC_pause_control(gmacdev); // This enables the pause control in Full duplex mode of operation

    /*IPC Checksum offloading is enabled for this driver. Should only be used if Full Ip checksumm offload engine is configured in the hardware*/
    GMAC_CHKSUM_OFFLOAD_ENABLE(gmacdev);  	//Enable the offload engine in the receive path
    GMAC_TCPIP_DROP_ERR_ENABLE(gmacdev); // This is default configuration, DMA drops the packets if error in encapsulated ethernet payload

    for(i = 0; i < RECEIVE_DESC_SIZE; i++) {
        skb = &rx_buf[intf][i];
        GMAC_set_rx_qptr(gmacdev, (u32)((u64)(skb->data) & 0xFFFFFFFF), sizeof(skb->data));
    }

    GMAC_clear_interrupt(gmacdev);
    GMAC_enable_interrupt(gmacdev, DMA_INT_ENABLE);

    GMAC_DMA_RX_ENABLE(gmacdev);
    GMAC_DMA_TX_ENABLE(gmacdev);

    GMAC_set_mac_addr(gmacdev, 0, intf == GMACINTF0 ? mac_addr0 : mac_addr1);

    GMAC_set_mode(gmacdev);

    return 0;
}

/**
 * @brief Function to register GMAC interrupt handler and enable interrupt
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @return Always returns 0.
 */
s32 GMAC_register_interrupt(int intf)
{
    if(intf == GMACINTF0) {
        IRQ_SetHandler(GMAC0_IRQn, GMAC_int_handler0);
        IRQ_Enable(GMAC0_IRQn);
    } else {
        IRQ_SetHandler(GMAC1_IRQn, GMAC_int_handler1);
        IRQ_Enable(GMAC1_IRQn);
    }

    return 0;
}

/**
 * @brief This gives up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the pcidev structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 * - Free the network buffer memory if any.
 * - Fee the memory allocated for the descriptors.
 * @param[in] gmacdev pointer to GMACdevice.
 * @param[in] desc_mode whether descriptors to be created in RING mode or CHAIN mode.
 * @return None.
 */
void GMAC_giveup_rx_desc_queue(GMACdevice *gmacdev, u32 desc_mode)
{
    gmacdev->RxDesc    = NULL;
    gmacdev->RxDescDma = 0;
}

/**
 * @brief This gives up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the pcidev structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 * - Free the network buffer memory if any.
 * - Fee the memory allocated for the descriptors.
 * @param[in] gmacdev pointer to GMACdevice.
 * @param[in] desc_mode whether descriptors to be created in RING mode or CHAIN mode.
 * @return None.
 */
void GMAC_giveup_tx_desc_queue(GMACdevice *gmacdev, u32 desc_mode)
{
    gmacdev->TxDesc    = NULL;
    gmacdev->TxDescDma = 0;
}

/**
 * @brief Function used when the interface is closed.
 * This releases all the system resources allocated during open call.
 * - Disable the device interrupts
 * - Stop the receiver and get back all the rx descriptors from the DMA
 * - Stop the transmitter and get back all the tx descriptors from the DMA
 * - Stop the Linux network queue interface
 * - Free the irq (ISR registered is removed from the kernel)
 * - Release the TX and RX descripor memory
 * - De-initialize one second timer rgistered for cable plug/unplug tracking
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @return Always return 0
 */
s32 GMAC_close(int intf)
{
    GMACdevice *gmacdev = &GMACdev[intf];

    /*Disable all the interrupts*/
    GMAC_disable_interrupt_all(gmacdev);

    TR("the GMAC interrupt has been disabled\n");

    /*Disable the reception*/
    GMAC_DMA_RX_DISABLE(gmacdev);
    GMAC_take_desc_ownership_rx(gmacdev);
    TR("the GMAC Reception has been disabled\n");

    /*Disable the transmission*/
    GMAC_DMA_TX_DISABLE(gmacdev);
    GMAC_take_desc_ownership_tx(gmacdev);

    if(intf == 0) {
        IRQ_Disable(GMAC0_IRQn);
    } else {
        IRQ_Disable(GMAC1_IRQn);
    }
    TR("the GMAC interrupt handler has been removed\n");

    /*Free the Rx Descriptor contents*/
    TR("Now calling GMAC_giveup_rx_desc_queue \n");
    GMAC_giveup_rx_desc_queue(gmacdev, RINGMODE);

    TR("Now calling GMAC_giveup_tx_desc_queue \n");
    GMAC_giveup_tx_desc_queue(gmacdev, RINGMODE);

    return 0;
}

/**
 * @brief Function to transmit a given packet on the wire.
 * Whenever Linux Kernel has a packet ready to be transmitted, this function is called.
 * The function prepares a packet and prepares the descriptor and
 * enables/resumes the transmission.
 * @param[in] skb pointer to sk_buff structure.
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @param[in] offload_needed whether enable hardware offload engine
 * @param[in] ts whether enable timestamp
 * @return Returns 0 on success and Error code on failure.
 */
s32 GMAC_xmit_frames(struct sk_buff *skb, int intf, u32 offload_needed, u32 ts)
{
    s32 status = 0;
    u32 dma_addr = (u32)((u64)skb->data & 0xFFFFFFFF);
    GMACdevice *gmacdev = &GMACdev[intf];

    /*Now we have skb ready and OS invoked this function. Lets make our DMA know about this*/
    status = GMAC_set_tx_qptr(gmacdev, skb->len, dma_addr, offload_needed, ts);
    if(status < 0) {
        TR("%s No More Free Tx Descriptors\n",__FUNCTION__);
        return -1;
    }

    /*Now force the DMA to start transmission*/
    GMAC_DMA_TX_PD_RESUME(gmacdev);

    return 0;
}

/**
 * @brief Function to handle housekeeping after a packet is transmitted over the wire.
 * After the transmission of a packet DMA generates corresponding interrupt
 * (if it is enabled). It takes care of returning the sk_buff to the linux
 * kernel, updating the networking statistics and tracking the descriptors.
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @return None.
 * @note This function runs in interrupt context.
 */
void GMAC_handle_transmit_over(int intf)
{
    GMACdevice *gmacdev;
    s32 desc_index;
    u32 status;
    u32 length;
    u32 buffer1, buffer2;
    u32 ext_status;
    u32 time_stamp_high;
    u32 time_stamp_low;

    gmacdev = &GMACdev[intf];

    /*Handle the transmit Descriptors*/
    do {
        desc_index = GMAC_get_tx_qptr(gmacdev, &status, &length, &buffer1, &buffer2, &ext_status, &time_stamp_high, &time_stamp_low);
        //GMAC_TS_read_timestamp_higher_val(gmacdev, &time_stamp_higher);

        if(desc_index >= 0 /*&& data1 != 0*/) {
            TR("Finished Transmit at Tx Descriptor %d for skb and buffer = %08x whose status is %08x \n", desc_index,buffer1,status);

            if(GMAC_is_tx_ipv4header_checksum_error(status)) {
                TR("Hardware Failed to Insert IPV4 Header Checksum\n");
                gmacdev->NetStats.tx_ip_header_errors++;
            }
            if(GMAC_is_tx_payload_checksum_error(status)) {
                TR("Hardware Failed to Insert Payload Checksum\n");
                gmacdev->NetStats.tx_ip_payload_errors++;
            }

            if(GMAC_is_desc_valid(status)) {
                gmacdev->NetStats.tx_bytes += length;
                gmacdev->NetStats.tx_packets++;
                if(status & DescTxTSStatus) {
                    gmacdev->tx_sec = time_stamp_high;
                    gmacdev->tx_subsec = time_stamp_low;
                } else {
                    gmacdev->tx_sec = 0;
                    gmacdev->tx_subsec = 0;
                }
            } else {
                TR("Error in Status %08x\n",status);
                gmacdev->NetStats.tx_errors++;
                gmacdev->NetStats.tx_aborted_errors += GMAC_is_tx_aborted(status);
                gmacdev->NetStats.tx_carrier_errors += GMAC_is_tx_carrier_error(status);
            }
        }
        gmacdev->NetStats.collisions += GMAC_get_tx_collision_count(status);
    } while(desc_index >= 0);
}

/**
 * @brief Function to Receive a packet from the interface.
 * After Receiving a packet, DMA transfers the received packet to the system memory
 * and generates corresponding interrupt (if it is enabled). This function prepares
 * the sk_buff for received packet after removing the ethernet CRC, and hands it over
 * to linux networking stack.
 * - Updataes the networking interface statistics
 * - Keeps track of the rx descriptors
 * @param[in] intf GMAC interface
 *          - \ref GMACINTF0
 *          - \ref GMACINTF1
 * @return None.
 * @note This function runs in interrupt context.
 */
void GMAC_handle_received_data(int intf)
{
    GMACdevice *gmacdev;
    s32 desc_index;
    u32 len;

    u32 status;
    u32 dma_addr1;
    u32 ext_status;
    u32 time_stamp_high;
    u32 time_stamp_low;
    struct sk_buff *rb = &rxbuf[intf];

    //struct sk_buff *skb; //This is the pointer to hold the received data

    TR("%s\n",__FUNCTION__);

    gmacdev = &GMACdev[intf];

    /*Handle the Receive Descriptors*/
    do {
        desc_index = GMAC_get_rx_qptr(gmacdev, &status, NULL, &dma_addr1, NULL, &ext_status, &time_stamp_high, &time_stamp_low);
        if(desc_index > 0) {
            TR("S:%08x ES:%08x DA1:%08x TSH:%08x TSL:%08x\n",status,ext_status,dma_addr1,time_stamp_high,time_stamp_low);
        }

        if(desc_index >= 0) {
            TR("Received Data at Rx Descriptor %d for skb whose status is %08x\n",desc_index,status);

            //skb = (struct sk_buff *)((u64)data1);
            if(1) {
                // Always enter this loop. GMAC_is_rx_desc_valid() also report invalid descriptor
                // if there's packet error generated by test code and drop it. But we need to execute ext_status
                // check code to tell what's going on.                                          --ya

                len = GMAC_get_rx_desc_frame_length(status) - 4; //Not interested in Ethernet CRC bytes

                // Now lets check for the IPC offloading
                /*  Since we have enabled the checksum offloading in hardware, lets inform the kernel
                    not to perform the checksum computation on the incoming packet. Note that ip header
                    checksum will be computed by the kernel immaterial of what we inform. Similary TCP/UDP/ICMP
                    pseudo header checksum will be computed by the stack. What we can inform is not to perform
                    payload checksum.
                    When CHECKSUM_UNNECESSARY is set kernel bypasses the checksum computation.
                */

                TR("Checksum Offloading will be done now\n");

                if(GMAC_is_ext_status(gmacdev, status)) { // extended status present indicates that the RDES4 need to be probed
                    TR("Extended Status present\n");
                    if(GMAC_ES_is_IP_header_error(ext_status)) {      // IP header (IPV4) checksum error
                        //Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
                        TR("(EXTSTS)Error in IP header error\n");
                        gmacdev->NetStats.rx_ip_header_errors++;
                    }
                    if(GMAC_ES_is_rx_checksum_bypassed(ext_status)) {  // Hardware engine bypassed the checksum computation/checking
                        TR("(EXTSTS)Hardware bypassed checksum computation\n");
                    }
                    if(GMAC_ES_is_IP_payload_error(ext_status)) {      // IP payload checksum is in error (UDP/TCP/ICMP checksum error)
                        TR("(EXTSTS) Error in EP payload\n");
                        gmacdev->NetStats.rx_ip_payload_errors++;
                    }
                } else { // No extended status. So relevant information is available in the status itself
                    if(GMAC_is_rx_checksum_error(status) == RxNoChkError) {
                        TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 4>  \n");
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxIpHdrChkError) {
                        //Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
                        TR(" Error in 16bit IPV4 Header Checksum <Chk Status = 6>  \n");
                        gmacdev->NetStats.rx_ip_header_errors++;
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxLenLT600) {
                        TR("IEEE 802.3 type frame with Length field Lesss than 0x0600 <Chk Status = 0> \n");
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxIpHdrPayLoadChkBypass) {
                        TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 1>\n");
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxChkBypass) {
                        TR("Ip header and TCP/UDP payload checksum Bypassed <Chk Status = 3>  \n");
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxPayLoadChkError) {
                        TR(" TCP/UDP payload checksum Error <Chk Status = 5>  \n");
                        gmacdev->NetStats.rx_ip_payload_errors++;
                    }
                    if(GMAC_is_rx_checksum_error(status) == RxIpHdrPayLoadChkError) {
                        //Linux Kernel doesnot care for ipv4 header checksum. So we will simply proceed by printing a warning ....
                        TR(" Both IP header and Payload Checksum Error <Chk Status = 7>  \n");
                        gmacdev->NetStats.rx_ip_header_errors++;
                        gmacdev->NetStats.rx_ip_payload_errors++;
                    }
                }
#ifdef CACHE_ON
                memcpy((void *)rb->data, (void *)((u64)dma_addr1 | NON_CACHE), len);
                memcpy((void *)rb->data + len, (void *)((u64)(dma_addr1 | NON_CACHE) + len), 4);
#else
                memcpy((void *)rb->data, (void *)((u64)dma_addr1), len);
                memcpy((void *)rb->data + len, (void *)((u64)dma_addr1 + len), 4);
#endif
                rb->rdy = 1;
                rb->len = len;

                gmacdev->NetStats.rx_packets++;
                gmacdev->NetStats.rx_bytes += len;
                if(GMAC_is_timestamp_available(status)) {
                    gmacdev->rx_sec = time_stamp_high;
                    gmacdev->rx_subsec = time_stamp_low;
                } else {
                    gmacdev->rx_sec = 0;
                    gmacdev->rx_subsec = 0;
                }
            } else {
                /*Now the present skb should be set free*/
                TR("s: %08x\n",status);
                gmacdev->NetStats.rx_errors++;
                gmacdev->NetStats.collisions       += GMAC_is_rx_frame_collision(status);
                gmacdev->NetStats.rx_crc_errors    += GMAC_is_rx_crc(status);
                gmacdev->NetStats.rx_frame_errors  += GMAC_is_frame_dribbling_errors(status);
                gmacdev->NetStats.rx_length_errors += GMAC_is_rx_frame_length_errors(status);
            }

        }
    } while(desc_index >= 0); // do until desc is empty
}

/**
 * @brief Function to power up and resume GMAC IP if magic packet is determined.
 * @param[in] gmacdev pointer to GMACdevice.
 * @return None.
 * @note This function runs in interrupt context.
 */
void GMAC_powerup_mac(GMACdevice *gmacdev)
{
    GMAC_Power_down = 0;	// Let ISR know that MAC is out of power down now
    if(GMAC_is_magic_packet_received(gmacdev))
        sysprintf("GMAC wokeup due to Magic Pkt Received\n");
    //Disable the assertion of PMT interrupt
    GMAC_PMT_INT_DISABLE(gmacdev);
    //Enable the mac and Dma rx and tx paths
    GMAC_RX_ENABLE(gmacdev);
    GMAC_DMA_RX_ENABLE(gmacdev);

    GMAC_TX_ENABLE(gmacdev);
    GMAC_DMA_TX_ENABLE(gmacdev);
}

/**
 * @brief Function to power down GMAC IP.
 *
 * @param[in] gmacdev pointer to GMACdevice.
 * @return None.
 */
void GMAC_powerdown_mac(GMACdevice *gmacdev)
{
    TR("Put the GMAC to power down mode..\n");
    // Disable the Dma engines in tx path
    GMAC_Power_down = 1;	// Let ISR know that Mac is going to be in the power down mode
    GMAC_DMA_TX_DISABLE(gmacdev);
    plat_delay(DEFAULT_LOOP_VARIABLE);		// Allow any pending transmission to complete
    // Disable the Mac for both tx and rx
    GMAC_TX_DISABLE(gmacdev);
    GMAC_RX_DISABLE(gmacdev);
    plat_delay(DEFAULT_LOOP_VARIABLE); 		// Allow any pending buffer to be read by host
    //Disable the Dma in rx path
    GMAC_DMA_RX_DISABLE(gmacdev);

    //enable the power down mode
    //GMAC_pmt_unicast_enable(gmacdev);

    //prepare the gmac for magic packet reception and wake up frame reception
    GMAC_PMT_MAGIC_PKT_ENABLE(gmacdev);

    //gate the application and transmit clock inputs to the code. This is not done in this driver :).

    //enable the Mac for reception
    GMAC_RX_ENABLE(gmacdev);

    //Enable the assertion of PMT interrupt
    GMAC_PMT_INT_ENABLE(gmacdev);
    //enter the power down mode
    GMAC_PMT_PD_ENABLE(gmacdev);
}

/**
 * @brief Interrupt service routing for GMAC0.
 * This is the function registered as ISR for device interrupts.
 * @param[in] None
 * @return None
 * @note This function runs in interrupt context
 */
void GMAC_int_handler0(void)
{
    GMACdevice *gmacdev = &GMACdev[GMACINTF0];
    u32 interrupt, dma_status_reg, mac_status_reg;
    u32 dma_addr;
    u32 volatile reg;

    // Check GMAC interrupt
    mac_status_reg = GMAC_GET_INT_SUMMARY(gmacdev);

    /* handle ts status */
    if(mac_status_reg & GMAC_IntStatus_TSIS_Msk) {
        gmacdev->NetStats.ts_int = 1;
        reg = GMAC_GET_TS_STATUS(gmacdev); // read to clear
        if(!(reg & GMAC_TSStatus_TSTARGT_Msk))
            TR("TS alarm flag not set??\n");
        else
            TR("TS alarm!!!!!!!!!!!!!!!!\n");
    }

    /* handle lpi status */
    if(mac_status_reg & GMAC_IntStatus_LPIIS_Msk) {
        reg = GMAC_GET_LPI_STATUS(gmacdev); // read to clear
    }

    /* handle rgmii status */
    if(mac_status_reg & GMAC_IntStatus_RGSMIIIS_Msk) {
        reg = GMAC_GET_RGMII_STATUS(gmacdev); // read to clear
    }

    GMAC_CLR_INT_SUMMARY(gmacdev, mac_status_reg);

    /*Read the Dma interrupt status to know whether the interrupt got generated by our device or not*/
    dma_status_reg = GMAC_GET_DMA_STATUS(gmacdev);

    if(dma_status_reg == 0)
        return;

    GMAC_disable_interrupt_all(gmacdev);

    TR("%s:Dma Status Reg: 0x%08x\n",__FUNCTION__,dma_status_reg);

    if(dma_status_reg & GMAC_DmaStatus_GPI_Msk) {
        TR("%s:: Interrupt due to PMT module\n",__FUNCTION__);
        GMAC_powerup_mac(gmacdev);
    }

    if(dma_status_reg & GMAC_DmaStatus_GLI_Msk) {
        TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
    }

    /*Now lets handle the DMA interrupts*/
    interrupt = GMAC_get_interrupt_type(gmacdev);
    TR("%s:Interrupts to be handled: 0x%08x\n",__FUNCTION__,interrupt);

    if(interrupt & GMACDmaError) {
        TR("%s::Fatal Bus Error Inetrrupt Seen\n",__FUNCTION__);
        GMAC_DMA_TX_DISABLE(gmacdev);
        GMAC_DMA_RX_DISABLE(gmacdev);

        GMAC_take_desc_ownership_tx(gmacdev);
        GMAC_take_desc_ownership_rx(gmacdev);

        GMAC_init_tx_rx_desc_queue(gmacdev);

        GMAC_reset(gmacdev); //reset the DMA engine and the GMAC ip

        GMAC_set_mac_addr(gmacdev, 0, gmacdev->Intf == 0 ? mac_addr0 : mac_addr1);
        GMAC_DMA_BUSMODE_INIT(gmacdev, GMAC_DmaBusMode_FB_Msk | DmaBurstLength8 | DmaDescriptorSkip0);
        GMAC_DMA_OPMODE_INIT(gmacdev, GMAC_DmaOpMode_TSF_Msk);

        GMAC_init_rx_desc_base(gmacdev);
        GMAC_init_tx_desc_base(gmacdev);
        GMAC_init(gmacdev);
        GMAC_DMA_RX_ENABLE(gmacdev);
        GMAC_DMA_TX_ENABLE(gmacdev);
    }

    if(interrupt & GMACDmaRxNormal) {
        TR("%s:: Rx Normal \n", __FUNCTION__);
        GMAC_handle_received_data(GMACINTF0);
    }

    if(interrupt & GMACDmaRxAbnormal) {
        TR("%s::Abnormal Rx Interrupt Seen\n",__FUNCTION__);
        gmacdev->NetStats.rx_over_errors++;

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_DMA_RX_PD_RESUME(gmacdev);//To handle GBPS with 12 descriptors
        }
    }

    if(interrupt & GMACDmaRxStopped) {
        TR("%s::Receiver stopped seeing Rx interrupts\n",__FUNCTION__); //Receiver gone in to stopped state

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            gmacdev->NetStats.rx_over_errors++;
            GMAC_DMA_RX_ENABLE(gmacdev);
        }
    }

    if(interrupt & GMACDmaTxNormal) {
        //xmit function has done its job
        TR("%s::Finished Normal Transmission \n",__FUNCTION__);
        GMAC_handle_transmit_over(GMACINTF0);//Do whatever you want after the transmission is over
    }

    if(interrupt & GMACDmaTxAbnormal) {
        TR("%s::Abnormal Tx Interrupt Seen\n",__FUNCTION__);

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_handle_transmit_over(GMACINTF0);
        }
    }

    if(interrupt & GMACDmaTxStopped) {
        TR("%s::Transmitter stopped sending the packets\n",__FUNCTION__);

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_DMA_TX_DISABLE(gmacdev);
            GMAC_take_desc_ownership_tx(gmacdev);

            GMAC_DMA_TX_ENABLE(gmacdev);
            TR("%s::Transmission Resumed\n",__FUNCTION__);
        }
    }

    /* Enable the interrupt before returning from ISR*/
    GMAC_enable_interrupt(gmacdev, DMA_INT_ENABLE);
}

/**
 * @brief Interrupt service routing for GMAC1.
 * This is the function registered as ISR for device interrupts.
 * @param[in] None
 * @return None
 * @note This function runs in interrupt context
 */
void GMAC_int_handler1(void)
{
    GMACdevice *gmacdev = &GMACdev[GMACINTF1];
    u32 interrupt, dma_status_reg, mac_status_reg;
    u32 dma_addr;
    u32 volatile reg;

    // Check GMAC interrupt
    mac_status_reg = GMAC_GET_INT_SUMMARY(gmacdev);

    /* handle ts status */
    if(mac_status_reg & GMAC_IntStatus_TSIS_Msk) {
        gmacdev->NetStats.ts_int = 1;
        reg = GMAC_GET_TS_STATUS(gmacdev); // read to clear
        if(!(reg & GMAC_TSStatus_TSTARGT_Msk))
            TR("TS alarm flag not set??\n");
        else
            TR("TS alarm!!!!!!!!!!!!!!!!\n");
    }

    /* handle lpi status */
    if(mac_status_reg & GMAC_IntStatus_LPIIS_Msk) {
        reg = GMAC_GET_LPI_STATUS(gmacdev); // read to clear
    }

    /* handle rgmii status */
    if(mac_status_reg & GMAC_IntStatus_RGSMIIIS_Msk) {
        reg = GMAC_GET_RGMII_STATUS(gmacdev); // read to clear
    }

    GMAC_CLR_INT_SUMMARY(gmacdev, mac_status_reg);

    /*Read the Dma interrupt status to know whether the interrupt got generated by our device or not*/
    dma_status_reg = GMAC_GET_DMA_STATUS(gmacdev);

    if(dma_status_reg == 0)
        return;

    GMAC_disable_interrupt_all(gmacdev);

    TR("%s:Dma Status Reg: 0x%08x\n",__FUNCTION__,dma_status_reg);

    if(dma_status_reg & GMAC_DmaStatus_GPI_Msk) {
        TR("%s:: Interrupt due to PMT module\n",__FUNCTION__);
        GMAC_powerup_mac(gmacdev);
    }

    if(dma_status_reg & GMAC_DmaStatus_GLI_Msk) {
        TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
    }

    /*Now lets handle the DMA interrupts*/
    interrupt = GMAC_get_interrupt_type(gmacdev);
    TR("%s:Interrupts to be handled: 0x%08x\n",__FUNCTION__,interrupt);

    if(interrupt & GMACDmaError) {
        TR("%s::Fatal Bus Error Inetrrupt Seen\n",__FUNCTION__);
        GMAC_DMA_TX_DISABLE(gmacdev);
        GMAC_DMA_RX_DISABLE(gmacdev);

        GMAC_take_desc_ownership_tx(gmacdev);
        GMAC_take_desc_ownership_rx(gmacdev);

        GMAC_init_tx_rx_desc_queue(gmacdev);

        GMAC_reset(gmacdev); //reset the DMA engine and the GMAC ip

        GMAC_set_mac_addr(gmacdev, 0, gmacdev->Intf == 0 ? mac_addr0 : mac_addr1);
        GMAC_DMA_BUSMODE_INIT(gmacdev, GMAC_DmaBusMode_FB_Msk | DmaBurstLength8 | DmaDescriptorSkip0);
        GMAC_DMA_OPMODE_INIT(gmacdev, GMAC_DmaOpMode_TSF_Msk);

        GMAC_init_rx_desc_base(gmacdev);
        GMAC_init_tx_desc_base(gmacdev);
        GMAC_init(gmacdev);
        GMAC_DMA_RX_ENABLE(gmacdev);
        GMAC_DMA_TX_ENABLE(gmacdev);
    }

    if(interrupt & GMACDmaRxNormal) {
        TR("%s:: Rx Normal \n", __FUNCTION__);
        GMAC_handle_received_data(GMACINTF1);
    }

    if(interrupt & GMACDmaRxAbnormal) {
        TR("%s::Abnormal Rx Interrupt Seen\n",__FUNCTION__);
        gmacdev->NetStats.rx_over_errors++;

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_DMA_RX_PD_RESUME(gmacdev);//To handle GBPS with 12 descriptors
        }
    }

    if(interrupt & GMACDmaRxStopped) {
        TR("%s::Receiver stopped seeing Rx interrupts\n",__FUNCTION__); //Receiver gone in to stopped state

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            gmacdev->NetStats.rx_over_errors++;
            GMAC_DMA_RX_ENABLE(gmacdev);
        }
    }

    if(interrupt & GMACDmaTxNormal) {
        //xmit function has done its job
        TR("%s::Finished Normal Transmission \n",__FUNCTION__);
        GMAC_handle_transmit_over(GMACINTF1);//Do whatever you want after the transmission is over
    }

    if(interrupt & GMACDmaTxAbnormal) {
        TR("%s::Abnormal Tx Interrupt Seen\n",__FUNCTION__);

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_handle_transmit_over(GMACINTF1);
        }
    }

    if(interrupt & GMACDmaTxStopped) {
        TR("%s::Transmitter stopped sending the packets\n",__FUNCTION__);

        if(GMAC_Power_down == 0) {	// If Mac is not in powerdown
            GMAC_DMA_TX_DISABLE(gmacdev);
            GMAC_take_desc_ownership_tx(gmacdev);

            GMAC_DMA_TX_ENABLE(gmacdev);
            TR("%s::Transmission Resumed\n",__FUNCTION__);
        }
    }

    /* Enable the interrupt before returning from ISR*/
    GMAC_enable_interrupt(gmacdev, DMA_INT_ENABLE);
}
