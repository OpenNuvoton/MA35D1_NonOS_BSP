/*************************************************************************//**
 * @file     ddr_def.h
 * @version  V1.00
 * @brief    baremetal DDR relative header for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef _MA35D1_DDR_DEF_H
#define _MA35D1_DDR_DEF_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if 1
#define   __I     volatile const       /*!< Defines 'read only' permissions */
#define   __O     volatile             /*!< Defines 'write only' permissions */
#define   __IO    volatile             /*!< Defines 'read / write' permissions */

#define UMCTL2_BASE                      (0x404D0000UL)
#define DDRPHY_BASE                      (0x404C0000UL)

struct UMCTL2 {
    uint32_t MSTR;                  /*!< [0x0000] Master Register0                                                 */
    uint32_t STAT;                  /*!< [0x0004] Operating Mode Status Register                                   */
    uint32_t RESERVE0[2];
    uint32_t MRCTRL0;               /*!< [0x0010] Mode Register Read/Write Control Register 0                      */
    uint32_t MRCTRL1;               /*!< [0x0014] Mode Register Read/Write Control Register 1                      */
    uint32_t MRSTAT;                /*!< [0x0018] Mode Register Read/Write Status Register                         */
    uint32_t RESERVE1[5];
    union
	{
    uint32_t PWRCTL_1;                /*!< [0x0030] Low Power Control Register                                       */
    uint32_t PWRCTL_2;
    uint32_t PWRCTL_3;
	};
    uint32_t PWRTMG;                /*!< [0x0034] Low Power Timing Register                                        */
    uint32_t HWLPCTL;               /*!< [0x0038] Hardware Low Power Control Register                              */
    uint32_t RESERVE2[5];
    uint32_t RFSHCTL0;              /*!< [0x0050] Refresh Control Register 0                                       */
    uint32_t RFSHCTL1;              /*!< [0x0054] Refresh Control Register 1                                       */
    uint32_t RESERVE3[2];
    uint32_t RFSHCTL3;              /*!< [0x0060] Refresh Control Register 3                                       */
    uint32_t RFSHTMG;               /*!< [0x0064] Refresh Timing Register                                          */
    uint32_t RESERVE4[22];
    uint32_t CRCPARCTL0;            /*!< [0x00c0] CRC Parity Control Register0                                     */
    uint32_t RESERVE5[2];
    uint32_t CRCPARSTAT;            /*!< [0x00cc] CRC Parity Status Register                                       */
    uint32_t INIT0;                 /*!< [0x00d0] SDRAM Initialization Register 0                                  */
    uint32_t INIT1;                 /*!< [0x00d4] SDRAM Initialization Register 1                                  */
    uint32_t RESERVE6[1];
    uint32_t INIT3;                 /*!< [0x00dc] SDRAM Initialization Register 3                                  */
    uint32_t INIT4;                 /*!< [0x00e0] SDRAM Initialization Register 4                                  */
    uint32_t INIT5;                 /*!< [0x00e4] SDRAM Initialization Register 5                                  */
    uint32_t RESERVE7[2];
    uint32_t DIMMCTL;               /*!< [0x00f0] DIMM Control Register                                            */
    uint32_t RANKCTL;               /*!< [0x00f4] Rank Control Register                                            */
    uint32_t RESERVE8[2];
    uint32_t DRAMTMG0;              /*!< [0x0100] SDRAM Timing Register 0                                          */
    uint32_t DRAMTMG1;              /*!< [0x0104] SDRAM Timing Register 1                                          */
    uint32_t DRAMTMG2;              /*!< [0x0108] SDRAM Timing Register 2                                          */
    uint32_t DRAMTMG3;              /*!< [0x010c] SDRAM Timing Register 3                                          */
    uint32_t DRAMTMG4;              /*!< [0x0110] SDRAM Timing Register 4                                          */
    uint32_t DRAMTMG5;              /*!< [0x0114] SDRAM Timing Register 5                                          */
    uint32_t RESERVE9[2];
    uint32_t DRAMTMG8;              /*!< [0x0120] SDRAM Timing Register 8                                          */
    uint32_t RESERVE10[6];
    uint32_t DRAMTMG15;             /*!< [0x013c] SDRAM Timing Register 15                                         */
    uint32_t RESERVE11[16];
    uint32_t ZQCTL0;                /*!< [0x0180] ZQ Control Register 0                                            */
    uint32_t ZQCTL1;                /*!< [0x0184] ZQ Control Register 1                                            */
    uint32_t RESERVE12[2];
    uint32_t DFITMG0;               /*!< [0x0190] DFI Timing Register 0                                            */
    uint32_t DFITMG1;               /*!< [0x0194] DFI Timing Register 1                                            */
    uint32_t DFILPCFG0;             /*!< [0x0198] DFI Low Power Configuration Register 0                           */
    uint32_t RESERVE13[1];
    uint32_t DFIUPD0;               /*!< [0x01a0] DFI Update Register 0                                            */
    uint32_t DFIUPD1;               /*!< [0x01a4] DFI Update Register 1                                            */
    uint32_t DFIUPD2;               /*!< [0x01a8] DFI Update Register 2                                            */
    uint32_t RESERVE14[1];
    uint32_t DFIMISC;               /*!< [0x01b0] DFI Miscellaneous Control Register                               */
    uint32_t RESERVE15[2];
    uint32_t DFISTAT;               /*!< [0x01bc] DFI Status Register                                              */
    uint32_t RESERVE16[1];
    uint32_t DFIPHYMSTR;            /*!< [0x01c4] DFI PHY Master                                                   */
    uint32_t RESERVE17[14];
    uint32_t ADDRMAP0;              /*!< [0x0200] Address Map Register 0                                           */
    uint32_t ADDRMAP1;              /*!< [0x0204] Address Map Register 1                                           */
    uint32_t ADDRMAP2;              /*!< [0x0208] Address Map Register 2                                           */
    uint32_t ADDRMAP3;              /*!< [0x020c] Address Map Register 3                                           */
    uint32_t ADDRMAP4;              /*!< [0x0210] Address Map Register 4                                           */
    uint32_t ADDRMAP5;              /*!< [0x0214] Address Map Register 5                                           */
    uint32_t ADDRMAP6;              /*!< [0x0218] Address Map Register 6                                           */
    uint32_t RESERVE18[2];
    uint32_t ADDRMAP9;              /*!< [0x0224] Address Map Register 9                                           */
    uint32_t ADDRMAP10;             /*!< [0x0228] Address Map Register 10                                          */
    uint32_t ADDRMAP11;             /*!< [0x022c] Address Map Register 11                                          */
    uint32_t RESERVE19[4];
    uint32_t ODTCFG;                /*!< [0x0240] ODT Configuration Register                                       */
    uint32_t ODTMAP;                /*!< [0x0244] ODT/Rank Map Register                                            */
    uint32_t RESERVE20[2];
    uint32_t SCHED;                 /*!< [0x0250] Scheduler Control Register                                       */
    uint32_t SCHED1;                /*!< [0x0254] Scheduler Control Register 1                                     */
    uint32_t RESERVE21[1];
    uint32_t PERFHPR1;              /*!< [0x025c] High Priority Read CAM Register 1                                */
    uint32_t RESERVE22[1];
    uint32_t PERFLPR1;              /*!< [0x0264] Low Priority Read CAM Register 1                                 */
    uint32_t RESERVE23[1];
    uint32_t PERFWR1;               /*!< [0x026c] Write CAM Register 1                                             */
    uint32_t RESERVE24[36];
    uint32_t DBG0;                  /*!< [0x0300] Debug Register 0                                                 */
    union
	{
    uint32_t DBG1_1;                /*!< [0x0304] Debug Register 1                                                 */
    uint32_t DBG1_2;
	};
    uint32_t DBGCAM;                /*!< [0x0308] CAM Debug Register                                               */
    uint32_t DBGCMD;                /*!< [0x030c] Command Debug Register                                           */
    uint32_t DBGSTAT;               /*!< [0x0310] Status Debug Register                                            */
    uint32_t RESERVE25[3];
    union
	{
    uint32_t SWCTL_1;               /*!< [0x0320] Software Register Programming Control Enable                     */
    uint32_t SWCTL_2;
    uint32_t SWCTL_3;
	};
    uint32_t SWSTAT;                /*!< [0x0324] Software Register Programming Control Status                     */
    uint32_t SWCTLSTATIC;           /*!< [0x0328] Static Registers Write Enable                                    */
    uint32_t RESERVE26[16];
    uint32_t POISONCFG;             /*!< [0x036c] AXI Poison Configuration Register. Common for all AXI ports.     */
    uint32_t POISONSTAT;            /*!< [0x0370] AXI Poison Status Register                                       */
    uint32_t RESERVE27[34];
    uint32_t PSTAT;                 /*!< [0x03fc] Port Status Register                                             */
    uint32_t PCCFG;                 /*!< [0x0400] Port Common Configuration Register                               */
    uint32_t PCFGR_0;               /*!< [0x0404] Port 0 Configuration Read Register                               */
    uint32_t PCFGW_0;               /*!< [0x0408] Port 0 Configuration Write Register                              */
    uint32_t RESERVE28[33];
    uint32_t PCTRL_0;               /*!< [0x0490] Port 0 Control Register                                          */
    uint32_t PCFGQOS0_0;            /*!< [0x0494] Port 0 Read QoS Configuration Register 0                         */
    uint32_t RESERVE29[7];
    uint32_t PCFGR_1;               /*!< [0x04b4] Port 1 Configuration Read Register                               */
    uint32_t PCFGW_1;               /*!< [0x04b8] Port 1 Configuration Write Register                              */
    uint32_t RESERVE30[33];
    uint32_t PCTRL_1;               /*!< [0x0540] Port 1 Control Register                                          */
    uint32_t PCFGQOS0_1;            /*!< [0x0544] Port 1 Read QoS Configuration Register 0                         */
    uint32_t RESERVE31[7];
    uint32_t PCFGR_2;               /*!< [0x0564] Port 2 Configuration Read Register                               */
    uint32_t PCFGW_2;               /*!< [0x0568] Port 2 Configuration Write Register                              */
    uint32_t RESERVE32[33];
    uint32_t PCTRL_2;               /*!< [0x05f0] Port 2 Control Register                                          */
    uint32_t PCFGQOS0_2;            /*!< [0x05f4] Port 2 Read QoS Configuration Register 0                         */
    uint32_t RESERVE33[7];
    uint32_t PCFGR_3;               /*!< [0x0614] Port 3 Configuration Read Register                               */
    uint32_t PCFGW_3;               /*!< [0x0618] Port 3 Configuration Write Register                              */
    uint32_t RESERVE34[33];
    uint32_t PCTRL_3;               /*!< [0x06a0] Port 3 Control Register                                          */
    uint32_t PCFGQOS0_3;            /*!< [0x06a4] Port 3 Read QoS Configuration Register 0                         */
    uint32_t RESERVE35[7];
    uint32_t PCFGR_4;               /*!< [0x06c4] Port 4 Configuration Read Register                               */
    uint32_t PCFGW_4;               /*!< [0x06c8] Port 4 Configuration Write Register                              */
    uint32_t RESERVE36[33];
    uint32_t PCTRL_4;               /*!< [0x0750] Port 4 Control Register                                          */
    uint32_t PCFGQOS0_4;            /*!< [0x0754] Port 4 Read QoS Configuration Register 0                         */
    uint32_t RESERVE37[7];
    uint32_t PCFGR_5;               /*!< [0x0774] Port 5 Configuration Read Register                               */
    uint32_t PCFGW_5;               /*!< [0x0778] Port 5 Configuration Write Register                              */
    uint32_t RESERVE38[33];
    uint32_t PCTRL_5;               /*!< [0x0800] Port 5 Control Register                                          */
    uint32_t PCFGQOS0_5;            /*!< [0x0804] Port 5 Read QoS Configuration Register 0                         */
    uint32_t RESERVE39[7];
    uint32_t PCFGR_6;               /*!< [0x0824] Port 6 Configuration Read Register                               */
    uint32_t PCFGW_6;               /*!< [0x0828] Port 6 Configuration Write Register                              */
    uint32_t RESERVE40[33];
    uint32_t PCTRL_6;               /*!< [0x08b0] Port 6 Control Register                                          */
    uint32_t PCFGQOS0_6;            /*!< [0x08b4] Port 6 Read QoS Configuration Register 0                         */
    uint32_t RESERVE41[403];
    uint32_t SARBASE0;              /*!< [0x0f04] SAR Base Address Register n                                      */
    uint32_t SARSIZE0;              /*!< [0x0f08] SAR Size Register n                                              */
    uint32_t RESERVE42[57];
    uint32_t VER_NUMBER;            /*!< [0x0ff0] UMCTL2 Version Number Register                                   */
    uint32_t VER_TYPE;              /*!< [0x0ff4] UMCTL2 Version Type Register */
};
#endif

struct DDRPHY
{
__I  uint32_t RIDR;                  /*!< [0x0000] Revision Identification Register                                 */
__IO uint32_t PIR;                   /*!< [0x0004] PHY Initialization Register                                      */
__IO uint32_t PGCR0;                 /*!< [0x0008] PHY General Configuration Registers 0                            */
__IO uint32_t PGCR1;                 /*!< [0x000c] PHY General Configuration Registers 1                            */
__I  uint32_t PGSR0;                 /*!< [0x0010] PHY General Status Registers 0                                   */
__I  uint32_t PGSR1;                 /*!< [0x0014] PHY General Status Registers 1                                   */
__IO uint32_t PLLCR;                 /*!< [0x0018] PLL Control Register                                             */
__IO uint32_t PTR0;                  /*!< [0x001c] PHY Timing Registers 0                                           */
__IO uint32_t PTR1;                  /*!< [0x0020] PHY Timing Registers 1                                           */
__IO uint32_t PTR2;                  /*!< [0x0024] PHY Timing Registers 2                                           */
__IO uint32_t PTR3;                  /*!< [0x0028] PHY Timing Registers 3                                           */
__IO uint32_t PTR4;                  /*!< [0x002c] PHY Timing Registers 4                                           */
__IO uint32_t ACMDLR;                /*!< [0x0030] AC Master Delay Line Register                                    */
__IO uint32_t ACBDLR;                /*!< [0x0034] AC Bit Delay Line Register                                       */
__IO uint32_t ACIOCR;                /*!< [0x0038] AC I/O Configuration Register                                    */
__IO uint32_t DXCCR;                 /*!< [0x003c] DATX8 Common Configuration Register                              */
__IO uint32_t DSGCR;                 /*!< [0x0040] DDR System General Configuration Register                        */
__IO uint32_t DCR;                   /*!< [0x0044] DRAM Configuration Register                                      */
__IO uint32_t DTPR0;                 /*!< [0x0048] DRAM Timing Parameters Register 0                                */
__IO uint32_t DTPR1;                 /*!< [0x004c] DRAM Timing Parameters Register 1                                */
__IO uint32_t DTPR2;                 /*!< [0x0050] DRAM Timing Parameters Register 2                                */
__IO uint32_t MR0;                   /*!< [0x0054] Mode Register 0                                                  */
__IO uint32_t MR1;                   /*!< [0x0058] Mode Register 1                                                  */
__IO uint32_t MR2;                   /*!< [0x005c] Mode Register 2/Extended Mode Register 2                         */
__IO uint32_t MR3;                   /*!< [0x0060] Mode Register 3                                                  */
__IO uint32_t ODTCR;                 /*!< [0x0064] ODT Configuration Register                                       */
__IO uint32_t DTCR;                  /*!< [0x0068] Data Training Configuration Register                             */
__IO uint32_t DTAR0;                 /*!< [0x006c] Data Training Address Register 0                                 */
__IO uint32_t DTAR1;                 /*!< [0x0070] Data Training Address Register 1                                 */
__IO uint32_t DTAR2;                 /*!< [0x0074] Data Training Address Register 2                                 */
__IO uint32_t DTAR3;                 /*!< [0x0078] Data Training Address Register 3                                 */
__IO uint32_t DTDR0;                 /*!< [0x007c] Data Training Data Register 0                                    */
__IO uint32_t DTDR1;                 /*!< [0x0080] Data Training Data Register 1                                    */
__I  uint32_t DTEDR0;                /*!< [0x0084] Data Training Eye Data Register 0                                */
__I  uint32_t DTEDR1;                /*!< [0x0088] Data Training Eye Data Register 1                                */
__IO uint32_t PGCR2;                 /*!< [0x008c] PHY General Configuration Register 2                             */
__I  uint32_t RESERVE0[8];
__I  uint32_t RDIMMGCR0;             /*!< [0x00b0] RDIMM General Configuration Register 0                           */
__I  uint32_t RDIMMGCR1;             /*!< [0x00b4] RDIMM General Configuration Register 1                           */
__I  uint32_t RDIMMCR0;              /*!< [0x00b8] RDIMM Control Register 0                                         */
__I  uint32_t RDIMMCR1;              /*!< [0x00bc] RDIMM Control Register 1                                         */
__IO uint32_t DCUAR;                 /*!< [0x00c0] DCU Address Register                                             */
__IO uint32_t DCUDR;                 /*!< [0x00c4] DCU Data Register                                                */
__IO uint32_t DCURR;                 /*!< [0x00c8] DCU Run Register                                                 */
__IO uint32_t DCULR;                 /*!< [0x00cc] DCU Loop Register                                                */
__IO uint32_t DCUGCR;                /*!< [0x00d0] DCU General Configuration Register                               */
__IO uint32_t DCUTPR;                /*!< [0x00d4] DCU Timing Parameter Register                                    */
__I  uint32_t DCUSR0;                /*!< [0x00d8] DCU Status Register 0                                            */
__I  uint32_t DCUSR1;                /*!< [0x00dc] DCU Status Register 1                                            */
__I  uint32_t RESERVE1[8];
__IO uint32_t BISTRR;                /*!< [0x0100] BIST Run Register                                                */
__IO uint32_t BISTWCR;               /*!< [0x0104] BIST Word Count Register                                         */
__IO uint32_t BISTMSKR0;             /*!< [0x0108] BIST Mask Register 0                                             */
__IO uint32_t BISTMSKR1;             /*!< [0x010c] BIST Mask Register 1                                             */
__IO uint32_t BISTMSKR2;             /*!< [0x0110] BIST Mask Register 2                                             */
__IO uint32_t BISTLSR;               /*!< [0x0114] BIST LFSR Seed Register                                          */
__IO uint32_t BISTAR0;               /*!< [0x0118] BIST Address Register 0                                          */
__IO uint32_t BISTAR1;               /*!< [0x011c] BIST Address Register 1                                          */
__IO uint32_t BISTAR2;               /*!< [0x0120] BIST Address Register 2                                          */
__IO uint32_t BISTUDPR;              /*!< [0x0124] BIST User Data Pattern Register                                  */
__I  uint32_t BISTGSR;               /*!< [0x0128] BIST General Status Register                                     */
__I  uint32_t BISTWER;               /*!< [0x012c] BIST Word Error Register                                         */
__I  uint32_t BISTBER0;              /*!< [0x0130] BIST Bit Error Register 0                                        */
__I  uint32_t BISTBER1;              /*!< [0x0134] BIST Bit Error Register 1                                        */
__I  uint32_t BISTBER2;              /*!< [0x0138] BIST Bit Error Register 2                                        */
__I  uint32_t BISTBER3;              /*!< [0x013c] BIST Bit Error Register 3                                        */
__I  uint32_t BISTWCSR;              /*!< [0x0140] BIST Word Count Status Register                                  */
__I  uint32_t BISTFWR0;              /*!< [0x0144] BIST Fail Word Register 0                                        */
__I  uint32_t BISTFWR1;              /*!< [0x0148] BIST Fail Word Register 1                                        */
__I  uint32_t BISTFWR2;              /*!< [0x014c] BIST Fail Word Register 2                                        */
__I  uint32_t RESERVE2[9];
__IO uint32_t AACR;                  /*!< [0x0174] Anti-Aging Control Register                                      */
__IO uint32_t GPR0;                  /*!< [0x0178] General Purpose Register 0                                       */
__IO uint32_t GPR1;                  /*!< [0x017c] General Purpose Register 1                                       */
__IO uint32_t ZQ0CR0;                /*!< [0x0180] Impedance Control Register 0 for ZQ0                             */
__IO uint32_t ZQ0CR1;                /*!< [0x0184] Impedance Control Register 1 for ZQ0                             */
__I  uint32_t ZQ0SR0;                /*!< [0x0188] Impedance Status Register 0 for ZQ0                              */
__I  uint32_t ZQ0SR1;                /*!< [0x018c] Impedance Status Register 1 for ZQ0                              */
__IO uint32_t ZQ1CR0;                /*!< [0x0190] Impedance Control Register 0 for ZQ1                             */
__IO uint32_t ZQ1CR1;                /*!< [0x0194] Impedance Control Register 1 for ZQ1                             */
__I  uint32_t ZQ1SR0;                /*!< [0x0198] Impedance Status Register 0 for ZQ1                              */
__I  uint32_t ZQ1SR1;                /*!< [0x019c] Impedance Status Register 1 for ZQ1                              */
__IO uint32_t ZQ2CR0;                /*!< [0x01a0] Impedance Control Register 0 for ZQ2                             */
__IO uint32_t ZQ2CR1;                /*!< [0x01a4] Impedance Control Register 1 for ZQ2                             */
__I  uint32_t ZQ2SR0;                /*!< [0x01a8] Impedance Status Register 0 for ZQ2                              */
__I  uint32_t ZQ2SR1;                /*!< [0x01ac] Impedance Status Register 1 for ZQ2                              */
__IO uint32_t ZQ3CR0;                /*!< [0x01b0] Impedance Control Register 0 for ZQ3                             */
__IO uint32_t ZQ3CR1;                /*!< [0x01b4] Impedance Control Register 1 for ZQ3                             */
__I  uint32_t ZQ3SR0;                /*!< [0x01b8] Impedance Status Register 0 for ZQ3                              */
__I  uint32_t ZQ3SR1;                /*!< [0x01bc] Impedance Status Register 1 for ZQ3                              */
__IO uint32_t DX0GCR;                /*!< [0x01c0] DATX8 General Configuration Register for Byte Lane0              */
__I  uint32_t DX0GSR0;               /*!< [0x01c4] DATX8 General Status Registers 0 for Byte Lane0                  */
__I  uint32_t DX0GSR1;               /*!< [0x01c8] DATX8 General Status Registers 1 for Byte Lane0                  */
__IO uint32_t DX0BDLR0;              /*!< [0x01cc] DATX8 Bit Delay Line Register 0 for Byte Lane0                   */
__IO uint32_t DX0BDLR1;              /*!< [0x01d0] DATX8 Bit Delay Line Register 1 for Byte Lane0                   */
__IO uint32_t DX0BDLR2;              /*!< [0x01d4] DATX8 Bit Delay Line Register 2 for Byte Lane0                   */
__IO uint32_t DX0BDLR3;              /*!< [0x01d8] DATX8 Bit Delay Line Register 3 for Byte Lane0                   */
__IO uint32_t DX0BDLR4;              /*!< [0x01dc] DATX8 Bit Delay Line Register 4 for Byte Lane0                   */
__IO uint32_t DX0LCDLR0;             /*!< [0x01e0] DATX8 Bit Delay Line Register 0 for Byte Lane0                   */
__IO uint32_t DX0LCDLR1;             /*!< [0x01e4] DATX8 Bit Delay Line Register 1 for Byte Lane0                   */
__IO uint32_t DX0LCDLR2;             /*!< [0x01e8] DATX8 Bit Delay Line Register 2 for Byte Lane0                   */
__IO uint32_t DX0MDLR;               /*!< [0x01ec] DATX8 Master Delay Line Register for Byte Lane0                  */
__IO uint32_t DX0GTR;                /*!< [0x01f0] DATX8 General Timing Register for Byte Lane0                     */
__IO uint32_t DX0GSR2;               /*!< [0x01f4] DATX8 General Status Register 2 for Byte Lane0                   */
__I  uint32_t RESERVE3[2];
__IO uint32_t DX1GCR;                /*!< [0x0200] DATX8 General Configuration Register for Byte Lane1              */
__I  uint32_t DX1GSR0;               /*!< [0x0204] DATX8 General Status Registers 0 for Byte Lane1                  */
__I  uint32_t DX1GSR1;               /*!< [0x0208] DATX8 General Status Registers 1 for Byte Lane1                  */
__IO uint32_t DX1BDLR0;              /*!< [0x020c] DATX8 Bit Delay Line Register 0 for Byte Lane1                   */
__IO uint32_t DX1BDLR1;              /*!< [0x0210] DATX8 Bit Delay Line Register 1 for Byte Lane1                   */
__IO uint32_t DX1BDLR2;              /*!< [0x0214] DATX8 Bit Delay Line Register 2 for Byte Lane1                   */
__IO uint32_t DX1BDLR3;              /*!< [0x0218] DATX8 Bit Delay Line Register 3 for Byte Lane1                   */
__IO uint32_t DX1BDLR4;              /*!< [0x021c] DATX8 Bit Delay Line Register 4 for Byte Lane1                   */
__IO uint32_t DX1LCDLR0;             /*!< [0x0220] DATX8 Bit Delay Line Register 0 for Byte Lane1                   */
__IO uint32_t DX1LCDLR1;             /*!< [0x0224] DATX8 Bit Delay Line Register 1 for Byte Lane1                   */
__IO uint32_t DX1LCDLR2;             /*!< [0x0228] DATX8 Bit Delay Line Register 2 for Byte Lane1                   */
__IO uint32_t DX1MDLR;               /*!< [0x022c] DATX8 Master Delay Line Register for Byte Lane1                  */
__IO uint32_t DX1GTR;                /*!< [0x0230] DATX8 General Timing Register for Byte Lane1                     */
__IO uint32_t DX1GSR2;               /*!< [0x0234] DATX8 General Status Register 2 for Byte Lane1                   */
__I  uint32_t RESERVE4[2];
__IO uint32_t DX2GCR;                /*!< [0x0240] DATX8 General Configuration Register for Byte Lane2              */
__I  uint32_t DX2GSR0;               /*!< [0x0244] DATX8 General Status Registers 0 for Byte Lane2                  */
__I  uint32_t DX2GSR1;               /*!< [0x0248] DATX8 General Status Registers 1 for Byte Lane2                  */
__IO uint32_t DX2BDLR0;              /*!< [0x024c] DATX8 Bit Delay Line Register 0 for Byte Lane2                   */
__IO uint32_t DX2BDLR1;              /*!< [0x0250] DATX8 Bit Delay Line Register 1 for Byte Lane2                   */
__IO uint32_t DX2BDLR2;              /*!< [0x0254] DATX8 Bit Delay Line Register 2 for Byte Lane2                   */
__IO uint32_t DX2BDLR3;              /*!< [0x0258] DATX8 Bit Delay Line Register 3 for Byte Lane2                   */
__IO uint32_t DX2BDLR4;              /*!< [0x025c] DATX8 Bit Delay Line Register 4 for Byte Lane2                   */
__IO uint32_t DX2LCDLR0;             /*!< [0x0260] DATX8 Bit Delay Line Register 0 for Byte Lane2                   */
__IO uint32_t DX2LCDLR1;             /*!< [0x0264] DATX8 Bit Delay Line Register 1 for Byte Lane2                   */
__IO uint32_t DX2LCDLR2;             /*!< [0x0268] DATX8 Bit Delay Line Register 2 for Byte Lane2                   */
__IO uint32_t DX2MDLR;               /*!< [0x026c] DATX8 Master Delay Line Register for Byte Lane2                  */
__IO uint32_t DX2GTR;                /*!< [0x0270] DATX8 General Timing Register for Byte Lane2                     */
__IO uint32_t DX2GSR2;               /*!< [0x0274] DATX8 General Status Register 2 for Byte Lane2                   */
__I  uint32_t RESERVE5[2];
__IO uint32_t DX3GCR;                /*!< [0x0280] DATX8 General Configuration Register for Byte Lane3              */
__I  uint32_t DX3GSR0;               /*!< [0x0284] DATX8 General Status Registers 0 for Byte Lane3                  */
__I  uint32_t DX3GSR1;               /*!< [0x0288] DATX8 General Status Registers 1 for Byte Lane3                  */
__IO uint32_t DX3BDLR0;              /*!< [0x028c] DATX8 Bit Delay Line Register 0 for Byte Lane3                   */
__IO uint32_t DX3BDLR1;              /*!< [0x0290] DATX8 Bit Delay Line Register 1 for Byte Lane3                   */
__IO uint32_t DX3BDLR2;              /*!< [0x0294] DATX8 Bit Delay Line Register 2 for Byte Lane3                   */
__IO uint32_t DX3BDLR3;              /*!< [0x0298] DATX8 Bit Delay Line Register 3 for Byte Lane3                   */
__IO uint32_t DX3BDLR4;              /*!< [0x029c] DATX8 Bit Delay Line Register 4 for Byte Lane3                   */
__IO uint32_t DX3LCDLR0;             /*!< [0x02a0] DATX8 Bit Delay Line Register 0 for Byte Lane3                   */
__IO uint32_t DX3LCDLR1;             /*!< [0x02a4] DATX8 Bit Delay Line Register 1 for Byte Lane3                   */
__IO uint32_t DX3LCDLR2;             /*!< [0x02a8] DATX8 Bit Delay Line Register 2 for Byte Lane3                   */
__IO uint32_t DX3MDLR;               /*!< [0x02ac] DATX8 Master Delay Line Register for Byte Lane3                  */
__IO uint32_t DX3GTR;                /*!< [0x02b0] DATX8 General Timing Register for Byte Lane3                     */
__IO uint32_t DX3GSR2;               /*!< [0x02b4] DATX8 General Status Register 2 for Byte Lane3                   */
__I  uint32_t RESERVE6[2];
__IO uint32_t DX4GCR;                /*!< [0x02c0] DATX8 General Configuration Register for Byte Lane4              */
__I  uint32_t DX4GSR0;               /*!< [0x02c4] DATX8 General Status Registers 0 for Byte Lane4                  */
__I  uint32_t DX4GSR1;               /*!< [0x02c8] DATX8 General Status Registers 1 for Byte Lane4                  */
__IO uint32_t DX4BDLR0;              /*!< [0x02cc] DATX8 Bit Delay Line Register 0 for Byte Lane4                   */
__IO uint32_t DX4BDLR1;              /*!< [0x02d0] DATX8 Bit Delay Line Register 1 for Byte Lane4                   */
__IO uint32_t DX4BDLR2;              /*!< [0x02d4] DATX8 Bit Delay Line Register 2 for Byte Lane4                   */
__IO uint32_t DX4BDLR3;              /*!< [0x02d8] DATX8 Bit Delay Line Register 3 for Byte Lane4                   */
__IO uint32_t DX4BDLR4;              /*!< [0x02dc] DATX8 Bit Delay Line Register 4 for Byte Lane4                   */
__IO uint32_t DX4LCDLR0;             /*!< [0x02e0] DATX8 Bit Delay Line Register 0 for Byte Lane4                   */
__IO uint32_t DX4LCDLR1;             /*!< [0x02e4] DATX8 Bit Delay Line Register 1 for Byte Lane4                   */
__IO uint32_t DX4LCDLR2;             /*!< [0x02e8] DATX8 Bit Delay Line Register 2 for Byte Lane4                   */
__IO uint32_t DX4MDLR;               /*!< [0x02ec] DATX8 Master Delay Line Register for Byte Lane4                  */
__IO uint32_t DX4GTR;                /*!< [0x02f0] DATX8 General Timing Register for Byte Lane4                     */
__IO uint32_t DX4GSR2;               /*!< [0x02f4] DATX8 General Status Register 2 for Byte Lane4                   */
__I  uint32_t RESERVE7[2];
__IO uint32_t DX5GCR;                /*!< [0x0300] DATX8 General Configuration Register for Byte Lane5              */
__I  uint32_t DX5GSR0;               /*!< [0x0304] DATX8 General Status Registers 0 for Byte Lane5                  */
__I  uint32_t DX5GSR1;               /*!< [0x0308] DATX8 General Status Registers 1 for Byte Lane5                  */
__IO uint32_t DX5BDLR0;              /*!< [0x030c] DATX8 Bit Delay Line Register 0 for Byte Lane5                   */
__IO uint32_t DX5BDLR1;              /*!< [0x0310] DATX8 Bit Delay Line Register 1 for Byte Lane5                   */
__IO uint32_t DX5BDLR2;              /*!< [0x0314] DATX8 Bit Delay Line Register 2 for Byte Lane5                   */
__IO uint32_t DX5BDLR3;              /*!< [0x0318] DATX8 Bit Delay Line Register 3 for Byte Lane5                   */
__IO uint32_t DX5BDLR4;              /*!< [0x031c] DATX8 Bit Delay Line Register 4 for Byte Lane5                   */
__IO uint32_t DX5LCDLR0;             /*!< [0x0320] DATX8 Bit Delay Line Register 0 for Byte Lane5                   */
__IO uint32_t DX5LCDLR1;             /*!< [0x0324] DATX8 Bit Delay Line Register 1 for Byte Lane5                   */
__IO uint32_t DX5LCDLR2;             /*!< [0x0328] DATX8 Bit Delay Line Register 2 for Byte Lane5                   */
__IO uint32_t DX5MDLR;               /*!< [0x032c] DATX8 Master Delay Line Register for Byte Lane5                  */
__IO uint32_t DX5GTR;                /*!< [0x0330] DATX8 General Timing Register for Byte Lane5                     */
__IO uint32_t DX5GSR2;               /*!< [0x0334] DATX8 General Status Register 2 for Byte Lane5                   */
__I  uint32_t RESERVE8[2];
__IO uint32_t DX6GCR;                /*!< [0x0340] DATX8 General Configuration Register for Byte Lane6              */
__I  uint32_t DX6GSR0;               /*!< [0x0344] DATX8 General Status Registers 0 for Byte Lane6                  */
__I  uint32_t DX6GSR1;               /*!< [0x0348] DATX8 General Status Registers 1 for Byte Lane6                  */
__IO uint32_t DX6BDLR0;              /*!< [0x034c] DATX8 Bit Delay Line Register 0 for Byte Lane6                   */
__IO uint32_t DX6BDLR1;              /*!< [0x0350] DATX8 Bit Delay Line Register 1 for Byte Lane6                   */
__IO uint32_t DX6BDLR2;              /*!< [0x0354] DATX8 Bit Delay Line Register 2 for Byte Lane6                   */
__IO uint32_t DX6BDLR3;              /*!< [0x0358] DATX8 Bit Delay Line Register 3 for Byte Lane6                   */
__IO uint32_t DX6BDLR4;              /*!< [0x035c] DATX8 Bit Delay Line Register 4 for Byte Lane6                   */
__IO uint32_t DX6LCDLR0;             /*!< [0x0360] DATX8 Bit Delay Line Register 0 for Byte Lane6                   */
__IO uint32_t DX6LCDLR1;             /*!< [0x0364] DATX8 Bit Delay Line Register 1 for Byte Lane6                   */
__IO uint32_t DX6LCDLR2;             /*!< [0x0368] DATX8 Bit Delay Line Register 2 for Byte Lane6                   */
__IO uint32_t DX6MDLR;               /*!< [0x036c] DATX8 Master Delay Line Register for Byte Lane6                  */
__IO uint32_t DX6GTR;                /*!< [0x0370] DATX8 General Timing Register for Byte Lane6                     */
__IO uint32_t DX6GSR2;               /*!< [0x0374] DATX8 General Status Register 2 for Byte Lane6                   */
__I  uint32_t RESERVE9[2];
__IO uint32_t DX7GCR;                /*!< [0x0380] DATX8 General Configuration Register for Byte Lane7              */
__I  uint32_t DX7GSR0;               /*!< [0x0384] DATX8 General Status Registers 0 for Byte Lane7                  */
__I  uint32_t DX7GSR1;               /*!< [0x0388] DATX8 General Status Registers 1 for Byte Lane7                  */
__IO uint32_t DX7BDLR0;              /*!< [0x038c] DATX8 Bit Delay Line Register 0 for Byte Lane7                   */
__IO uint32_t DX7BDLR1;              /*!< [0x0390] DATX8 Bit Delay Line Register 1 for Byte Lane7                   */
__IO uint32_t DX7BDLR2;              /*!< [0x0394] DATX8 Bit Delay Line Register 2 for Byte Lane7                   */
__IO uint32_t DX7BDLR3;              /*!< [0x0398] DATX8 Bit Delay Line Register 3 for Byte Lane7                   */
__IO uint32_t DX7BDLR4;              /*!< [0x039c] DATX8 Bit Delay Line Register 4 for Byte Lane7                   */
__IO uint32_t DX7LCDLR0;             /*!< [0x03a0] DATX8 Bit Delay Line Register 0 for Byte Lane7                   */
__IO uint32_t DX7LCDLR1;             /*!< [0x03a4] DATX8 Bit Delay Line Register 1 for Byte Lane7                   */
__IO uint32_t DX7LCDLR2;             /*!< [0x03a8] DATX8 Bit Delay Line Register 2 for Byte Lane7                   */
__IO uint32_t DX7MDLR;               /*!< [0x03ac] DATX8 Master Delay Line Register for Byte Lane7                  */
__IO uint32_t DX7GTR;                /*!< [0x03b0] DATX8 General Timing Register for Byte Lane7                     */
__IO uint32_t DX7GSR2;               /*!< [0x03b4] DATX8 General Status Register 2 for Byte Lane7                   */
__I  uint32_t RESERVE10[2];
__IO uint32_t DX8GCR;                /*!< [0x03c0] DATX8 General Configuration Register for Byte Lane8              */
__I  uint32_t DX8GSR0;               /*!< [0x03c4] DATX8 General Status Registers 0 for Byte Lane8                  */
__I  uint32_t DX8GSR1;               /*!< [0x03c8] DATX8 General Status Registers 1 for Byte Lane8                  */
__IO uint32_t DX8BDLR0;              /*!< [0x03cc] DATX8 Bit Delay Line Register 0 for Byte Lane8                   */
__IO uint32_t DX8BDLR1;              /*!< [0x03d0] DATX8 Bit Delay Line Register 1 for Byte Lane8                   */
__IO uint32_t DX8BDLR2;              /*!< [0x03d4] DATX8 Bit Delay Line Register 2 for Byte Lane8                   */
__IO uint32_t DX8BDLR3;              /*!< [0x03d8] DATX8 Bit Delay Line Register 3 for Byte Lane8                   */
__IO uint32_t DX8BDLR4;              /*!< [0x03dc] DATX8 Bit Delay Line Register 4 for Byte Lane8                   */
__IO uint32_t DX8LCDLR0;             /*!< [0x03e0] DATX8 Bit Delay Line Register 0 for Byte Lane8                   */
__IO uint32_t DX8LCDLR1;             /*!< [0x03e4] DATX8 Bit Delay Line Register 1 for Byte Lane8                   */
__IO uint32_t DX8LCDLR2;             /*!< [0x03e8] DATX8 Bit Delay Line Register 2 for Byte Lane8                   */
__IO uint32_t DX8MDLR;               /*!< [0x03ec] DATX8 Master Delay Line Register for Byte Lane8                  */
__IO uint32_t DX8GTR;                /*!< [0x03f0] DATX8 General Timing Register for Byte Lane8                     */
__IO uint32_t DX8GSR2;               /*!< [0x03f4] DATX8 General Status Register 2 for Byte Lane8                   */

};


#define DDRCTL_DBG1_1_REG       0x00000001  // 0 // DBG1
#define DDRCTL_PWRCTL_1_REG     0x00000001  // 1 // PWRCTL
#define DDRCTL_MSTR_REG         0x01040001  // 2
#define DDRCTL_MRCTRL0_REG      0x0000d010  // 3
#define DDRCTL_MRCTRL1_REG      0x00000000  // 4
#define DDRCTL_PWRCTL_2_REG     0x00000000  // 5 // PWRCTL
#define DDRCTL_PWRTMG_REG       0x00400010  // 6
#define DDRCTL_HWLPCTL_REG      0x000a0003  // 7
#define DDRCTL_RFSHCTL0_REG     0x00210000  // 8
#define DDRCTL_RFSHCTL1_REG     0x003c003c  // 9
#define DDRCTL_RFSHCTL3_REG     0x00000000  // 10
#define DDRCTL_RFSHTMG_REG      0x0010002b  // 11
#define DDRCTL_CRCPARCTL0_REG   0x00000000  // 12
#define DDRCTL_INIT0_REG        0x40020083  // 13
#define DDRCTL_INIT1_REG        0x00350002  // 14
#define DDRCTL_INIT3_REG        0x1b400006  // 15
#define DDRCTL_INIT4_REG        0x00480000  // 16
#define DDRCTL_INIT5_REG        0x00090000  // 17
#define DDRCTL_DIMMCTL_REG      0x00000000  // 18
#define DDRCTL_RANKCTL_REG      0x0000032f  // 19
#define DDRCTL_DRAMTMG0_REG     0x090d040a  // 20
#define DDRCTL_DRAMTMG1_REG     0x0003020e  // 21
#define DDRCTL_DRAMTMG2_REG     0x00000408  // 22
#define DDRCTL_DRAMTMG3_REG     0x00003007  // 23
#define DDRCTL_DRAMTMG4_REG     0x04020205  // 24
#define DDRCTL_DRAMTMG5_REG     0x03030202  // 25
#define DDRCTL_DRAMTMG8_REG     0x00000a02  // 26
#define DDRCTL_DRAMTMG15_REG    0x80000032  // 27
#define DDRCTL_ZQCTL0_REG       0x00800020  // 28
#define DDRCTL_ZQCTL1_REG       0x00000100  // 29
#define DDRCTL_DFITMG0_REG      0x04020101  // 30
#define DDRCTL_DFITMG1_REG      0x00060101  // 31
#define DDRCTL_DFILPCFG0_REG    0x0700b030  // 32
#define DDRCTL_DFIUPD0_REG      0x00400005  // 33
#define DDRCTL_DFIUPD1_REG      0x00170066  // 34
#define DDRCTL_DFIUPD2_REG      0x80000000  // 35
#define DDRCTL_DFIMISC_REG      0x00000011  // 36
#define DDRCTL_DFIPHYMSTR_REG   0x00000000  // 37
#define DDRCTL_ADDRMAP0_REG     0x00000015  // 38
#define DDRCTL_ADDRMAP1_REG     0x00080808  // 39
#define DDRCTL_ADDRMAP2_REG     0x00000000  // 40
#define DDRCTL_ADDRMAP3_REG     0x00000000  // 41
#define DDRCTL_ADDRMAP4_REG     0x00001f1f  // 42
#define DDRCTL_ADDRMAP5_REG     0x070f0707  // 43
#define DDRCTL_ADDRMAP6_REG     0x0f0f0707  // 44
#define DDRCTL_ADDRMAP9_REG     0x07070707  // 45
#define DDRCTL_ADDRMAP10_REG    0x07070707  // 46
#define DDRCTL_ADDRMAP11_REG    0x00000007  // 47
#define DDRCTL_ODTCFG_REG       0x06000608  // 48
#define DDRCTL_ODTMAP_REG       0x00000101  // 49
#define DDRCTL_SCHED_REG        0x00f51f00  // 50
#define DDRCTL_SCHED1_REG       0x00000000  // 51
#define DDRCTL_PERFHPR1_REG     0x0f000001  // 52
#define DDRCTL_PERFLPR1_REG     0x0f00007f  // 53
#define DDRCTL_PERFWR1_REG      0x0f00007f  // 54
#define DDRCTL_DBG0_REG         0x00000000  // 55
#define DDRCTL_DBG1_2_REG       0x00000000  // 56 // DBG1
#define DDRCTL_DBGCMD_REG       0x00000000  // 57
#define DDRCTL_SWCTL_1_REG      0x00000001  // 58
#define DDRCTL_SWCTLSTATIC_REG  0x00000000  // 59
#define DDRCTL_POISONCFG_REG    0x00110011  // 60
#define DDRCTL_PCTRL_0_REG      0x00000001  // 61
#define DDRCTL_PCTRL_1_REG      0x00000000  // 62
#define DDRCTL_PCTRL_2_REG      0x00000000  // 63
#define DDRCTL_PCTRL_3_REG      0x00000000  // 64
#define DDRCTL_PCTRL_4_REG      0x00000000  // 65
#define DDRCTL_PCTRL_5_REG      0x00000000  // 66
#define DDRCTL_PCTRL_6_REG      0x00000001  // 67
#define DDRCTL_PCCFG_REG        0x00000000  // 68
#define DDRCTL_PCFGR_0_REG      0x0001500f  // 69
#define DDRCTL_PCFGR_1_REG      0x0001500f  // 70
#define DDRCTL_PCFGR_2_REG      0x0001500f  // 71
#define DDRCTL_PCFGR_3_REG      0x0001500f  // 72
#define DDRCTL_PCFGR_4_REG      0x0001500f  // 73
#define DDRCTL_PCFGR_5_REG      0x0001500f  // 74
#define DDRCTL_PCFGR_6_REG      0x0001500f  // 75
#define DDRCTL_PCFGW_0_REG      0x0000500f  // 76
#define DDRCTL_PCFGW_1_REG      0x0000500f  // 77
#define DDRCTL_PCFGW_2_REG      0x0000500f  // 78
#define DDRCTL_PCFGW_3_REG      0x0000500f  // 79
#define DDRCTL_PCFGW_4_REG      0x0000500f  // 80
#define DDRCTL_PCFGW_5_REG      0x0000500f  // 81
#define DDRCTL_PCFGW_6_REG      0x0000500f  // 82
#define DDRCTL_SARBASE0_REG     0x00000008  // 83
#define DDRCTL_SARSIZE0_REG     0x00000000  // 84

#define DDRCTL_PWRCTL_3_REG     0x0000000b  // 106
#define DDRCTL_SWCTL_2_REG      0x00000000  // 105
#define DDRCTL_SWCTL_3_REG      0x00000001  // 107


#define DDRPHY_DSGCR_REG        0xf004649f  // 85
#define DDRPHY_PGCR1_REG        0x0300c461  // 86
#define DDRPHY_PGCR2_REG        0x00f0027f  // 87
#define DDRPHY_PTR0_REG         0x0c806403  // 88
#define DDRPHY_PTR1_REG         0x27100385  // 89
#define DDRPHY_PTR2_REG         0x00083def  // 90
#define DDRPHY_PTR3_REG         0x05b4111d  // 91
#define DDRPHY_PTR4_REG         0x0801a072  // 92
#define DDRPHY_MR0_REG          0x00001b40  // 93
#define DDRPHY_MR1_REG          0x00000006  // 94
#define DDRPHY_MR2_REG          0x00000048  // 95
#define DDRPHY_MR3_REG          0x00000000  // 96
#define DDRPHY_DTPR0_REG        0x71568855  // 97
#define DDRPHY_DTPR1_REG        0x2282b32a  // 98
#define DDRPHY_DTPR2_REG        0x30023e00  // 99
#define DDRPHY_ZQ0CR1_REG       0x0000105d  // 100
#define DDRPHY_DCR_REG          0x0000040b  // 101
#define DDRPHY_DTCR_REG         0x91003587  // 102
#define DDRPHY_PLLCR_REG        0x0001c000  // 103
#define DDRPHY_PIR_REG          0x0000ff81  // 104



struct nvt_ddr_init_param {
	uint32_t DBG1_1;     // 0
	uint32_t PWRCTL_1;   // 1
	uint32_t MSTR;       // 2
	uint32_t MRCTRL0;    // 3
	uint32_t MRCTRL1;    // 4
	uint32_t PWRCTL_2;   // 5
	uint32_t PWRTMG;     // 6
	uint32_t HWLPCTL;    // 7
	uint32_t RFSHCTL0;   // 8
	uint32_t RFSHCTL1;   // 9
	uint32_t RFSHCTL3;   // 10
	uint32_t RFSHTMG;    // 11
	uint32_t CRCPARCTL0; // 12
	uint32_t INIT0;      // 13
	uint32_t INIT1;      // 14
	uint32_t INIT3;      // 15
	uint32_t INIT4;      // 16
	uint32_t INIT5;      // 17
	uint32_t DIMMCTL;    // 18
	uint32_t RANKCTL;    // 19
	uint32_t DRAMTMG0;   // 20
	uint32_t DRAMTMG1;   // 21
	uint32_t DRAMTMG2;   // 22
	uint32_t DRAMTMG3;   // 23
	uint32_t DRAMTMG4;   // 24
	uint32_t DRAMTMG5;   // 25
	uint32_t DRAMTMG8;   // 26
	uint32_t DRAMTMG15;  // 27
	uint32_t ZQCTL0;     // 28
	uint32_t ZQCTL1;     // 29
	uint32_t DFITMG0;    // 30
	uint32_t DFITMG1;    // 31
	uint32_t DFILPCFG0;  // 32
	uint32_t DFIUPD0;    // 33
	uint32_t DFIUPD1;    // 34
	uint32_t DFIUPD2;    // 35
	uint32_t DFIMISC;    // 36
	uint32_t DFIPHYMSTR; // 37
	uint32_t ADDRMAP0;   // 38
	uint32_t ADDRMAP1;   // 39
	uint32_t ADDRMAP2;   // 40
	uint32_t ADDRMAP3;   // 41
	uint32_t ADDRMAP4;   // 42
	uint32_t ADDRMAP5;   // 43
	uint32_t ADDRMAP6;   // 44
	uint32_t ADDRMAP9;   // 45
	uint32_t ADDRMAP10;  // 46
	uint32_t ADDRMAP11;  // 47
	uint32_t ODTCFG;     // 48
	uint32_t ODTMAP;     // 49
	uint32_t SCHED;      // 50
	uint32_t SCHED1;     // 51
	uint32_t PERFHPR1;   // 52
	uint32_t PERFLPR1;   // 53
	uint32_t PERFWR1;    // 54
	uint32_t DBG0;       // 55
	uint32_t DBG1_2;     // 56
	uint32_t DBGCMD;     // 57
	uint32_t SWCTL_1;    // 58
	uint32_t SWCTLSTATIC;// 59
	uint32_t POISONCFG;  // 60
	uint32_t PCTRL_0;    // 61
	uint32_t PCTRL_1;    // 62
	uint32_t PCTRL_2;    // 63
	uint32_t PCTRL_3;    // 64
	uint32_t PCTRL_4;    // 65
	uint32_t PCTRL_5;    // 66
	uint32_t PCTRL_6;    // 67
	uint32_t PCCFG;      // 68
	uint32_t PCFGR_0;    // 69
	uint32_t PCFGR_1;    // 70
	uint32_t PCFGR_2;    // 71
	uint32_t PCFGR_3;    // 72
	uint32_t PCFGR_4;    // 73
	uint32_t PCFGR_5;    // 74
	uint32_t PCFGR_6;    // 75
	uint32_t PCFGW_0;    // 76
	uint32_t PCFGW_1;    // 77
	uint32_t PCFGW_2;    // 78
	uint32_t PCFGW_3;    // 79
	uint32_t PCFGW_4;    // 80
	uint32_t PCFGW_5;    // 81
	uint32_t PCFGW_6;    // 82
	uint32_t SARBASE0;   // 83
	uint32_t SARSIZE0;   // 84

	// DDR PHY
	uint32_t DSGCR;      // 85
	uint32_t PGCR1;      // 86
	uint32_t PGCR2;      // 87
	uint32_t PTR0;       // 88
	uint32_t PTR1;       // 89
	uint32_t PTR2;       // 90
	uint32_t PTR3;       // 91
	uint32_t PTR4;       // 92
	uint32_t MR0;        // 93
	uint32_t MR1;        // 94
	uint32_t MR2;        // 95
	uint32_t MR3;        // 96
	uint32_t DTPR0;      // 97
	uint32_t DTPR1;      // 98
	uint32_t DTPR2;      // 99
	uint32_t ZQ0CR1;     // 100
	uint32_t DCR;        // 101
	uint32_t DTCR;       // 102
	uint32_t PLLCR;      // 103
	uint32_t PIR;        // 104

	uint32_t SWCTL_2;    // 105
	uint32_t PWRCTL_3;   // 106
	uint32_t SWCTL_3;    // 107
};

struct nvt_ddr_init_param ddrparam = {
		DDRCTL_DBG1_1_REG,     // 0
		DDRCTL_PWRCTL_1_REG,   // 1
		DDRCTL_MSTR_REG,       // 2
		DDRCTL_MRCTRL0_REG,    // 3
		DDRCTL_MRCTRL1_REG,    // 4
		DDRCTL_PWRCTL_2_REG,   // 5
		DDRCTL_PWRTMG_REG,     // 6
		DDRCTL_HWLPCTL_REG,    // 7
		DDRCTL_RFSHCTL0_REG,   // 8
		DDRCTL_RFSHCTL1_REG,   // 9
		DDRCTL_RFSHCTL3_REG,   // 10
		DDRCTL_RFSHTMG_REG,    // 11
		DDRCTL_CRCPARCTL0_REG, // 12
		DDRCTL_INIT0_REG,      // 13
		DDRCTL_INIT1_REG,      // 14
		DDRCTL_INIT3_REG,      // 15
		DDRCTL_INIT4_REG,      // 16
		DDRCTL_INIT5_REG,      // 17
		DDRCTL_DIMMCTL_REG,    // 18
		DDRCTL_RANKCTL_REG,    // 19
		DDRCTL_DRAMTMG0_REG,   // 20
		DDRCTL_DRAMTMG1_REG,   // 21
		DDRCTL_DRAMTMG2_REG,   // 22
		DDRCTL_DRAMTMG3_REG,   // 23
		DDRCTL_DRAMTMG4_REG,   // 24
		DDRCTL_DRAMTMG5_REG,   // 25
		DDRCTL_DRAMTMG8_REG,   // 26
		DDRCTL_DRAMTMG15_REG,  // 27
		DDRCTL_ZQCTL0_REG,     // 28
		DDRCTL_ZQCTL1_REG,     // 29
		DDRCTL_DFITMG0_REG,    // 30
		DDRCTL_DFITMG1_REG,    // 31
		DDRCTL_DFILPCFG0_REG,  // 32
		DDRCTL_DFIUPD0_REG,    // 33
		DDRCTL_DFIUPD1_REG,    // 34
		DDRCTL_DFIUPD2_REG,    // 35
		DDRCTL_DFIMISC_REG,    // 36
		DDRCTL_DFIPHYMSTR_REG, // 37
		DDRCTL_ADDRMAP0_REG,   // 38
		DDRCTL_ADDRMAP1_REG,   // 39
		DDRCTL_ADDRMAP2_REG,   // 40
		DDRCTL_ADDRMAP3_REG,   // 41
		DDRCTL_ADDRMAP4_REG,   // 42
		DDRCTL_ADDRMAP5_REG,   // 43
		DDRCTL_ADDRMAP6_REG,   // 44
		DDRCTL_ADDRMAP9_REG,   // 45
		DDRCTL_ADDRMAP10_REG,  // 46
		DDRCTL_ADDRMAP11_REG,  // 47
		DDRCTL_ODTCFG_REG,     // 48
		DDRCTL_ODTMAP_REG,     // 49
		DDRCTL_SCHED_REG,      // 50
		DDRCTL_SCHED1_REG,     // 51
		DDRCTL_PERFHPR1_REG,   // 52
		DDRCTL_PERFLPR1_REG,   // 53
		DDRCTL_PERFWR1_REG,    // 54
		DDRCTL_DBG0_REG,       // 55
		DDRCTL_DBG1_2_REG,     // 56
		DDRCTL_DBGCMD_REG,     // 57
		DDRCTL_SWCTL_1_REG,    // 58
		DDRCTL_SWCTLSTATIC_REG,// 59
		DDRCTL_POISONCFG_REG,  // 60
		DDRCTL_PCTRL_0_REG,    // 61
		DDRCTL_PCTRL_1_REG,    // 62
		DDRCTL_PCTRL_2_REG,    // 63
		DDRCTL_PCTRL_3_REG,    // 64
		DDRCTL_PCTRL_4_REG,    // 65
		DDRCTL_PCTRL_5_REG,    // 66
		DDRCTL_PCTRL_6_REG,    // 67
		DDRCTL_PCCFG_REG,      // 68
		DDRCTL_PCFGR_0_REG,    // 69
		DDRCTL_PCFGR_1_REG,    // 70
		DDRCTL_PCFGR_2_REG,    // 71
		DDRCTL_PCFGR_3_REG,    // 72
		DDRCTL_PCFGR_4_REG,    // 73
		DDRCTL_PCFGR_5_REG,    // 74
		DDRCTL_PCFGR_6_REG,    // 75
		DDRCTL_PCFGW_0_REG,    // 76
		DDRCTL_PCFGW_1_REG,    // 77
		DDRCTL_PCFGW_2_REG,    // 78
		DDRCTL_PCFGW_3_REG,    // 79
		DDRCTL_PCFGW_4_REG,    // 80
		DDRCTL_PCFGW_5_REG,    // 81
		DDRCTL_PCFGW_6_REG,    // 82
		DDRCTL_SARBASE0_REG,   // 83
		DDRCTL_SARSIZE0_REG,   // 84

		 // DDR PHY
		DDRPHY_DSGCR_REG,      // 85
		DDRPHY_PGCR1_REG,      // 86
		DDRPHY_PGCR2_REG,      // 87
		DDRPHY_PTR0_REG,       // 88
		DDRPHY_PTR1_REG,       // 89
		DDRPHY_PTR2_REG,       // 90
		DDRPHY_PTR3_REG,       // 91
		DDRPHY_PTR4_REG,       // 92
		DDRPHY_MR0_REG,        // 93
		DDRPHY_MR1_REG,        // 94
		DDRPHY_MR2_REG,        // 95
		DDRPHY_MR3_REG,        // 96
		DDRPHY_DTPR0_REG,      // 97
		DDRPHY_DTPR1_REG,      // 98
		DDRPHY_DTPR2_REG,      // 99
		DDRPHY_ZQ0CR1_REG,     // 100
		DDRPHY_DCR_REG,        // 101
		DDRPHY_DTCR_REG,       // 102
		DDRPHY_PLLCR_REG,      // 103
		DDRPHY_PIR_REG,        // 104

		DDRCTL_SWCTL_2_REG,    // 105
		DDRCTL_PWRCTL_3_REG,   // 106
		DDRCTL_SWCTL_3_REG,    // 107
};

#define DDR_REG(a, b, c)				\
{							            \
	.base = (uintptr_t)a,					        \
	.offset = offsetof(struct b, c),	    \
	.init_flow_offset = offsetof(struct nvt_ddr_init_param, c), \
}


#define DDR_CTL_REG(ddrctl_reg)	 DDR_REG(UMCTL2_BASE, UMCTL2, ddrctl_reg)
#define DDR_PHY_REG(ddrphy_reg)	 DDR_REG(DDRPHY_BASE, DDRPHY, ddrphy_reg)

struct DDR_Setting
{
	volatile uint32_t base;
	volatile uint32_t offset;
	volatile uint32_t init_flow_offset;
};


#define nvt_ddr_init_setting_size sizeof(ddrparam)/sizeof(uint32_t)
struct DDR_Setting nvt_ddr_init_setting[nvt_ddr_init_setting_size] = {
	DDR_CTL_REG(DBG1_1),     // 0
	DDR_CTL_REG(PWRCTL_1),   // 1
	DDR_CTL_REG(MSTR),       // 2
	DDR_CTL_REG(MRCTRL0),    // 3
	DDR_CTL_REG(MRCTRL1),    // 4
	DDR_CTL_REG(PWRCTL_2),   // 5
	DDR_CTL_REG(PWRTMG),     // 6
	DDR_CTL_REG(HWLPCTL),    // 7
	DDR_CTL_REG(RFSHCTL0),   // 8
	DDR_CTL_REG(RFSHCTL1),   // 9
	DDR_CTL_REG(RFSHCTL3),   // 10
	DDR_CTL_REG(RFSHTMG),    // 11
	DDR_CTL_REG(CRCPARCTL0), // 12
	DDR_CTL_REG(INIT0),      // 13
	DDR_CTL_REG(INIT1),      // 14
	DDR_CTL_REG(INIT3),      // 15
	DDR_CTL_REG(INIT4),      // 16
	DDR_CTL_REG(INIT5),      // 17
	DDR_CTL_REG(DIMMCTL),    // 18
	DDR_CTL_REG(RANKCTL),    // 19
	DDR_CTL_REG(DRAMTMG0),   // 20
	DDR_CTL_REG(DRAMTMG1),   // 21
	DDR_CTL_REG(DRAMTMG2),   // 22
	DDR_CTL_REG(DRAMTMG3),   // 23
	DDR_CTL_REG(DRAMTMG4),   // 24
	DDR_CTL_REG(DRAMTMG5),   // 25
	DDR_CTL_REG(DRAMTMG8),   // 26
	DDR_CTL_REG(DRAMTMG15),  // 27
	DDR_CTL_REG(ZQCTL0),     // 28
	DDR_CTL_REG(ZQCTL1),     // 29
	DDR_CTL_REG(DFITMG0),    // 30
	DDR_CTL_REG(DFITMG1),    // 31
	DDR_CTL_REG(DFILPCFG0),  // 32
	DDR_CTL_REG(DFIUPD0),    // 33
	DDR_CTL_REG(DFIUPD1),    // 34
	DDR_CTL_REG(DFIUPD2),    // 35
	DDR_CTL_REG(DFIMISC),    // 36
	DDR_CTL_REG(DFIPHYMSTR), // 37
	DDR_CTL_REG(ADDRMAP0),   // 38
	DDR_CTL_REG(ADDRMAP1),   // 39
	DDR_CTL_REG(ADDRMAP2),   // 40
	DDR_CTL_REG(ADDRMAP3),   // 41
	DDR_CTL_REG(ADDRMAP4),   // 42
	DDR_CTL_REG(ADDRMAP5),   // 43
	DDR_CTL_REG(ADDRMAP6),   // 44
	DDR_CTL_REG(ADDRMAP9),   // 45
	DDR_CTL_REG(ADDRMAP10),  // 46
	DDR_CTL_REG(ADDRMAP11),  // 47
	DDR_CTL_REG(ODTCFG),     // 48
	DDR_CTL_REG(ODTMAP),     // 49
	DDR_CTL_REG(SCHED),      // 50
	DDR_CTL_REG(SCHED1),     // 51
	DDR_CTL_REG(PERFHPR1),   // 52
	DDR_CTL_REG(PERFLPR1),   // 53
	DDR_CTL_REG(PERFWR1),    // 54
	DDR_CTL_REG(DBG0),       // 55
	DDR_CTL_REG(DBG1_2),     // 56
	DDR_CTL_REG(DBGCMD),     // 57
	DDR_CTL_REG(SWCTL_1),    // 58
	DDR_CTL_REG(SWCTLSTATIC),// 59
	DDR_CTL_REG(POISONCFG),  // 60
	DDR_CTL_REG(PCTRL_0),    // 61
	DDR_CTL_REG(PCTRL_1),    // 62
	DDR_CTL_REG(PCTRL_2),    // 63
	DDR_CTL_REG(PCTRL_3),    // 64
	DDR_CTL_REG(PCTRL_4),    // 65
	DDR_CTL_REG(PCTRL_5),    // 66
	DDR_CTL_REG(PCTRL_6),    // 67
	DDR_CTL_REG(PCCFG),      // 68
	DDR_CTL_REG(PCFGR_0),    // 69
	DDR_CTL_REG(PCFGR_1),    // 70
	DDR_CTL_REG(PCFGR_2),    // 71
	DDR_CTL_REG(PCFGR_3),    // 72
	DDR_CTL_REG(PCFGR_4),    // 73
	DDR_CTL_REG(PCFGR_5),    // 74
	DDR_CTL_REG(PCFGR_6),    // 75
	DDR_CTL_REG(PCFGW_0),    // 76
	DDR_CTL_REG(PCFGW_1),    // 77
	DDR_CTL_REG(PCFGW_2),    // 78
	DDR_CTL_REG(PCFGW_3),    // 79
	DDR_CTL_REG(PCFGW_4),    // 80
	DDR_CTL_REG(PCFGW_5),    // 81
	DDR_CTL_REG(PCFGW_6),    // 82
	DDR_CTL_REG(SARBASE0),   // 83
	DDR_CTL_REG(SARSIZE0),   // 84

	// DDR PHY
	DDR_PHY_REG(DSGCR),      // 85
	DDR_PHY_REG(PGCR1),      // 86
	DDR_PHY_REG(PGCR2),      // 87
	DDR_PHY_REG(PTR0),       // 88
	DDR_PHY_REG(PTR1),       // 89
	DDR_PHY_REG(PTR2),       // 90
	DDR_PHY_REG(PTR3),       // 91
	DDR_PHY_REG(PTR4),       // 92
	DDR_PHY_REG(MR0),        // 93
	DDR_PHY_REG(MR1),        // 94
	DDR_PHY_REG(MR2),        // 95
	DDR_PHY_REG(MR3),        // 96
	DDR_PHY_REG(DTPR0),      // 97
	DDR_PHY_REG(DTPR1),      // 98
	DDR_PHY_REG(DTPR2),      // 99
	DDR_PHY_REG(ZQ0CR1),     // 100
	DDR_PHY_REG(DCR),        // 101
	DDR_PHY_REG(DTCR),       // 102
	DDR_PHY_REG(PLLCR),      // 103
	DDR_PHY_REG(PIR),        // 104

	DDR_CTL_REG(SWCTL_2),    // 105
	DDR_CTL_REG(PWRCTL_3),   // 106
	DDR_CTL_REG(SWCTL_3),    // 107

};

#endif /* _MA35D1_DDR_DEF_H */
