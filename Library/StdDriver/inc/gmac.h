/**************************************************************************//**
 * @file     gmac.h
 * @brief    GMAC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __GMAC_H__
#define __GMAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup GMAC_Driver GMAC Driver
  @{
*/

/** @addtogroup GMAC_EXPORTED_CONSTANTS GMAC Exported Constants
  @{
*/

//#define DEBUG
#ifdef DEBUG
#undef TR
#define TR(fmt, args...) sysprintf("GMAC: " fmt, ##args)
#else
#define TR(fmt, args...) /* not debugging: nothing */
#endif

#define CACHE_ON

/******************************************************************************
 * MAC
 ******************************************************************************/
#define GMAC_CNT	2

enum GMACINTF {
    GMACINTF0 = 0,
    GMACINTF1 = 1,
};

enum GMACPhyBase {
    PHY0  = 0,          //The device can support 32 phys, but we use first phy only
    PHY1  = 1,
    PHY31 = 31,
};

enum MIIMODE {
    RGMII_1G = 0,
    RMII_100M = 1,
    RMII_10M = 2,
};

#define DEFAULT_PHY_BASE        PHY0

#define DEFAULT_DELAY_VARIABLE  10
#define DEFAULT_LOOP_VARIABLE   10000

#define TRANSMIT_DESC_SIZE      32 //Tx Descriptors needed in the Descriptor pool/queue
#define RECEIVE_DESC_SIZE       32 //Rx Descriptors needed in the Descriptor pool/queue

/******************************************************************************
 * Descriptor
 ******************************************************************************/
#define MODULO_INTERRUPT   1 // if it is set to 1, interrupt is available for all the descriptors or else interrupt is available only for
// descriptor whose index%MODULO_INTERRUPT is zero
typedef struct DmaDescStruct {
    u32   status;         /* Status                                                                             */
    u32   length;         /* Buffer 1  and Buffer 2 length                                                      */
    u32   buffer1;        /* Network Buffer 1 pointer (Dma-able)                                                */
    u32   buffer2;        /* Network Buffer 2 pointer or next descriptor pointer (Dma-able)in chain structure   */
    /* This data below is used only by driver                                                                   */
    u32   extstatus;      /* Extended status of a Rx Descriptor                                                 */
    u32   reserved1;      /* Reserved word                                                                      */
    u32   timestamplow;   /* Lower 32 bits of the 64 bit timestamp value                                        */
    u32   timestamphigh;  /* Higher 32 bits of the 64 bit timestamp value                                       */
    //u32   data1;        /* This holds virtual address of buffer1, not used by DMA                             */
    //u32   data2;        /* This holds virtual address of buffer2, not used by DMA                             */
} DmaDesc;

enum DescMode {
    RINGMODE    = 0x00000001,
    CHAINMODE   = 0x00000002,
};

enum BufferMode {
    SINGLEBUF   = 0x00000001,
    DUALBUF     = 0x00000002,
};

typedef u32 *       dma_addr_t;

enum DmaDescriptorStatus    /* status word of DMA descriptor */
{
    /********** TDES0 **********/
    /* Normal */
    DescOwnByDma          = 0x80000000,   /* (OWN)Descriptor is owned by DMA engine              31      RW                 */
    DescTxTSStatus        = 0x00020000,   /* (TTSS) Transmit Timestamp Status                    17                         */
    DescTxIpv4ChkError    = 0x00010000,   /* (IHE) Tx Ip header error                            16                         */
    DescError             = 0x00008000,   /* (ES)Error summary bit  - OR of the follo. bits:     15                         */
    DescTxTimeout         = 0x00004000,   /* (JT)Tx - Transmit jabber timeout                    14                         */
    DescTxFrameFlushed    = 0x00002000,   /* (FF)Tx - DMA/MTL flushed the frame due to SW flush  13                         */
    DescTxPayChkError     = 0x00001000,   /* (PCE) Tx Payload checksum Error                     12                         */
    DescTxLostCarrier     = 0x00000800,   /* (LC)Tx - carrier lost during tramsmission           11                         */
    DescTxNoCarrier       = 0x00000400,   /* (NC)Tx - no carrier signal from the tranceiver      10                         */
    DescTxLateCollision   = 0x00000200,   /* (LC)Tx - transmission aborted due to collision      9                          */
    DescTxExcCollisions   = 0x00000100,   /* (EC)Tx - transmission aborted after 16 collisions   8                          */
    DescTxVLANFrame       = 0x00000080,   /* (VF)Tx - VLAN-type frame                            7                          */
    DescTxCollMask        = 0x00000078,   /* (CC)Tx - Collision count                            6:3                        */
    DescTxCollShift       = 3,
    DescTxExcDeferral     = 0x00000004,   /* (ED)Tx - excessive deferral                         2                          */
    DescTxUnderflow       = 0x00000002,   /* (UF)Tx - late data arrival from the memory          1                          */
    DescTxDeferred        = 0x00000001,   /* (DB)Tx - frame transmision deferred                 0                          */
    /* Enhanced */
    eDescTxIntOnCompl     = 0x40000000,  /* (IC)Tx - interrupt on completion                    30                         */
    eDescTxLastSeg        = 0x20000000,  /* (LS)Tx - Last segment of the frame                  29                         */
    eDescTxFirstSeg       = 0x10000000,  /* (FS)Tx - First segment of the frame                 28                         */
    eDescTxDisableCrc     = 0x08000000,  /* (DC)Tx - Add CRC disabled (first segment only)      27                         */
    eDescTxDisablePadd    = 0x04000000,  /* (DP)disable padding, added by - reyaz               26                         */
    eDescTxTSEnable       = 0x02000000,  /* (TTSE) Transmit Timestamp Enable                    25                         */
    eDescTxCrcReplacement = 0x01000000,  /* (CRCR) CRC Replacement Control                      24                         */
    eDescTxCisMask        = 0x00c00000,  /* (CIC)Tx checksum offloading control mask            23:22                      */
    eDescTxCisBypass      = 0x00000000,  /*   |- Checksum bypass                                                           */
    eDescTxCisIpv4HdrCs   = 0x00400000,  /*   |- IPv4 header checksum                                                      */
    eDescTxCisTcpOnlyCs   = 0x00800000,  /*   |- TCP/UDP/ICMP checksum. Pseudo header checksum is assumed to be present    */
    eDescTxCisTcpPseudoCs = 0x00c00000,  /*   '- TCP/UDP/ICMP checksum fully in hardware including pseudo header           */
    eDescTxEndOfRing      = 0x00200000,  /* (TER)End of descriptors ring                        21                         */
    eDescTxChain          = 0x00100000,  /* (TCH)Second buffer address is chain address         20                         */
    eDescTxVlanCtrl       = 0x000C0000,  /* (VLIC)Perform VLAN tagging or untagging             19:18                      */
    eDescTxVlanBypass     = 0x00000000,  /*   |- Do not add VLAN tag                                                       */
    eDescTxVlanRemove     = 0x00040000,  /*   |- Remove VLAN tag before transmission                                       */
    eDescTxVlanInsert     = 0x00080000,  /*   |- Insert VLAN tag                                                           */
    eDescTxVlanReplace    = 0x000C0000,  /*   '- Replace with VLAN tag                                                     */
    /********** TDES1 **********/
    /* Normal */
    nDescTxIntOnCompl     = 0x80000000,   /* (IC)   Sets TxINT on transmition complete           31                         */
    nDescTxLastSeg        = 0x40000000,   /* (LS)   Indicates the last segment of the frame      30                         */
    nDescTxFirstSeg       = 0x20000000,   /* (FS)   Indicates the first segment of the frame     29                         */
    nDescTxCsInsCtrl      = 0x18000000,   /* (CIC)  Checksum insertion control                   28:27                      */
    nDescTxDisableCRC     = 0x04000000,   /* (DC)   Disable CRC                                  26                         */
    nDescTxEndOfRing      = 0x02000000,   /* (TER)  Indicates desc reach end of its final        25                         */
    nDescTxBuf2Chained    = 0x01000000,   /* (TCH)  Use next desc address rather than buffer2    24                         */
    nDescTxDisablePad     = 0x00800000,   /* (DP)   Disable automatic padding                    23                         */
    nDescTxTSEnable       = 0x00400000,   /* (TTSE) Enable Tx timestamp                          22                         */
    nDescSize2Mask        = 0x003FF800,   /* (TBS2) Buffer 2 size                                21:11                      */
    nDescSize2Shift       = 11,
    nDescSize1Mask        = 0x000007FF,   /* (TBS1) Buffer 1 size                                10:0                       */
    nDescSize1Shift       = 0,
    /* Enhanced */
    eDescSAInsCtrl        = 0xE0000000,   /* (SAIC) Add or replace the Source Address field      31:29                      */
    eDescSize2Mask        = 0x1FFF0000,   /* (TBS2) Buffer 2 size                                28:16                      */
    eDescSize2Shift       = 16,
    eDescSize1Mask        = 0x00001FFF,   /* (TBS1) Buffer 1 size                                12:0                       */
    eDescSize1Shift       = 0,

    /********** TDES2 **********/
    /* Buffer1 address pointer */

    /********** TDES3 **********/
    /* Buffer2 address pointer or next descriptor address */

    /********** TDES6 **********/
    /* Timstamp low */

    /********** TDES7 **********/
    /* Timstamp high */

    /********** RDES0 **********/
    /* DescOwnByDma       = 0x80000000,      (OWN)Descriptor is owned by DMA engine              31      RW                 */
    DescRxDAFilterFail    = 0x40000000,   /* (AFM)Rx - DA Filter Fail for the rx frame           30                         */
    DescRxFrameLengthMask = 0x3FFF0000,   /* (FL)Receive descriptor frame length                 29:16                      */
    /* DescError          = 0x00008000,      (ES)Error summary bit  - OR of the follo. bits:     15                         */
    DescRxFrameLengthShift= 16,
    /*  DE || OE || IPC || LC || RWT || RE || CE */
    DescRxTruncated       = 0x00004000,   /* (DE)Rx - no more descriptors for receive frame      14                         */
    DescRxSAFilterFail    = 0x00002000,   /* (SAF)Rx - SA Filter Fail for the received frame     13                         */
    DescRxLengthError     = 0x00001000,   /* (LE)Rx - frm size not matching with len field       12                         */
    DescRxDamaged         = 0x00000800,   /* (OE)Rx - frm was damaged due to buffer overflow     11                         */
    DescRxVLANTag         = 0x00000400,   /* (VLAN)Rx - received frame is a VLAN frame           10                         */
    DescRxFirst           = 0x00000200,   /* (FS)Rx - first descriptor of the frame              9                          */
    DescRxLast            = 0x00000100,   /* (LS)Rx - last descriptor of the frame               8                          */
    DescRxTSavail         = 0x00000080,   /* Time stamp available                                7                          */
    DescRxLongFrame       = 0x00000080,   /* (Giant Frame)Rx - frame is longer than 1518/1522    7                          */
    DescRxTSAvailable     = 0x00000080,   /* Share bit with (Giant Frame)Rx                      7                          */
    DescRxChkBit7         = 0x00000080,   /* (IPC CS ERROR)Rx - Ipv4 header checksum error       7                          */
    DescRxCollision       = 0x00000040,   /* (LC)Rx - late collision occurred during reception   6                          */
    DescRxChkBit5         = 0x00000020,   /* (FT)Rx - Frame type - Ethernet, otherwise 802.3     5                          */
    DescRxWatchdog        = 0x00000010,   /* (RWT)Rx - watchdog timer expired during reception   4                          */
    DescRxMiiError        = 0x00000008,   /* (RE)Rx - error reported by MII interface            3                          */
    DescRxDribbling       = 0x00000004,   /* (DE)Rx - frame contains non int multiple of 8 bits  2                          */
    DescRxCrc             = 0x00000002,   /* (CE)Rx - CRC error                                  1                          */
    /* Normal */
    nDescRxChkBit0        = 0x00000001,   /* () Rx - Rx Payload Checksum Error                   0                          */
    /* Enhanced */
    eDescRxEXTsts         = 0x00000001,   /* () Extended(RDES4) Status Available                 0                          */

    /********** RDES1 **********/
    /* Normal */
    DescRxDisIntCompl     = 0x80000000,   /* (DIC)Disable Rx int on completion                   31                         */
    nDescRxEndOfRing      = 0x02000000,   /* (RER)End of descriptors ring                        25                         */
    nDescRxChain          = 0x01000000,   /* (RCH)Second buffer address is chain address         24                         */
    /* nDescSize2Mask     = 0x003FF800,      (RBS2) Buffer 2 size                                21:11                      */
    /* nDescSize1Mask     = 0x000007FF,      (RBS1) Buffer 1 size                                10:0                       */
    /* Enhanced */
    /* eDescSize2Mask     = 0x1FFF0000,   /* (RBS2) Buffer 2 size                                28:16                      */
    eDescRxEndOfRing      = 0x00008000,   /* (RER)End of descriptors ring                        15                         */
    eDescRxChain          = 0x00004000,   /* (RCH)Second buffer address is chain address         14                         */
    /* eDescSize1Mask     = 0x00001FFF,   /* (RBS1) Buffer 1 size                                12:0                       */

    /********** RDES2 **********/
    /* Buffer1 address pointer */

    /********** RDES3 **********/
    /* Buffer2 address pointer or next descriptor address */

    /********** RDES4 **********/
    eDescRxPtpAvail       = 0x00004000,   /* PTP snapshot available                              14                         */
    eDescRxPtpVer         = 0x00002000,   /* When set indicates IEEE1584 Version 2 (else Ver1)   13                         */
    eDescRxPtpFrameType   = 0x00001000,   /* PTP frame type Indicates PTP sent over ethernet     12                         */
    eDescRxPtpMessageType = 0x00000F00,   /* Message Type                                        11:8                       */
    eDescRxPtpNo          = 0x00000000,   /*  |- 0000 => No PTP message received                                            */
    eDescRxPtpSync        = 0x00000100,   /*  |- 0001 => Sync (all clock types) received                                    */
    eDescRxPtpFollowUp    = 0x00000200,   /*  |- 0010 => Follow_Up (all clock types) received                               */
    eDescRxPtpDelayReq    = 0x00000300,   /*  |- 0011 => Delay_Req (all clock types) received                               */
    eDescRxPtpDelayResp   = 0x00000400,   /*  |- 0100 => Delay_Resp (all clock types) received                              */
    eDescRxPtpPdelayReq   = 0x00000500,   /*  |- 0101 => Pdelay_Req (in P to P tras clk)  or Announce in Ord and Bound clk  */
    eDescRxPtpPdelayResp  = 0x00000600,   /*  |- 0110 => Pdealy_Resp(in P to P trans clk) or Management in Ord and Bound clk*/
    eDescRxPtpPdelayRespFP= 0x00000700,   /*  '- 0111 => Pdealy_Resp_Follow_Up (in P to P trans clk) or Signaling in Ord and Bound clk   */
    eDescRxPtpIPV6        = 0x00000080,   /* Received Packet is  in IPV6 Packet                  7                          */
    eDescRxPtpIPV4        = 0x00000040,   /* Received Packet is  in IPV4 Packet                  6                          */

    eDescRxChkSumBypass   = 0x00000020,   /* When set indicates checksum offload engine is bypassed 5                       */
    eDescRxIpPayloadError = 0x00000010,   /* When set indicates 16bit IP payload CS is in error  4                          */
    eDescRxIpHeaderError  = 0x00000008,   /* When set indicates 16bit IPV4 header CS is in       3
                                             error or IP datagram version is not consistent
                                             with Ethernet type value                                                       */
    eDescRxIpPayloadType  = 0x00000007,   /* ()Indicate the type of payload processed by COE    2:0                         */
    eDescRxIpPayloadUnknown= 0x00000000,  /*  |- Unknown or didnot process IP payload                                       */
    eDescRxIpPayloadUDP   = 0x00000001,   /*  |- UDP                                                                        */
    eDescRxIpPayloadTCP   = 0x00000002,   /*  |- TCP                                                                        */
    eDescRxIpPayloadICMP  = 0x00000003,   /*  '- ICMP                                                                       */

    /********** RDES6 **********/
    /* Timstamp low */

    /********** RDES7 **********/
    /* Timstamp high */
};

/* RDES0 encode if COE(Config_IPC) enabled (type2) */
enum RxDescCOEEncode {
    RxLenLT600              = 0,    /* Bit(5:7:0)=>0 IEEE 802.3 type frame Length field is Lessthan 0x0600                  */
    RxIpHdrPayLoadChkBypass = 1,    /* Bit(5:7:0)=>1 Payload & Ip header checksum bypassed (unsuppported payload)           */
    RxIpHdrPayLoadRes       = 2,    /* Bit(5:7:0)=>2 Reserved                                                               */
    RxChkBypass             = 3,    /* Bit(5:7:0)=>3 Neither IPv4 nor IPV6. So checksum bypassed                            */
    RxNoChkError            = 4,    /* Bit(5:7:0)=>4 No IPv4/IPv6 Checksum error detected                                   */
    RxPayLoadChkError       = 5,    /* Bit(5:7:0)=>5 Payload checksum error detected for Ipv4/Ipv6 frames                   */
    RxIpHdrChkError         = 6,    /* Bit(5:7:0)=>6 Ip header checksum error detected for Ipv4 frames                      */
    RxIpHdrPayLoadChkError  = 7,    /* Bit(5:7:0)=>7 Payload & Ip header checksum error detected for Ipv4/Ipv6 frames       */
};

/******************************************************************************
 * DMA
 ******************************************************************************/
enum DmaThresholdAct { // GMAC_DmaOpMode_RFA_Msk
    DmaThresdholdAct1K      = 0 << GMAC_DmaOpMode_RFA_Pos,      /* Full - 1Kbytes */
    DmaThresdholdAct2K      = 1 << GMAC_DmaOpMode_RFA_Pos,      /* Full - 2Kbytes */
    DmaThresdholdAct3K      = 2 << GMAC_DmaOpMode_RFA_Pos,      /* Full - 3Kbytes */
    DmaThresdholdAct4K      = 3 << GMAC_DmaOpMode_RFA_Pos,      /* Full - 4Kbytes */
};

enum DmaThresholdDeact { // GMAC_DmaOpMode_RFD_Msk
    DmaThresdholdDeact1K    = 0 << GMAC_DmaOpMode_RFD_Pos,      /* Full - 1Kbytes */
    DmaThresdholdDeact2K    = 1 << GMAC_DmaOpMode_RFD_Pos,      /* Full - 2Kbytes */
    DmaThresdholdDeact3K    = 2 << GMAC_DmaOpMode_RFD_Pos,      /* Full - 3Kbytes */
    DmaThresdholdDeact4K    = 3 << GMAC_DmaOpMode_RFD_Pos,      /* Full - 4Kbytes */
};

enum GmacPBLReg { // GMAC_DmaBusMode_PBL_Msk
    DmaBurstLength32       = 0x20 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 32 */
    DmaBurstLength16       = 0x10 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 16 */
    DmaBurstLength8        = 0x08 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 8  */
    DmaBurstLength4        = 0x04 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 4  */
    DmaBurstLength2        = 0x02 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 2  */
    DmaBurstLength1        = 0x01 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 1  */
    DmaBurstLength0        = 0x00 << GMAC_DmaBusMode_PBL_Pos,   /* Dma burst length = 0  */
};

enum GmacDSLReg { // GMAC_DmaBusMode_DSL_Msk
    DmaDescriptorSkip16    = 0x10 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 16 words */
    DmaDescriptorSkip8     = 0x08 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 8 words */
    DmaDescriptorSkip4     = 0x04 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 4 words */
    DmaDescriptorSkip2     = 0x02 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 2 words */
    DmaDescriptorSkip1     = 0x01 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 1 words */
    DmaDescriptorSkip0     = 0x00 << GMAC_DmaBusMode_DSL_Pos,   /* Skip length = 0 words */
};

enum GmacRTCReg { // GMAC_DmaOpMode_RTC_Msk
    DmaRxThreshCtrl128     = 3 << GMAC_DmaOpMode_RTC_Pos,       /* Receive threshold = 128 bytes */
    DmaRxThreshCtrl96      = 2 << GMAC_DmaOpMode_RTC_Pos,       /* Receive threshold = 96 bytes */
    DmaRxThreshCtrl32      = 1 << GMAC_DmaOpMode_RTC_Pos,       /* Receive threshold = 32 bytes */
    DmaRxThreshCtrl64      = 0 << GMAC_DmaOpMode_RTC_Pos,       /* Receive threshold = 64 bytes */
};

enum usrDMAIntHandle {
    GMACDmaRxNormal        = 0x01,   /* normal receiver interrupt        */
    GMACDmaRxAbnormal      = 0x02,   /* abnormal receiver interrupt      */
    GMACDmaRxStopped       = 0x04,   /* receiver stopped                 */
    GMACDmaTxNormal        = 0x08,   /* normal transmitter interrupt     */
    GMACDmaTxAbnormal      = 0x10,   /* abnormal transmitter interrupt   */
    GMACDmaTxStopped       = 0x20,   /* transmitter stopped              */
    GMACDmaError           = 0x80,   /* Dma engine error                 */
};

#define DMA_INT_ENABLE     (GMAC_DmaInt_NIE_Msk | GMAC_DmaInt_AIE_Msk | GMAC_DmaInt_FBE_Msk |\
                            GMAC_DmaInt_RSE_Msk | GMAC_DmaInt_RUE_Msk | GMAC_DmaInt_RIE_Msk |\
                            GMAC_DmaInt_UNE_Msk | GMAC_DmaInt_TSE_Msk | GMAC_DmaInt_TIE_Msk)


/******************************************************************************
 * Ethernet
 ******************************************************************************/
#define ETHERNET_HEADER             14  //6 byte Dest addr, 6 byte Src addr, 2 byte length/type
#define ETHERNET_CRC                 4  //Ethernet CRC
#define ETHERNET_EXTRA               2  //Only God knows about this?????
#define ETHERNET_PACKET_COPY       250  // Maximum length when received data is copied on to a new skb
#define ETHERNET_PACKET_EXTRA       18  // Preallocated length for the rx packets is MTU + ETHERNET_PACKET_EXTRA
#define VLAN_TAG                     4  //optional 802.1q VLAN Tag
#define MIN_ETHERNET_PAYLOAD        46  //Minimum Ethernet payload size
#define MAX_ETHERNET_PAYLOAD      1500  //Maximum Ethernet payload size
#define JUMBO_FRAME_PAYLOAD       9000  //Jumbo frame payload size

#define TX_BUF_SIZE        ETHERNET_HEADER + ETHERNET_CRC + MAX_ETHERNET_PAYLOAD + VLAN_TAG

/******************************************************************************
 * IEEE 1588-2008 Precision Time Protocol (PTP) Support
 ******************************************************************************/
enum PTPMessageType {
    SYNC                   = 0x0,
    Delay_Req              = 0x1,
    Pdelay_Req             = 0x2,
    Pdelay_Resp            = 0x3,
    Follow_up              = 0x8,
    Delay_Resp             = 0x9,
    Pdelay_Resp_Follow_Up  = 0xA,
    Announce               = 0xB,
    Signaling              = 0xC,
    Management             = 0xD,
};

enum TSSnapType {
    GmacTSOrdClk          = 0x00000000,     /* 00=> Ordinary clock*/
    GmacTSBouClk          = 0x00010000,     /* 01=> Boundary clock*/
    GmacTSEtoEClk         = 0x00020000,     /* 10=> End-to-End transparent clock*/
    GmacTSPtoPClk         = 0x00030000,     /* 11=> P-to-P transparent clock*/
};

/******************************************************************************
 * VLAN
 ******************************************************************************/
enum GmacVLANIncRepReg {
    GmacSVLAN		       = 0x00080000,
    GmacCVLAN		       = 0x00000000,
    GmacVLP			       = 0x00040000,
    GmacVLANNoACT	       = 0x00000000,
    GmacVLANDel		       = 0x00010000,
    GmacVLANIns		       = 0x00020000,
    GmacVLANRep		       = 0x00030000,
};

/******************************************************************************
 * PHY
 ******************************************************************************/

/* Below is "RTL8211 Integrated 10/100/1000 Gigabit Ethernet Transceiver"
 * Register and their layouts. This Phy has been used in the Dot Aster GMAC Phy daughter.
 * Since the Phy register map is standard, this map hardly changes to a different Phy
 */
enum MiiRegisters {
    PHY_CONTROL_REG           = 0x0000,     /*Control Register*/
    PHY_STATUS_REG            = 0x0001,     /*Status Register */
    PHY_ID1_REG               = 0x0002,     /*PHY Identifier Register 1*/
    PHY_ID2_REG               = 0x0003,     /*PHY Identifier Register 2*/
    PHY_AN_ADV_REG            = 0x0004,     /*Auto-Negotiation Advertisement Register*/
    PHY_AN_LNK_PART_ABL_REG   = 0x0005,     /*Link Partner Ability Register (Base Page)*/
    PHY_AN_EXP_REG            = 0x0006,     /*Auto-Negotiation Expansion Register*/
    PHY_AN_NXT_PAGE_TX_REG    = 0x0007,     /*Next Page Transmit Register*/
    PHY_AN_NXT_PAGE_RX_REG    = 0x0008,     /*Next Page Receive Register*/
    PHY_1000BT_CTRL_REG       = 0x0009,     /*1000BASE-T Control Register*/
    PHY_1000BT_STATUS_REG     = 0x000a,     /*1000BASE-T Status Register*/
    PHY_1000BT_EXT_STATUS_REG = 0x000f,     /*1000BASE-T Extended Status Register*/
    /* Page 0xa42 */
    PHY_INTERRUPT_ENABLE_REG  = 0x0012,     /*Phy interrupt enable register*/
    /* Page 0xa43 */
    PHY_SPECIFIC_CTRL1_REG    = 0x0018,     /*Phy specific control register 1*/
    PHY_SPECIFIC_CTRL2_REG    = 0x0019,     /*Phy specific control register 2*/
    PHY_SPECIFIC_STATUS_REG   = 0x001a,     /*Phy specific status register*/
    PHY_INTERRUPT_STATUS_REG  = 0x001D,     /*Phy interrupt status register*/
    PHY_PAGE_SELECT_REG       = 0x001F,     /*Page select register*/
    /* Page 0xd04 */
    PHY_LED_CONTROL           = 0x0010,     /*LED Control*/
};

enum Mii_GEN_CTRL { /* PHY_CONTROL_REG */
    /*  Description                bits        R/W  default value  */
    Mii_reset                = 0x8000,       /* Reset PHY                    15      RW     */
    Mii_Loopback             = 0x4000,       /* Enable Loop back             14      RW     */
    Mii_NoLoopback           = 0x0000,       /* Disable Loop back            14      RW     */
    Mii_Speed_mask           = 0x2040,
    Mii_Speed_10             = 0x0000,       /* 10   Mbps                    6:13    RW     */
    Mii_Speed_100            = 0x2000,       /* 100  Mbps                    6:13    RW     */
    Mii_Speed_1000           = 0x0040,       /* 1000 Mbit/s                  6:13    RW     */
    Mii_AN_enable            = 0x1000,       /* Enable auto-negotiation      12      RW     */
    Mii_Power_down           = 0x0800,       /* Power down                   11      RW     */
    Mii_Restart_AN           = 0x0200,       /* Restart auto-negotiation     9       RW     */
    Mii_Duplex               = 0x0100,       /* Full Duplex mode             8       RW     */
};

enum Mii_GEN_STATUS { /* PHY_STATUS_REG */
    Mii_Cap_100_full         = 0x4000,   /* 100 Base-TX full duplex capable  14      RO     */
    Mii_Cap_100_half         = 0x2000,   /* 100 Base-TX half duplex capable  13      RO     */
    Mii_Cap_10_full          = 0x1000,   /* 10 Base-T full duplex capable    12      RO     */
    Mii_Cap_10_half          = 0x0800,   /* 10 Base-T half duplex capable    11      RO     */
    Mii_Ext_status           = 0x0100,   /* Support extend statusreg  0xF    8       RO     */
    Mii_AutoNegCmplt         = 0x0020,   /* Autonegotiation completed        5       RO     */
    Mii_Link                 = 0x0004,   /* Link status                      2       RO     */
};

enum Mii_ADV { /* PHY_AN_ADV_REG / PHY_AN_LNK_PART_ABL_REG */
    Mii_Adv_100_full         = 0x0100,
    Mii_Adv_100_half         = 0x0080,
    Mii_Adv_10_full          = 0x0040,
    Mii_Adv_10_half          = 0x0020,
};

enum GMii_1000BT_CTRL { /* PHY_1000BT_CTRL_REG */
    Mii_Adv_1000_full        = 0x0200,
};

enum GMii_1000BT_STATUS { /* PHY_1000BT_STATUS_REG */
    Mii_partner_1000_full    = 0x0800, // not valid until 6.1 is 1
};

enum Mii_Link_Status {
    LINKDOWN    = 0,
    LINKUP      = 1,
};

enum Mii_Duplex_Mode {
    HALFDUPLEX = 1,
    FULLDUPLEX = 2,
};

enum Mii_Link_Speed {
    SPEED1000   = RGMII_1G,
    SPEED100    = RMII_100M,
    SPEED10     = RMII_10M,
};

enum Mii_Loop_Back {
    NOLOOPBACK  = 0,
    LOOPBACK    = 1,
};

/******************************************************************************
 * Application
 ******************************************************************************/
/* Error Codes */
#define GMACNOERR   0
#define GMACNOMEM   1
#define GMACPHYERR  2
#define GMACBUSY    3

enum GmacSAInsOrReplReg { // GMAC_Config_SARC_Msk
    GmacSAInstAddr0         = 2 << GMAC_Config_SARC_Pos,      /* MAC insert by Address0(reg 16,17) */
    GmacSAInstAddr1         = 6 << GMAC_Config_SARC_Pos,      /* MAC insert by Address1(reg 18,19) */
    GmacSAReplAddr0         = 3 << GMAC_Config_SARC_Pos,      /* MAC replace with Address0(reg 16,17) */
    GmacSAReplAddr1         = 7 << GMAC_Config_SARC_Pos,      /* MAC replace with Address1(reg 18,19) */
};

enum GmacBackoffLimitReg { // GMAC_Config_BL_Msk
    GmacBackoffLimit0       = 0 << GMAC_Config_BL_Pos,      /* k is the min of (n, 10) */
    GmacBackoffLimit1       = 1 << GMAC_Config_BL_Pos,      /* k is the min of (n, 8) */
    GmacBackoffLimit2       = 2 << GMAC_Config_BL_Pos,      /* k is the min of (n, 4) */
    GmacBackoffLimit3       = 3 << GMAC_Config_BL_Pos,      /* k is the min of (n, 1) */
};

enum GmacPassControlReg { // GMAC_FrameFilter_PCF_Msk
    GmacPassControl0       = 0 << GMAC_FrameFilter_PCF_Pos, /* Does not pass control frames                   */
    GmacPassControl1       = 1 << GMAC_FrameFilter_PCF_Pos, /* Pass control frames except pause control frame */
    GmacPassControl2       = 2 << GMAC_FrameFilter_PCF_Pos, /* Pass all control frames                        */
    GmacPassControl3       = 3 << GMAC_FrameFilter_PCF_Pos, /* Pass control frames that pass address filter   */
};

enum GmacGmiiAddrReg { // GMAC_GmiiAddr_CR_Msk
    GmiiCsrClk0            = 0 << GMAC_GmiiAddr_CR_Pos,     /* 60-100 MHz  */
    GmiiCsrClk1            = 1 << GMAC_GmiiAddr_CR_Pos,     /* 100-150 MHz */
    GmiiCsrClk2            = 2 << GMAC_GmiiAddr_CR_Pos,     /* 20-35 MHz   */
    GmiiCsrClk3            = 3 << GMAC_GmiiAddr_CR_Pos,     /* 35-60 MHz   */
    GmiiCsrClk4            = 4 << GMAC_GmiiAddr_CR_Pos,     /* 150-250 MHz */
    GmiiCsrClk5            = 5 << GMAC_GmiiAddr_CR_Pos,     /* 250-300 MHz */
};

struct sk_buff {
    unsigned char data[2048];
    unsigned int len;
    unsigned int volatile rdy;
};

struct net_device_stats {
    u32 tx_bytes;
    u32 tx_packets;
    u32 tx_errors;
    u32 tx_aborted_errors;
    u32 tx_carrier_errors;
    u32 tx_ip_header_errors;
    u32 tx_ip_payload_errors;
    u32 collisions;
    u32 rx_bytes;
    u32 rx_packets;
    u32 rx_errors;
    u32 rx_crc_errors;
    u32 rx_frame_errors;
    u32 rx_length_errors;
    u32 rx_dropped;
    u32 rx_over_errors;
    u32 rx_ip_header_errors;
    u32 rx_ip_payload_errors;
    volatile u32 ts_int;
};

typedef struct GMACDeviceStruct {
    GMAC_T *MacBase;                 /* base address of MAC registers           */
    u64     PhyBase;                 /* PHY device address on MII interface     */
    u64     Version;                 /* Gmac Revision version                   */
    u32     Intf;
    u32     ClockDivMdc;
    u32     LinkState;
    u32     DuplexMode;
    u32     Speed;
    u32     LoopbackMode;
    struct net_device_stats NetStats;

    /* Descriptor */
    DmaDesc *TxDescDma;              /* Dma-able address of first tx descriptor either in ring or chain mode, this is used by the GMAC device*/
    DmaDesc *RxDescDma;              /* Dma-albe address of first rx descriptor either in ring or chain mode, this is used by the GMAC device*/
    DmaDesc *TxDesc;                 /* start address of TX descriptors ring or chain, this is used by the driver  */
    DmaDesc *RxDesc;                 /* start address of RX descriptors ring or chain, this is used by the driver  */
    u32     RxDescCount;             /* number of rx descriptors in the tx descriptor queue/pool */
    u32     TxDescCount;             /* number of tx descriptors in the rx descriptor queue/pool */
    u32     BusyTxDesc;              /* Number of Tx Descriptors owned by DMA at any given time*/
    u32     BusyRxDesc;              /* Number of Rx Descriptors owned by DMA at any given time*/
    u32     TxBusy;                  /* index of the tx descriptor owned by DMA, is obtained by GMAC_get_tx_qptr()                */
    u32     TxNext;                  /* index of the tx descriptor next available with driver, given to DMA by GMAC_set_tx_qptr() */
    u32     RxBusy;                  /* index of the rx descriptor owned by DMA, obtained by GMAC_get_rx_qptr()                   */
    u32     RxNext;                  /* index of the rx descriptor next available with driver, given to DMA by GMAC_set_rx_qptr() */
    DmaDesc *TxBusyDesc;             /* Tx Descriptor address corresponding to the index TxBusy */
    DmaDesc *TxNextDesc;             /* Tx Descriptor address corresponding to the index TxNext */
    DmaDesc *RxBusyDesc;             /* Rx Descriptor address corresponding to the index TxBusy */
    DmaDesc *RxNextDesc;             /* Rx Descriptor address corresponding to the index RxNext */
    DmaDesc *TxPrevDesc;             /* Previous Tx Descriptor */
    u32     tx_sec;
    u32     tx_subsec;
    u32     rx_sec;
    u32     rx_subsec;
} GMACdevice;

void plat_delay(uint32_t ticks);

/* GMAC bit operation helpers */
static u32 __inline__ GMAC_READ(u64 u64Reg)
{
    return read32((void *)u64Reg);
}

static void __inline__ GMAC_WRITE(u64 u64Reg, u32 u32Data)
{
    if((!(u32)u64Reg & 0xFFFFul))
        plat_delay(1);
    write32((void *)u64Reg, u32Data);
}

static void __inline__ GMAC_SETBITS(u64 u64Reg, u32 u32Mask)
{
    if((!(u32)u64Reg & 0xFFFFul))
        plat_delay(1);
    write32((void *)u64Reg, read32((void *)u64Reg) | u32Mask);
}

static void __inline__ GMAC_CLEARBITS(u64 u64Reg, u32 u32Mask)
{
    if((!(u32)u64Reg & 0xFFFFul))
        plat_delay(1);
    write32((void *)u64Reg, read32((void *)u64Reg) & ~u32Mask);
}

/* Config */
#define GMAC_TX_CONFIG_ENABLE(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_TC_Msk)
#define GMAC_TX_CONFIG_DISABLE(gmacdev)      GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_TC_Msk)

#define GMAC_WD_ENABLE(gmacdev)              GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_WD_Msk)
#define GMAC_WD_DISABLE(gmacdev)             GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_WD_Msk)

#define GMAC_JAB_ENABLE(gmacdev)             GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_JD_Msk)
#define GMAC_JAB_DISABLE(gmacdev)            GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_JD_Msk)

#define GMAC_FRAME_BURST_ENABLE(gmacdev)     GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_BE_Msk)
#define GMAC_FRAME_BURST_DISABLE(gmacdev)    GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_BE_Msk)

#define GMAC_JUMBO_FRAME_ENABLE(gmacdev)     GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_JE_Msk)
#define GMAC_JUMBO_FRAME_DISABLE(gmacdev)    GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_JE_Msk)

#define GMAC_CRS_ENABLE(gmacdev)             GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DCRS_Msk)
#define GMAC_CRS_DISABLE(gmacdev)            GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DCRS_Msk)

#define GMAC_PORT_SEL_GMII(gmacdev)          GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_PS_Msk)
#define GMAC_PORT_SEL_MII(gmacdev)           GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_PS_Msk)

#define GMAC_SPD_SEL_100M(gmacdev)           GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_FES_Msk)
#define GMAC_SPD_SEL_10M(gmacdev)            GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_FES_Msk)

#define GMAC_RX_OWN_ENABLE(gmacdev)          GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DO_Msk)
#define GMAC_RX_OWN_DISABLE(gmacdev)         GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DO_Msk)

#define GMAC_LOOPBACK_ENABLE(gmacdev)        GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_LM_Msk)
#define GMAC_LOOPBACK_DISABLE(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_LM_Msk)

#define GMAC_FULL_DUPLEX(gmacdev)            GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DM_Msk)
#define GMAC_HALF_DUPLEX(gmacdev)            GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DM_Msk)

#define GMAC_CHKSUM_OFFLOAD_ENABLE(gmacdev)  GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_IPC_Msk)
#define GMAC_CHKSUM_OFFLOAD_DISABLE(gmacdev) GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_IPC_Msk)

#define GMAC_RETRY_ENABLE(gmacdev)           GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DR_Msk)
#define GMAC_RETRY_DISABLE(gmacdev)          GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DR_Msk)

#define GMAC_STRIP_PAD_CRC_ENABLE(gmacdev)   GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_ACS_Msk)
#define GMAC_STRIP_PAD_CRC_DISABLE(gmacdev)  GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_ACS_Msk)

#define GMAC_DEFERRAL_CHK_ENABLE(gmacdev)    GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DC_Msk)
#define GMAC_DEFERRAL_CHK_DISABLE(gmacdev)   GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_DC_Msk)

#define GMAC_TX_ENABLE(gmacdev)              GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_TE_Msk)
#define GMAC_TX_DISABLE(gmacdev)             GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_TE_Msk)

#define GMAC_RX_ENABLE(gmacdev)              GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_RE_Msk)
#define GMAC_RX_DISABLE(gmacdev)             GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->Config, GMAC_Config_RE_Msk)

/* Frame filter */
#define GMAC_FR_FILTER_ENABLE(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_RA_Msk)
#define GMAC_FR_FILTER_DISABLE(gmacdev)      GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_RA_Msk)

#define GMAC_SA_FILTER_ENABLE(gmacdev)       GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_SAF_Msk)
#define GMAC_SA_FILTER_DISABLE(gmacdev)      GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_SAF_Msk)

#define GMAC_BROADCAST_ENABLE(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_DBF_Msk)
#define GMAC_BROADCAST_DISABLE(gmacdev)      GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_DBF_Msk)

#define GMAC_DA_FILTER_INVERSE(gmacdev)      GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_DAIF_Msk)
#define GMAC_DA_FILTER_NORMAL(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_DAIF_Msk)

#define GMAC_MULTICAST_ENABLE(gmacdev)       GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_PM_Msk)
#define GMAC_MULTICAST_DISABLE(gmacdev)      GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_PM_Msk)

#define GMAC_PROMISC_ENABLE(gmacdev)         GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_PR_Msk)
#define GMAC_PROMISC_DISABLE(gmacdev)        GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FrameFilter, GMAC_FrameFilter_PR_Msk)

/* Flow control */
#define GMAC_UPF_DETECT_ENABLE(gmacdev)      GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_UP_Msk)
#define GMAC_UPF_DETECT_DISABLE(gmacdev)     GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_UP_Msk)

#define GMAC_RX_FLOW_CTRL_ENABLE(gmacdev)    GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_RFE_Msk)
#define GMAC_RX_FLOW_CTRL_DISABLE(gmacdev)   GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_RFE_Msk)

#define GMAC_TX_FLOW_CTRL_ENABLE(gmacdev)    GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_TFE_Msk)
#define GMAC_TX_FLOW_CTRL_DISABLE(gmacdev)   GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->FlowControl, GMAC_FlowControl_TFE_Msk)

/* DMA */
#define GMAC_DMA_BUSMODE_INIT(gmacdev, val)  GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->DmaBusMode, val)

#define GMAC_DMA_TX_PD_RESUME(gmacdev)       GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->DmaTxPollDemand, 0UL)

#define GMAC_DMA_RX_PD_RESUME(gmacdev)       GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->DmaRxPollDemand, 0UL)

#define GMAC_DMA_OPMODE_INIT(gmacdev, val)   GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, val)

#define GMAC_DMA_RX_ENABLE(gmacdev)          GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_SR_Msk)
#define GMAC_DMA_RX_DISABLE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_SR_Msk)

#define GMAC_DMA_TX_ENABLE(gmacdev)          GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_ST_Msk)
#define GMAC_DMA_TX_DISABLE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_ST_Msk)

#define GMAC_DMA_US_FRAME_ENABLE(gmacdev)    GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_FUF_Msk)
#define GMAC_DMA_US_FRAME_DISABLE(gmacdev)   GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_FUF_Msk)

#define GMAC_DMA_ERR_FRAME_ENABLE(gmacdev)   GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_FEF_Msk)
#define GMAC_DMA_ERR_FRAME_DISABLE(gmacdev)  GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_FEF_Msk)

#define GMAC_TCPIP_DROP_ERR_ENABLE(gmacdev)  GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_DT_Msk)
#define GMAC_TCPIP_DROP_ERR_DISABLE(gmacdev) GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->DmaOpMode, GMAC_DmaOpMode_DT_Msk)

/* PMT */
#define GMAC_PMT_INT_ENABLE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->IntMask, GMAC_IntMask_PMTIM_Msk)
#define GMAC_PMT_INT_DISABLE(gmacdev)        GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->IntMask, GMAC_IntMask_PMTIM_Msk)

#define GMAC_PMT_PD_ENABLE(gmacdev)          GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->PmtCtrlStatus, GMAC_PmtCtrlStatus_PWRDWN_Msk)
#define GMAC_PMT_PD_DISABLE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->PmtCtrlStatus, GMAC_PmtCtrlStatus_PWRDWN_Msk)

#define GMAC_PMT_MAGIC_PKT_ENABLE(gmacdev)   GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->PmtCtrlStatus, GMAC_PmtCtrlStatus_MGKPKTEN_Msk)
#define GMAC_PMT_MAGIC_PKT_DISABLE(gmacdev)  GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->PmtCtrlStatus, GMAC_PmtCtrlStatus_MGKPKTEN_Msk)

/* Timestamp */
#define GMAC_TS_ENABLE(gmacdev)              GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENA_Msk)
#define GMAC_TS_DISABLE(gmacdev)             GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENA_Msk)

#define GMAC_TS_INT_ENABLE(gmacdev)          GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSTRIG_Msk)
#define GMAC_TS_INT_DISABLE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSTRIG_Msk)

#define GMAC_TS_PTP_FILTER_ENABLE(gmacdev)   GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENMACADDR_Msk)
#define GMAC_TS_PTP_FILTER_DISABLE(gmacdev)  GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENMACADDR_Msk)

#define GMAC_TS_MASTER_ENABLE(gmacdev)       GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_SNAPTYPSEL_Msk)
#define GMAC_TS_MASTER_DISABLE(gmacdev)      GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_SNAPTYPSEL_Msk)

#define GMAC_TS_EVENT_ENABLE(gmacdev)        GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSEVNTENA_Msk)
#define GMAC_TS_EVENT_DISABLE(gmacdev)       GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSEVNTENA_Msk)

#define GMAC_TS_IPV4_ENABLE(gmacdev)         GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPV4ENA_Msk)
#define GMAC_TS_IPV4_DISABLE(gmacdev)        GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPV4ENA_Msk)

#define GMAC_TS_IPV6_ENABLE(gmacdev)         GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPV6ENA_Msk)
#define GMAC_TS_IPV6_DISABLE(gmacdev)        GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPV6ENA_Msk)

#define GMAC_TS_ETHERNET_ENABLE(gmacdev)     GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPENA_Msk)
#define GMAC_TS_ETHERNET_DISABLE(gmacdev)    GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSIPENA_Msk)

#define GMAC_TS_PTPV2(gmacdev)               GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSVER2ENA_Msk)
#define GMAC_TS_PTPV1(gmacdev)               GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSVER2ENA_Msk)

#define GMAC_TS_ROLLOVER_ENABLE(gmacdev)     GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSCTRLSSR_Msk)
#define GMAC_TS_ROLLOVER_DISABLE(gmacdev)    GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSCTRLSSR_Msk)

#define GMAC_TS_ALL_FRAME_ENABLE(gmacdev)    GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENALL_Msk)
#define GMAC_TS_ALL_FRAME_DISABLE(gmacdev)   GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSENALL_Msk)

#define GMAC_TS_COARSE_UPDATE(gmacdev)       GMAC_SETBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSCFUPDT_Msk)
#define GMAC_TS_FINE_UPDATE(gmacdev)         GMAC_CLEARBITS((u64)&((GMACdevice *)gmacdev)->MacBase->TSControl, GMAC_TSControl_TSCFUPDT_Msk)

/* Interrupt status */
#define GMAC_GET_INT_SUMMARY(gmacdev)        GMAC_READ((u64)&((GMACdevice *)gmacdev)->MacBase->IntStatus)
#define GMAC_CLR_INT_SUMMARY(gmacdev, val)   GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->IntStatus, val)

#define GMAC_GET_RGMII_STATUS(gmacdev)       GMAC_READ((u64)&((GMACdevice *)gmacdev)->MacBase->RgmiiCtrlSts)

#define GMAC_GET_TS_STATUS(gmacdev)          GMAC_READ((u64)&((GMACdevice *)gmacdev)->MacBase->TSStatus)

#define GMAC_GET_LPI_STATUS(gmacdev)         GMAC_READ((u64)&((GMACdevice *)gmacdev)->MacBase->LpiCtrlStatus)

#define GMAC_GET_DMA_STATUS(gmacdev)         GMAC_READ((u64)&((GMACdevice *)gmacdev)->MacBase->DmaStatus)
#define GMAC_CLR_DMA_STATUS(gmacdev, val)    GMAC_WRITE((u64)&((GMACdevice *)gmacdev)->MacBase->DmaStatus, val)

/* PHY */
s32 GMAC_set_mdc_clk_div(GMACdevice *gmacdev,u32 clk_div);
u32 GMAC_get_mdc_clk_div(GMACdevice *gmacdev);
s32 GMAC_read_phy_reg(GMACdevice *gmacdev, u32 PhyOffset, u16 *data);
s32 GMAC_write_phy_reg(GMACdevice *gmacdev, u32 PhyOffset, u16 data);
s32 GMAC_phy_loopback(GMACdevice *gmacdev, bool loopback);
s32 GMAC_perform_phy_reset(GMACdevice *gmacdev, int mode);
/* Driver */
void GMAC_read_version(GMACdevice * gmacdev);
s32 GMAC_reset(GMACdevice *gmacdev);
void GMAC_back_off_limit(GMACdevice *gmacdev, u32 val);
void GMAC_set_pass_control(GMACdevice * gmacdev,u32 passcontrol);
void GMAC_tx_activate_flow_control(GMACdevice *gmacdev);
void GMAC_tx_deactivate_flow_control(GMACdevice *gmacdev);
void GMAC_pause_control(GMACdevice *gmacdev);
s32 GMAC_init(GMACdevice *gmacdev);
s32 GMAC_check_phy_init(GMACdevice *gmacdev, int mode);
s32 GMAC_link_monitor(GMACdevice *gmacdev, int mode);
s32 GMAC_set_mac_addr(GMACdevice *gmacdev, u32 AddrID, u8 *MacAddr);
s32 GMAC_attach(GMACdevice *gmacdev, u32 intf, u32 phyBase);
void GMAC_set_mii_speed(GMACdevice *gmacdev);
/*Descriptor*/
void GMAC_rx_desc_init_ring(DmaDesc *desc, bool last_ring_desc);
void GMAC_tx_desc_init_ring(DmaDesc *desc, bool last_ring_desc);
s32 GMAC_init_tx_rx_desc_queue(GMACdevice *gmacdev);
void GMAC_init_rx_desc_base(GMACdevice *gmacdev);
void GMAC_init_tx_desc_base(GMACdevice *gmacdev);
bool GMAC_is_desc_enhanced_mode(GMACdevice *gmacdev);
bool GMAC_is_desc_owned_by_dma(DmaDesc *desc);
u32 GMAC_get_rx_desc_frame_length(u32 status);
bool GMAC_is_desc_valid(u32 status);
bool GMAC_is_desc_empty(GMACdevice *gmacdev, DmaDesc *desc);
bool GMAC_is_rx_desc_valid(u32 status);
bool GMAC_is_tx_aborted(u32 status);
bool GMAC_is_tx_carrier_error(u32 status);
u32 GMAC_get_tx_collision_count(u32 status);
u32 GMAC_is_exc_tx_collisions(u32 status);
bool GMAC_is_rx_frame_damaged(u32 status);
bool GMAC_is_timestamp_available(u32 status);
bool GMAC_is_rx_frame_collision(u32 status);
bool GMAC_is_rx_crc(u32 status);
bool GMAC_is_frame_dribbling_errors(u32 status);
bool GMAC_is_rx_frame_length_errors(u32 status);
bool GMAC_is_last_rx_desc(GMACdevice *gmacdev, DmaDesc *desc);
bool GMAC_is_last_tx_desc(GMACdevice *gmacdev, DmaDesc *desc);
u32 GMAC_is_rx_checksum_error(u32 status);
bool GMAC_is_tx_ipv4header_checksum_error(u32 status);
bool GMAC_is_tx_payload_checksum_error(u32 status);
void GMAC_tx_checksum_offload_bypass(DmaDesc *desc);
void GMAC_tx_checksum_offload_ipv4hdr(DmaDesc *desc);
void GMAC_tx_checksum_offload_tcponly(DmaDesc *desc);
void GMAC_tx_checksum_offload_tcp_pseudo(DmaDesc *desc);
bool GMAC_is_ext_status(GMACdevice *gmacdev, u32 status);
bool GMAC_ES_is_IP_header_error(u32 ext_status);
bool GMAC_ES_is_rx_checksum_bypassed(u32 ext_status);
bool GMAC_ES_is_IP_payload_error(u32 ext_status);
s32 GMAC_get_tx_qptr(GMACdevice *gmacdev, u32 *Status, u32 *Length, u32 *Buffer1, u32 *Buffer2, u32 *ExtStatus, u32 *TSLow, u32 *TSHigh);
s32 GMAC_set_tx_qptr(GMACdevice *gmacdev, u32 Length, u32 Buffer1, u32 offload_needed, u32 ts);
s32 GMAC_set_rx_qptr(GMACdevice *gmacdev, u32 Buffer1, u32 Length1);
s32 GMAC_get_rx_qptr(GMACdevice *gmacdev, u32 *Status, u32 *Length, u32 *Buffer1, u32 *Buffer2, u32 *ExtStatus, u32 *TSHigh, u32 *TSLow);
void GMAC_take_desc_ownership(DmaDesc *desc);
void GMAC_take_desc_ownership_rx(GMACdevice *gmacdev);
void GMAC_take_desc_ownership_tx(GMACdevice *gmacdev);
/* Timestamp */
void GMAC_TS_set_clk_type(GMACdevice *gmacdev, u32 clk_type);
s32 GMAC_TS_addend_update(GMACdevice *gmacdev, u32 addend_value);
s32 GMAC_TS_timestamp_update(GMACdevice *gmacdev, u32 sec, u32 nanosec);
s32 GMAC_TS_timestamp_init(GMACdevice *gmacdev, u32 sec, u32 nanosec);
void GMAC_TS_subsecond_incr_init(GMACdevice *gmacdev, u32 sub_sec_inc_value);
void GMAC_TS_read_timestamp(GMACdevice *gmacdev, u16 *higher_sec_val, u32 *sec_val, u32 *sub_sec_val);
void GMAC_TS_load_timestamp_higher_val(GMACdevice *gmacdev, u32 higher_sec_val);
void GMAC_TS_read_timestamp_higher_val(GMACdevice *gmacdev, u16 *higher_sec_val);
void GMAC_TS_load_target_timestamp(GMACdevice *gmacdev, u32 sec_val, u32 sub_sec_val);
void GMAC_TS_read_target_timestamp(GMACdevice *gmacdev, u32 * sec_val, u32 * sub_sec_val);
/* VLAN */
void GMAC_svlan_insertion_enable(GMACdevice *gmacdev, u16 vlantag);
void GMAC_cvlan_insertion_enable(GMACdevice *gmacdev, u16 vlantag);
void GMAC_svlan_replace_enable(GMACdevice *gmacdev, u16 vlantag);
void GMAC_cvlan_replace_enable(GMACdevice *gmacdev, u16 vlantag);
void GMAC_vlan_deletion_enable(GMACdevice *gmacdev);
void GMAC_vlan_no_act_enable(GMACdevice *gmacdev);
/* Interrupt */
void GMAC_clear_interrupt(GMACdevice *gmacdev);
u32 GMAC_get_interrupt_type(GMACdevice *gmacdev);
void GMAC_enable_interrupt(GMACdevice *gmacdev, u32 interrupts);
void GMAC_disable_interrupt_all(GMACdevice *gmacdev);
void GMAC_disable_interrupt(GMACdevice *gmacdev, u32 interrupts);
/* Packet */
void GMAC_src_addr_insert_enable(GMACdevice *gmacdev);
void GMAC_src_addr_insert_disable(GMACdevice *gmacdev);
void GMAC_src_addr_replace_enable(GMACdevice *gmacdev);
void GMAC_src_addr_replace_disable(GMACdevice *gmacdev);
bool GMAC_is_magic_packet_received(GMACdevice *gmacdev);

/*! @}*/ /* GMAC of group GMAC_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group GMAC_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __GMAC_H__ */
