/**************************************************************************//**
 * @file     sdh_reg.h
 * @brief    SDH register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 *****************************************************************************/
#ifndef __SDH_REG_H__
#define __SDH_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/** @addtogroup REGISTER Control Register

  @{

*/


/*---------------------- SD Card Host Interface -------------------------*/
/**
    @addtogroup SDH SD Card Host Interface(SDH)
    Memory Mapped Structure for SDH Controller
@{ */
 
typedef struct
{


/**
 * @var SDH_T::SDMASA_R
 * Offset: 0x00  SDMA System Address Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |BLOCKCNT_SDMASA|32-bit Block Count
 * |        |          |SDMA System Address (Host Version 4 Enable = 0): This register contains the system memory address for an SDMA transfer in the 32-bit addressing mode
 * |        |          |When the Host Controller stops an SDMA transfer, this register points to the system address of the next contiguous data position
 * |        |          |It can be accessed only if no transaction is executing
 * |        |          |Reading this register during data transfers may return an invalid value.
 * |        |          |32-bit Block Count (Host Version 4 Enable = 1): From the Host Controller Version 4.10 specification, this register is redefined as 32-bit Block Count
 * |        |          |The Host Controller decrements the block count of this register for every block transfer and the data transfer stops when the count reaches zero
 * |        |          |This register must be accessed when no transaction is executing
 * |        |          |Reading this register during data transfers may return invalid value.
 * |        |          |Following are the values for BLOCKCNT_SDMASA:
 * |        |          |0xFFFF_FFFF = 4G - 1 Block
 * |        |          |...
 * |        |          |0x0000_0002 = 2 Blocks
 * |        |          |0x0000_0001 = 1 Block
 * |        |          |0x0000_0000 = Stop Count
 * |        |          |Note:
 * |        |          |For Host Version 4 Enable = 0, the Host driver does not program the system address in this register while operating in ADMA mode
 * |        |          |The system address must be programmed in the ADMA System Address register.
 * |        |          |For Host Version 4 Enable = 0, the Host driver programs a non-zero 32-bit block count value in this register when Auto CMD23 is enabled for non-DMA and ADMA modes
 * |        |          |Auto CMD23 cannot be used with SDMA.
 * |        |          |This register must be programmed with a non-zero value for data transfer if the 32-bit Block count register is used instead of the 16-bit Block count register.
 * @var SDH_T::BLOCKSIZE_R
 * Offset: 0x04  Block Size Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[11:0]  |XFER_BLOCK_SIZE|Transfer Block Size
 * |        |          |These bits specify the block size of data transfers
 * |        |          |In case of memory, it is set to 512 bytes
 * |        |          |It can be accessed only if no transaction is executing
 * |        |          |Read operations during transfers may return an invalid value, and write operations are ignored
 * |        |          |Following are the values for XFER_BLOCK_SIZE:
 * |        |          |0x1 = 1 byte
 * |        |          |0x2 = 2 bytes
 * |        |          |0x3 = 3 bytes
 * |        |          |......
 * |        |          |0x1FF = 511 byte
 * |        |          |0x200 = 512 bytes
 * |        |          |......
 * |        |          |0x800 = 2048 bytes
 * |        |          |Note: This register must be programmed with a non-zero value for data transfer. 
 * |[14:12] |SDMA_BUF_BDARY|SDMA Buffer Boundary
 * |        |          |These bits specify the size of contiguous buffer in system memory
 * |        |          |The SDMA transfer waits at every boundary specified by these fields and the Host Controller generates the DMA interrupt to request the Host Driver to update the SDMA System Address register.
 * |        |          |0x0 = 4 Kbytes SDMA Buffer Boundary
 * |        |          |0x1 = 8 Kbytes SDMA Buffer Boundary
 * |        |          |0x2 = 16 Kbytes SDMA Buffer Boundary
 * |        |          |0x3 = 32 Kbytes SDMA Buffer Boundary
 * |        |          |0x4 = 64 Kbytes SDMA Buffer Boundary
 * |        |          |0x5 = 128 Kbytes SDMA Buffer Boundary
 * |        |          |0x6 = 256 Kbytes SDMA Buffer Boundary
 * |        |          |0x7 = 512 Kbytes SDMA Buffer Boundary
 * |[15]    |RSVD_BLOCKSIZE15|This bit of the BLOCKSIZE_R register is reserved and always returns 0.
 * @var SDH_T::BLOCKCOUNT_R
 * Offset: 0x0£½  16-bit Block Count Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |BLOCK_CNT |16-bit Block Count
 * |        |          |If the Host Version 4 Enable bit is set 0 or the 16-bit Block Count register is set to non-zero, the 16-bit Block Count register is selected.
 * |        |          |If the Host Version 4 Enable bit is set 1 and the 16-bit Block Count register is set to zero, the 32-bit Block Count register is selected.
 * |        |          |Following are the values for BLOCK_CNT:
 * |        |          |0x0 = Stop Count
 * |        |          |0x1 = 1 Block
 * |        |          |0x2 = 2 Blocks
 * |        |          |... - ...
 * |        |          |0xFFFF = 65535 Blocks
 * |        |          |Note: For Host Version 4 Enable = 0, this register must be set to 0000h before programming the 32-bit block count register when Auto CMD23 is enabled for non-DMA and ADMA modes
 * @var SDH_T::ARGUMENT_R
 * Offset: 0x08  Argument Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |ARGUMENT  |Command Argument
 * |        |          |These bits specify the SD/eMMC command argument that is specified in bits 39-8 of the Command format.
 * @var SDH_T::XFER_MODE_R
 * Offset: 0x0c  Transfer Mode Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |DMA_ENABLE|DMA Enable
 * |        |          |This bit enables the DMA functionality
 * |        |          |If this bit is set to 1, a DMA operation begins when the Host Driver writes to the Command register
 * |        |          |You can select one of the DMA modes by using DMA Select in the Host Control 1 register.
 * |        |          |0x1 (ENABLED) = DMA Data transfer
 * |        |          |0x0 (DISABLED) = No data transfer or Non-DMA data transfer
 * |[1]     |BLOCK_COUNT_ENABLE|Block Count Enable
 * |        |          |This bit is used to enable the Block Count register, which is relevant for multiple block transfers
 * |        |          |If this bit is set to 0, the Block Count register is disabled, which is useful in executing an infinite transfer
 * |        |          |The Host Driver must set this bit to 0 when ADMA is used.
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |0x0 (DISABLED) = Disable
 * |[3:2]   |AUTO_CMD_ENABLE|Auto Command Enable
 * |        |          |This field determines use of Auto Command functions.
 * |        |          |Note: In SDIO, this field must be set as 00b (Auto Command Disabled).
 * |        |          |0x0 (AUTO_CMD_DISABLED) = Auto Command Disabled
 * |        |          |0x1 (AUTO_CMD12_ENABLED) = Auto CMD12 Enable
 * |        |          |0x2 (AUTO_CMD23_ENABLED) = Auto CMD23 Enable
 * |        |          |0x3 (AUTO_CMD_AUTO_SEL) = Auto CMD Auto Select
 * |[4]     |DATA_XFER_DIR|Data Transfer Direction Select
 * |        |          |This bit defines the direction of DAT line data transfers
 * |        |          |This bit is set to 1 by the Host Driver to transfer data from the SD/eMMC card to the Host Controller and it is set to 0 for all other commands.
 * |        |          |0x1 (READ) = Read (Card to Host)
 * |        |          |0x0 (WRITE) = Write (Host to Card)
 * |[5]     |MULTI_BLK_SEL|Multi/Single Block Select
 * |        |          |This bit is set when issuing multiple-block transfer commands using the DAT line
 * |        |          |If this bit is set to 0, it is not necessary to set the Block Count register.
 * |        |          |0x0 (SINGLE) = Single Block
 * |        |          |0x1 (MULTI) = Multiple Block
 * |[6]     |RESP_TYPE |Response Type R1/R5
 * |        |          |This bit selects either R1 or R5 as a response type when the Response Error Check is selected.
 * |        |          |Error statuses checked in R1:
 * |        |          | OUT_OF_RANGE
 * |        |          | ADDRESS_ERROR
 * |        |          | BLOCK_LEN_ERROR
 * |        |          | WP_VIOLATION
 * |        |          | CARD_IS_LOCKED
 * |        |          | COM_CRC_ERROR
 * |        |          | CARD_ECC_FAILED
 * |        |          | CC_ERROR
 * |        |          | ERROR
 * |        |          |Response Flags checked in R5:
 * |        |          | COM_CRC_ERROR
 * |        |          | ERROR
 * |        |          | FUNCTION_NUMBER
 * |        |          | OUT_OF_RANGE
 * |        |          |0x0 (RESP_R1) = R1 (Memory)
 * |        |          |0x1 (RESP_R5) = R5 (SDIO)
 * |[7]     |RESP_ERR_CHK_ENABLE|Response Error Check Enable
 * |        |          |0x0 = Response Error Check is disabled
 * |        |          |0x1 = Response Error Check is enabled
 * |        |          |The Host Controller supports response check function to avoid overhead of response error check by Host driver
 * |        |          |Response types of only R1 and R5 can be checked by the Controller
 * |        |          |If the Host Controller checks the response error, set this bit to 1 and set Response Interrupt Disable to 1
 * |        |          |If an error is detected, the Response Error interrupt is generated in the Error Interrupt Status register.
 * |        |          |Note:
 * |        |          | Response error check must not be enabled for any response type other than R1 and R5.
 * |        |          | Response check must not be enabled for the tuning command. 
 * |[8]     |RESP_INT_DISABLE|Response Interrupt Disable
 * |        |          |The Host Controller supports response check function to avoid overhead of response error check by the Host driver
 * |        |          |Response types of only R1 and R5 can be checked by the Controller.
 * |        |          |If Host Driver checks the response error, set this bit to 0 and wait for Command Complete Interrupt and then check the response register.
 * |        |          |If the Host Controller checks the response error, set this bit to 1 and set the Response Error Check Enable bit to 1
 * |        |          |The Command Complete Interrupt is disabled by this bit regardless of the Command Complete Signal Enable.
 * |        |          |Note: During tuning (when the Execute Tuning bit in the Host Control2 register is set), the Command Complete Interrupt is not generated irrespective of the Response Interrupt Disable setting.
 * |        |          |0x0 = Response Interrupt is enabled
 * |        |          |0x1 = Response Interrupt is disabled
 * |[15:9]  |RSVD      |These bits of the XFER_MODE_R register are reserved and always return 0.
 * @var SDH_T::CMD_R
 * Offset: 0x0e  Command Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[1:0]   |RESP_TYPE_SELECT|Response Type Select
 * |        |          |0x0 (NO_RESP) = No Response
 * |        |          |0x1 (RESP_LEN_136) = Response Length 136
 * |        |          |0x2 (RESP_LEN_48) = Response Length 48
 * |        |          |0x3 (RESP_LEN_48B) = Response Length 48; Check Busy after response
 * |        |          |This bit indicates the type of response expected from the card.
 * |[2]     |SUB_CMD_FLAG|Sub Command Flag
 * |        |          |0x0 (MAIN) = Main Command
 * |        |          |0x1 (SUB) = Sub Command
 * |        |          |This bit distinguishes between a main command and a sub command.
 * |[3]     |CMD_CRC_CHK_ENABLE|Command CRC Check Enable
 * |        |          |0x0 (DISABLED) = Disable
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |This bit enables the Host Controller to check the CRC field in the response
 * |        |          |If an error is detected, it is reported as a Command CRC error.
 * |        |          |Note: CRC Check enable must be set to 0 for the command with no response, R3 response, and R4 response.
 * |        |          | For the tuning command, this bit must always be set to 1 to enable the CRC check. 
 * |[4]     |CMD_IDX_CHK_ENABLE|Command Index Check Enable
 * |        |          |0x0 (DISABLED) = Disable
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |This bit enables the Host Controller to check the index field in the response to verify if it has the same value as the command index
 * |        |          |If the value is not the same, it is reported as a Command Index error.
 * |        |          |Note:
 * |        |          |Index Check enable must be set to 0 for the command with no response, R2 response, R3 response and R4 response.
 * |        |          | For the tuning command, this bit must always be set to enable the index check. 
 * |[5]     |DATA_PRESENT_SEL|Data Present Select
 * |        |          |0x0 (NO_DATA) = No Data Present
 * |        |          |0x1 (DATA) = Data Present
 * |        |          |This bit is set to 1 to indicate that data is present and that the data is transferred using the DAT line
 * |        |          |This bit is set to 0 in the following instances:
 * |        |          | Command using the CMD line
 * |        |          | Command with no data transfer but using busy signal on the DAT[0] line
 * |        |          | Resume Command 
 * |[7:6]   |CMD_TYPE  |Command Type
 * |        |          |0x3 (ABORT_CMD) = Abort
 * |        |          |0x2 (RESUME_CMD) = Resume
 * |        |          |0x1 (SUSPEND_CMD) = Suspend
 * |        |          |0x0 (NORMAL_CMD) = Normal
 * |        |          |These bits indicate the command type.
 * |        |          |Note: While issuing Abort CMD using CMD12/CMD52 or reset CMD using CMD0/CMD52, CMD_TYPE field shall be set to 0x3.
 * |[13:8]  |CMD_INDEX |Command Index
 * |        |          |These bits are set to the command number that is specified in bits 45-40 of the Command Format.
 * |[15:14] |RSVD      |These bits of the CMD_R register are reserved and always return 0.
 * @var SDH_T::RESP01_R
 * Offset: 0x10  Response Register 0/1
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |RESP01    |Command Response
 * |        |          |These bits reflect 39-8 bits of SD/eMMC Response Field.
 * |        |          |Note: For Auto CMD, the 32-bit response (bits 39-8 of the Response Field) is updated in the RESP67_R register
 * @var SDH_T::RESP23_R
 * Offset: 0x14  Response Register 2/3
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |RESP23    |Command Response
 * |        |          |These bits reflect 71-40 bits of the SD/eMMC Response Field.
 * @var SDH_T::RESP45_R
 * Offset: 0x18  Response Register 4/5
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |RESP45    |Command Response
 * |        |          |These bits reflect 103-72 bits of the Response Field. 
 * @var SDH_T::RESP67_R
 * Offset: 0x1c  Response Register 6/7
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |RESP67    |Command Response
 * |        |          |These bits reflect bits 135-104 of SD/EMMC Response Field.
 * |        |          |Note: For Auto CMD, this register also reflects the 32-bit response (bits 39-8 of the Response Field). 
 * @var SDH_T::BUF_DATA_R
 * Offset: 0x20  Buffer Data Port Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |BUF_DATA  |Buffer Data
 * |        |          |These bits enable access to the Host Controller packet buffer.
 * @var SDH_T::PSTATE_REG
 * Offset: 0x24  Present State Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_INHIBIT|Command Inhibit
 * |        |          |0x0 (READY) = Host Controller is ready to issue a command
 * |        |          |0x1 (NOT_READY) = Host Controller is not ready to issue a command
 * |        |          |This bit indicates the following :
 * |        |          |SD/eMMC mode: If this bit is set to 0, it indicates that the CMD line is not in use and the Host controller can issue an SD/eMMC command using the CMD line
 * |        |          |This bit is set when the command register is written
 * |        |          |This bit is cleared when the command response is received
 * |        |          |This bit is not cleared by the response of auto CMD12/23 but cleared by the response of read/write command
 * |[1]     |CMD_INHIBIT_DAT|Command Inhibit DAT
 * |        |          |0x0 (READY) = Can issue command which used DAT line
 * |        |          |0x1 (NOT_READY) = Cannot issue command which used DAT line
 * |        |          |This bit is applicable for SD/eMMC mode and is generated if either DAT line active or Read transfer active is set to 1
 * |        |          |If this bit is set to 0, it indicates that the Host Controller can issue subsequent SD/eMMC commands
 * |[2]     |DAT_LINE_ACTIVE|DAT Line Active
 * |        |          |0x0 (INACTIVE) = DAT Line Inactive
 * |        |          |0x1 (ACTIVE) = DAT Line Active
 * |        |          |This bit indicates whether one of the DAT lines on the SD/eMMC bus is in use.
 * |        |          |In the case of read transactions, this bit indicates whether a read transfer is executing on the SD/eMMC bus.
 * |        |          |In the case of write transactions, this bit indicates whether a write transfer is executing on the SD/eMMC bus.
 * |        |          |For a command with busy, this status indicates whether the command executing busy is executing on an SD or eMMC bus.
 * |        |          |Note: SD/eMMC Mode only.
 * |[3]     |RE_TUNE_REQ|Re-Tuning Request
 * |        |          |DWC_mshc does not generate retuning request. The software must maintain the Retuning timer. 
 * |[7:4]   |DAT_7_4   |DAT[7:4] Line Signal Level
 * |        |          |This bit is used to check the DAT line level to recover from errors and for debugging
 * |        |          |These bits reflect the value of the sd_dat_in (upper nibble) signal
 * |[8]     |WR_XFER_ACTIVE|Write Transfer Active
 * |        |          |This status indicates whether a write transfer is active for SD/eMMC mode. 
 * |[9]     |RD_XFER_ACTIVE|Read Transfer Active
 * |        |          |0x0 (INACTIVE) = No valid data
 * |        |          |0x1 (ACTIVE) = Transferring data
 * |        |          |This bit indicates whether a read transfer is active for SD/eMMC mode. 
 * |[10]    |BUF_WR_ENABLE|Buffer Write Enable
 * |        |          |0x0 (DISABLED) = Write disable
 * |        |          |0x1 (ENABLED) = Write enable
 * |        |          |This bit is used for non-DMA transfers. This bit is set if space is available for writing data.
 * |[11]    |BUF_RD_ENABLE|Buffer Read Enable
 * |        |          |0x0 (DISABLED) = Read disable
 * |        |          |0x1 (ENABLED) = Read enable
 * |        |          |This bit is used for non-DMA transfers. This bit is set if valid data exists in the Host buffer.
 * |[15:12] |RSVD_15_12|These bits of the PRESENT_STAT_R register are reserved and always return 0.
 * |[16]    |CARD_INSERTED|Card Inserted
 * |        |          |0x0 (FALSE) = Reset, Debouncing, or No card
 * |        |          |0x1 (TRUE) = Card Inserted
 * |        |          |This bit indicates whether a card has been inserted
 * |        |          |The Host Controller debounces this signal so that Host Driver need not wait for it to stabilize.
 * |[17]    |CARD_STABLE|Card Stable
 * |        |          |0x0 (FALSE) = Reset or Debouncing
 * |        |          |0x1 (TRUE) = No Card or Inserted
 * |        |          |This bit indicates the stability of the Card Detect Pin Level
 * |        |          |A card is not detected if this bit is set to 1 and the value of the CARD_INSERTED bit is 0.
 * |[18]    |CARD_DETECT_PIN_LEVEL|Card Detect Pin Level
 * |        |          |0x0 (FALSE) = No card present
 * |        |          |0x1 (TRUE) = Card Present
 * |        |          |This bit reflects the inverse synchronized value of the card_detect_n signal.
 * |[19]    |WR_PROTECT_SW_LVL|Write Protect Switch Pin Level
 * |        |          |0x0 (FALSE) = Write protected
 * |        |          |0x1 (TRUE) = Write enabled
 * |        |          |This bit is supported only for memory and combo cards
 * |        |          |This bit reflects the synchronized value of the card_write_prot signal.
 * |[23:20] |DAT_3_0   |DAT[3:0] Line Signal Level
 * |        |          |This bit is used to check the DAT line level to recover from errors and for debugging
 * |        |          |These bits reflect the value of the sd_dat_in (lower nibble) signal
 * |[24]    |CMD_LINE_LVL|Command-Line Signal Level
 * |        |          |This bit is used to check the CMD line level to recover from errors and for debugging
 * |        |          |These bits reflect the value of the sd_cmd_in signal
 * |[25]    |HOST_REG_VOL|Host Regulator Voltage Stable
 * |        |          |This bit is used to check whether the host regulator voltage is stable for switching the voltage of UHS-I mode
 * |        |          |This bit reflects the synchronized value of the host_reg_vol_stable signal
 * |[26]    |RSVD_26   |This bit of the PRESENT_ST_R register is reserved bits and always returns 0.
 * |[27]    |CMD_ISSUE_ERR|Command Not Issued by Error
 * |        |          |0x0 (FALSE) = Host Regulator Voltage is not stable
 * |        |          |0x1 (TRUE) = Host Regulator Voltage is stable
 * |        |          |This bit is set if a command cannot be issued after setting the command register due to an error except the Auto CMD12 error.
 * |[28]    |SUB_CMD_STAT|Sub Command Status
 * |        |          |0x0 (FALSE) = Main Command Status
 * |        |          |0x1 (TRUE) = Sub Command Status
 * |        |          |This bit is used to distinguish between a main command and a sub command status.
 * |[29]    |IN_DORMANT_ST|In Dormant Status
 * |        |          |0x0 (FALSE) = Not in DORMANT state
 * |        |          |0x1 (TRUE) = In DORMANT state
 * |        |          |For SD/eMMC mode, this bit always returns 0. 
 * |[30]    |LANE_SYNC |Lane Synchronization
 * |        |          |0x0 (FALSE) = UHS-II PHY is not initialized
 * |        |          |0x1 (TRUE) = UHS-II PHY is initialized
 * |        |          |For SD/eMMC mode, this bit always returns 0.
 * |[31]    |UHS2_IF_DETECT|UHS-II Interface Detection
 * |        |          |0x0 (FALSE) = UHS-II interface is not detected
 * |        |          |0x1 (TRUE) = UHS-II interface is detected
 * |        |          |For SD/eMMC mode, this bit always returns 0.
 * @var SDH_T::HOST_CTRL1_R
 * Offset: 0x28  Host Control 1 Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |LED_CTRL  |LED Control
 * |        |          |0x0 (OFF) = LED off
 * |        |          |0x1 (ON) = LED on
 * |        |          |This bit is used to caution the user not to remove the card while the SD card is being accessed
 * |        |          |The value is reflected on the led_control signal.
 * |        |          |Note: This bit is unused, do not set this bit to 0x1.
 * |[1]     |DAT_XFER_WIDTH|Data Transfer Width
 * |        |          |0x1 (FOUR_BIT) = 4-bit mode
 * |        |          |0x0 (ONE_BIT) = 1-bit mode
 * |        |          |For SD/eMMC mode, this bit selects the data transfer width of the Host Controller
 * |        |          |The Host Driver sets it to match the data width of the SD/eMMC card
 * |[2]     |HIGH_SPEED_EN|High Speed Enable
 * |        |          |0x1 (HIGH_SPEED) = High Speed mode
 * |        |          |0x0 (NORMAL_SPEED) = Normal Speed mode
 * |        |          |In SD/eMMC mode, this bit is used to determine the selection of preset value for High Speed mode
 * |        |          |Before setting this bit, the Host Driver checks the High Speed Support in the Capabilities register.
 * |        |          |Note = DWC_MSHC always outputs the sd_cmd_out and sd_dat_out lines at the rising edge of cclk_tx clock irrespective of this bit
 * |        |          |Please refer the section Connecting the Clock I/O interface in the Mobile Storage Host Controller user guide on clocking requirement for an SD/eMMC card.
 * |        |          |Note: SD/eMMC Mode only.
 * |[4:3]   |DMA_SEL   |DMA Select
 * |        |          |0x0 (SDMA) = SDMA is selected
 * |        |          |0x1 (RSVD_BIT) = Reserved
 * |        |          |0x2 (ADMA2) = ADMA2 is selected
 * |        |          |0x3 (ADMA2_3) = ADMA2 or ADMA3 is selected
 * |        |          |This field is used to select the DMA type.
 * |        |          |When Host Version 4 Enable is 1 in Host Control 2 register:
 * |        |          |0x0 = SDMA is selected
 * |        |          |0x1 = Reserved
 * |        |          |0x2 = ADMA2 is selected
 * |        |          |0x3 = ADMA2 or ADMA3 is selected
 * |        |          |When Host Version 4 Enable is 0 in Host Control 2 register:
 * |        |          |0x0 = SDMA is selected
 * |        |          |0x1 = Reserved
 * |        |          |0x2 = 32-bit Address ADMA2 is selected
 * |        |          |0x3 = 64-bit Address ADMA2 is selected 
 * |[5]     |EXT_DAT_XFER|Extended Data Transfer Width
 * |        |          |0x1 (CARD_INSERTED) = Card Inserted
 * |        |          |0x0 (No_CARD) = No Card
 * |        |          |This bit controls 8-bit bus width mode of embedded device. 
 * |[6]     |CARD_DETECT_TEST_LVL|Card Detect Test Level
 * |        |          |0x1 (CARD_INSERTED) = Card Inserted
 * |        |          |0x0 (No_CARD) = No Card
 * |        |          |This bit is enabled while the Card Detect Signal Selection is set to 1 and it indicates whether a card inserted or not.
 * |[7]     |CARD_DETECT_SIG_SEL|Card Detect Signal Selection
 * |        |          |0x1 (CARD_DT_TEST_LEVEL) = Card Detect Test Level is selected (for test purpose)
 * |        |          |0x0 (SDCD_PIN) = SDCD# (card_detect_n signal) is selected (for normal use)
 * |        |          |This bit selects a source for card detection
 * |        |          |When the source for the card detection is switched, the interrupt must be disabled during the switching period.
 * @var SDH_T::PWR_CTRL_R
 * Offset: 0x29  Power Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |SD_BUS_PWR_VDD1|SD Bus Power for VDD1
 * |        |          |0x0 (OFF) = Power off
 * |        |          |0x1 (ON) = Power on
 * |        |          |This bit enables VDD1 power of the card
 * |        |          |This setting is available on the sd_vdd1_on output of DWC_mshc so that it can be used to control the VDD1 power supply of the card
 * |        |          |Before setting this bit, the SD Host Driver sets the SD Bus Voltage Select bit
 * |        |          |If the Host Controller detects a No Card state, this bit is cleared.
 * |        |          |In SD mode, if this bit is cleared, the Host Controller stops the SD Clock by clearing the SD_CLK_IN bit in the CLK_CTRL_R register.
 * |[3:1]   |SD_BUS_VOL_VDD1|SD Bus Voltage Select for VDD1/eMMC Bus Voltage Select for VDD
 * |        |          |0x7 (V_3_3) = 3.3V (Typ.)
 * |        |          |0x6 (V_3_0) = 3.0V (Typ.)
 * |        |          |0x5 (V_1_8) = 1.8V (Typ.) for Embedded
 * |        |          |0x4 (RSVD4) = Reserved
 * |        |          |0x3 (RSVD3) = Reserved
 * |        |          |0x2 (RSVD2) = Reserved
 * |        |          |0x1 (RSVD1) = Reserved
 * |        |          |0x0 (RSVD0) = Reserved
 * |        |          |These bits enable the Host Driver to select the voltage level for an SD/eMMC card
 * |        |          |Before setting this register, the Host Driver checks the Voltage Support bits in the Capabilities register
 * |        |          |If an unsupported voltage is selected, the Host System does not supply the SD Bus voltage
 * |        |          |The value set in this field is available on the DWC_mshc output signal (sd_vdd1_sel), which is used by the voltage switching circuitry.
 * |        |          |SD Bus Voltage Select options:
 * |        |          |0x7 = 3.3V(Typical)
 * |        |          |0x6 = 3.0V(Typical)
 * |        |          |0x5 = 1.8V(Typical) for Embedded
 * |        |          |0x4 = 0x0 - Reserved
 * |        |          |eMMC Bus Voltage Select options:
 * |        |          |0x7 = 3.3V(Typical)
 * |        |          |0x6 = 1.8V(Typical)
 * |        |          |0x5 = 1.2V(Typical)
 * |        |          |0x4 = 0x0 - Reserved 
 * |[4]     |SD_BUS_PWR_VDD2|SD Bus Power for VDD2
 * |        |          |This is irrelevant for SD/eMMC card.
 * |        |          |0x0 (OFF) = Power off
 * |        |          |0x1 (ON) = Power on
 * |[7:5]   |SD_BUS_VOL_VDD2|SD Bus Voltage Select for VDD2
 * |        |          |This is irrelevant for SD/eMMC card.
 * |        |          |0x0 (NO_VDD2) = VDD2 Not Supported
 * |        |          |0x1 (RSVD1) = Reserved
 * |        |          |0x2 (RSVD2) = Reserved
 * |        |          |0x3 (RSVD3) = Reserved
 * |        |          |0x4 (V_1_2) = Reserved for 1.2V
 * |        |          |0x5 (V_1_8) = 1.8V
 * |        |          |0x6 (NOT_USED6) = Not used
 * |        |          |0x7 (NOT_USED7) = Not used 
 * @var SDH_T::BGAP_CTRL_R
 * Offset: 0x2a  Block Gap Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |STOP_BG_REQ|Stop At Block Gap Request
 * |        |          |0x0 (XFER) = Transfer
 * |        |          |0x1 (STOP) = Stop
 * |        |          |This bit is used to stop executing read and write transactions at the next block gap for non-DMA, SDMA, and ADMA transfers.
 * |[1]     |CONTINUE_REQ|Continue Request
 * |        |          |0x0 (NO_AFFECT) = No Affect
 * |        |          |0x1 (RESTART) = Restart
 * |        |          |This bit is used to restart the transaction, which was stopped using the Stop At Block Gap Request
 * |        |          |The Host Controller automatically clears this bit when the transaction restarts
 * |        |          |If stop at block gap request is set to 1, any write to this bit is ignored
 * |[2]     |RD_WAIT_CTRL|Read Wait Control
 * |        |          |0x0 (DISABLE) = Disable Read Wait Control
 * |        |          |0x1 (ENABLE) = Enable Read Wait Control
 * |        |          |This bit is used to enable the read wait protocol to stop read data using DAT[2] line if the card supports read wait
 * |        |          |Otherwise, the Host Controller has to stop the card clock to hold the read data
 * |[3]     |INT_AT_BGAP|Interrupt At Block Gap
 * |        |          |0x0 (DISABLE) = Disabled
 * |        |          |0x1 (ENABLE) = Enabled
 * |        |          |This bit is valid only in the 4-bit mode of an SDIO card and is used to select a sample point in the interrupt cycle
 * |        |          |Setting to 1 enables interrupt detection at the block gap for a multiple block transfer
 * |[7:4]   |RSVD_7_4  |These bits of the Block Gap Control register are reserved and always return 0.
 * @var SDH_T::WUP_CTRL_R
 * Offset: 0x2b  Wakeup Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CARD_INT  |Wakeup Event Enable on Card Interrupt
 * |        |          |0x0 (DISABLED) = Disable
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |This bit enables wakeup event through a Card Interrupt assertion in the Normal Interrupt Status register
 * |        |          |This bit can be set to 1 if FN_WUS (Wake Up Support) in CIS is set to 1.
 * |[1]     |CARD_INSERT|Wakeup Event Enable on SD Card Insertion
 * |        |          |0x0 (DISABLED) = Disable
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |This bit enables wakeup event through Card Insertion assertion in the Normal Interrupt Status register
 * |        |          |FN_WUS (Wake Up Support) in CIS does not affect this bit.
 * |[2]     |CARD_REMOVAL|Wakeup Event Enable on SD Card Removal
 * |        |          |0x0 (DISABLED) = Disable
 * |        |          |0x1 (ENABLED) = Enable
 * |        |          |This bit enables wakeup event through Card Removal assertion in the Normal Interrupt Status register
 * |        |          |For the SDIO card, Wake Up Support (FN_WUS) in the Card Information Structure (CIS) register does not affect this bit.
 * |[7:3]   |RSVD_7_3  |These bits of Wakeup Control register are reserved and always return 0.
 * @var SDH_T::CLK_CTRL_R
 * Offset: 0x2c  Clock Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |INTERNAL_CLK_EN|Internal Clock Enable
 * |        |          |0x0 (FALSE) = Stop
 * |        |          |0x1 (TRUE) = Oscillate
 * |        |          |This bit is set to 0 when the Host Driver is not using the Host Controller or the Host Controller awaits a wakeup interrupt
 * |        |          |The Host Controller must stop its internal clock to enter a very low power state
 * |        |          |However, registers can still be read and written to
 * |        |          |The value is reflected on the intclk_en signal.
 * |        |          |Note: If this bit is not used to control the internal clock (base clock and master clock), it is recommended to set this bit to '1' .
 * |[1]     |INTERNAL_CLK_STABLE|Internal Clock Stable
 * |        |          |0x0 (FALSE) = Not Ready
 * |        |          |0x1 (TRUE) = Ready
 * |        |          |This bit enables the Host Driver to check the clock stability twice after the Internal Clock Enable bit is set and after the PLL Enable bit is set
 * |        |          |This bit reflects the synchronized value of the intclk_stable signal after the Internal Clock Enable bit is set to 1 and also reflects the synchronized value of the card_clk_stable signal after the PLL Enable bit is set to 1.
 * |[2]     |SD_CLK_EN |SD/eMMC Clock Enable
 * |        |          |0x0 (FALSE) = Disable providing SDCLK/RCLK
 * |        |          |0x1 (TRUE) = Enable providing SDCLK/RCLK
 * |        |          |This bit stops the SDCLK or RCLK when set to 0
 * |        |          |The SDCLK/RCLK Frequency Select bit can be changed when this bit is set to 0
 * |        |          |The value is reflected on the clk2card_on pin.
 * |[3]     |PLL_ENABLE|PLL Enable
 * |        |          |0x0 (FALSE): PLL is in low power mode
 * |        |          |0x1 (TRUE) = PLL is enabled
 * |        |          |This bit is used to activate the PLL (applicable when Host Version 4 Enable = 1)
 * |        |          |When Host Version 4 Enable = 0, INTERNAL_CLK_EN bit may be used to activate PLL
 * |        |          |The value is reflected on the card_clk_en signal.
 * |        |          |Note: If this bit is not used to to active the PLL when Host Version 4 Enable = 1, it is recommended to set this bit to '1' .
 * |[4]     |RSVD_4    |This bit of the CLK_CTRL_R register is reserved and always returns 0.
 * |[5]     |CLK_GEN_SELECT|Clock Generator Select
 * |        |          |0x0 (FALSE) = Divided Clock Mode
 * |        |          |0x1 (TRUE) = Programmable Clock Mode
 * |        |          |This bit is used to select the clock generator mode in SDCLK/RCLK Frequency Select
 * |        |          |If Preset Value Enable = 0, this bit is set by the Host Driver
 * |        |          |If Preset Value Enable = 1, this bit is automatically set to a value specified in one of the Preset Value registers
 * |        |          |The value is reflected on the card_clk_gen_sel signal.
 * |[7:6]   |UPPER_FREQ_SEL|These bits specify the upper 2 bits of 10-bit SDCLK/RCLK Frequency Select control The value is reflected on the upper 2 bits of the card_clk_freq_sel signal.
 * |[15:8]  |FREQ_SEL  |SDCLK/RCLK Frequency Select
 * |        |          |These bits are used to select the frequency of the SDCLK signal
 * |        |          |These bits depend on setting of Preset Value Enable in the Host Control 2 register
 * |        |          |If Preset Value Enable = 0, these bits are set by the Host Driver
 * |        |          |If Preset Value Enable = 1, these bits are automatically set to a value specified in one of the Preset Value register
 * |        |          |The value is reflected on the lower 8-bit of the card_clk_freq_sel signal.
 * |        |          |10-bit Divided Clock Mode:
 * |        |          |0x3FF = 1/2046 Divided clock
 * |        |          |..........
 * |        |          |N : 1/2N Divided Clock
 * |        |          |..........
 * |        |          |0x002 = 1/4 Divided Clock
 * |        |          |0x001 = 1/2 Divided Clock
 * |        |          |0x000 = Base clock
 * @var SDH_T::TOUT_CTRL_R
 * Offset: 0x2e  Timeout Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[3:0]   |TOUT_CNT  |Data Timeout Counter Value
 * |        |          |This value determines the interval by which DAT line timeouts are detected
 * |        |          |The Timeout clock frequency is generated by dividing the base clock TMCLK value by this value
 * |        |          |When setting this register, prevent inadvertent timeout events by clearing the Data Timeout Error Status Enable (in the Error Interrupt Status Enable register)
 * |        |          |The values for these bits are:
 * |        |          |0xF = Reserved
 * |        |          |0xE = TMCLK x 2^27
 * |        |          |.........
 * |        |          |0x1 = TMCLK x 2^14
 * |        |          |0x0 = TMCLK x 2^13
 * |        |          |Note: During a boot operating in an eMMC mode, an application must configure the boot data timeout value (approximately 1 sec) in this bit
 * |[7:4]   |RSVD_7_4  |These bits of the Timeout Control register are reserved and always return 0.
 * @var SDH_T::SW_RST_R
 * Offset: 0x2f  Software Reset Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |SW_RST_ALL|Software   Reset For All
 * |        |          |0x0 (FALSE) = Work
 * |        |          |0x1 (TRUE) = Reset
 * |        |          |This reset affects the entire Host Controller   except for the card detection circuit
 * |        |          |During its initialization, the Host   Driver sets this bit to 1 to reset the Host Controller
 * |        |          |All registers are   reset except the capabilities register
 * |        |          |If this bit is set to 1, the Host   Driver must issue reset command and reinitialize the card.
 * |[1]     |SW_RST_CMD|Software   Reset For CMD line
 * |        |          |0x0 (FALSE) = Work
 * |        |          |0x1 (TRUE) = Reset
 * |        |          |This bit resets only a part of the command   circuit to be able to issue a command
 * |        |          |This reset is effective only for a   command issuing circuit (including response error statuses related to Command   Inhibit (CMD) control) and does not affect the data transfer circuit
 * |        |          |Host   Controller can continue data transfer even after this reset is executed while   handling subcommand-response errors.
 * |        |          |The following   registers and bits are cleared by this bit:
 * |        |          |   Present State register   : Command Inhibit (CMD) bit
 * |        |          |   Normal Interrupt   Status register : Command Complete bit
 * |        |          |   Error Interrupt Status   : Response error statuses related to Command Inhibit (CMD) bit 
 * |[2]     |SW_RST_DAT|Software   Reset For DAT line
 * |        |          |0x0 (FALSE) = Work
 * |        |          |0x1 (TRUE) = Reset
 * |        |          |This bit is   used in SD/eMMC mode and it resets only a part of the data circuit and the   DMA circuit is also reset.
 * |        |          |The following   registers and bits are cleared by this bit:
 * |        |          |Buffer   Data Port register
 * |        |          |   Buffer is cleared and   initialized.
 * |        |          |Present   state register
 * |        |          |   Buffer Read Enable
 * |        |          |   Buffer Write Enable
 * |        |          |   Read Transfer Active
 * |        |          |   Write Transfer Active
 * |        |          |   DAT Line Active
 * |        |          |   Command Inhibit (DAT)
 * |        |          |Block Gap   Control register
 * |        |          |   Continue Request
 * |        |          |   Stop At Block Gap   Request
 * |        |          |Normal   Interrupt status register
 * |        |          |   Buffer Read Ready
 * |        |          |   Buffer Write Ready
 * |        |          |   DMA Interrupt
 * |        |          |   Block Gap Event
 * |        |          |   Transfer Complete 
 * |[7:3]   |RSVD_7_3  |These bits of   the SW_RST_R register are reserved and always return 0.
 * @var SDH_T::NORMAL_INT_STAT_R
 * Offset: 0x30  Normal Interrupt Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_COMPLETE|Command Complete
 * |        |          |0x0 (FALSE) = No command complete
 * |        |          |0x1 (TRUE) = Command Complete
 * |        |          |In an SD/eMMC Mode, this bit is set when the end bit of a response except for Auto CMD12 and Auto CMD23.
 * |        |          |This interrupt is not generated when the Response Interrupt Disable in Transfer Mode Register is set to 1.
 * |[1]     |XFER_COMPLETE|Transfer Complete
 * |        |          |0x0 (FALSE) = Not complete
 * |        |          |0x1 (TRUE) = Command execution is completed
 * |        |          |This bit is set when a read/write transfer and a command with status busy is completed.
 * |[2]     |BGAP_EVENT|Block Gap Event
 * |        |          |0x0 (FALSE) = No Block Gap Event
 * |        |          |0x1 (TRUE) = Transaction stopped at block gap
 * |        |          |This bit is set when both read/write transaction is stopped at block gap due to a Stop at Block Gap Request.
 * |[3]     |DMA_INTERRUPT|DMA Interrupt
 * |        |          |0x0 (FALSE) = No DMA Interrupt
 * |        |          |0x1 (TRUE) = DMA Interrupt is generated
 * |        |          |This bit is set if the Host Controller detects the SDMA Buffer Boundary during transfer
 * |        |          |In case of ADMA, by setting the Int field in the descriptor table, the Host controller generates this interrupt
 * |        |          |This interrupt is not generated after a Transfer Complete.
 * |[4]     |BUF_WR_READY|Buffer Write Ready
 * |        |          |0x0 (FALSE) = Not ready to write buffer
 * |        |          |0x1 (TRUE) = Ready to write buffer
 * |        |          |This bit is set if the Buffer Write Enable changes from 0 to 1.
 * |[5]     |BUF_RD_READY|Buffer Read Ready
 * |        |          |0x0 (FALSE) = Not ready to read buffer
 * |        |          |0x1 (TRUE) = Ready to read buffer
 * |        |          |This bit is set if the Buffer Read Enable changes from 0 to 1.
 * |[6]     |CARD_INSERTION|Card Insertion
 * |        |          |0x0 (FALSE) = Card state stable or Debouncing
 * |        |          |0x1 (TRUE) = Card Inserted
 * |        |          |This bit is set if the Card Inserted in the Present State register changes from 0 to 1.
 * |[7]     |CARD_REMOVAL|Card Removal
 * |        |          |0x0 (FALSE) = Card state stable or Debouncing
 * |        |          |0x1 (TRUE) = Card Removed
 * |        |          |This bit is set if the Card Inserted in the Present State register changes from 1 to 0.
 * |[8]     |CARD_INTERRUPT|Card Interrupt
 * |        |          |0x0 (FALSE) = No Card Interrupt
 * |        |          |0x1 (TRUE) = Generate Card Interrupt
 * |        |          |This bit reflects the synchronized value of:
 * |        |          | DAT[1] Interrupt Input for SD Mode
 * |        |          | DAT[2] Interrupt Input for UHS-II Mode 
 * |[9]     |INT_A     |INT_A
 * |        |          |This bit is set if INT_A is enabled and if INT_A# pin is in low level
 * |        |          |The INT_A# pin is not supported
 * |[10]    |INT_B     |INT_B
 * |        |          |This bit is set if INT_B is enabled and if INT_B# pin is in low level
 * |        |          |The INT_B# pin is not supported
 * |[11]    |INT_C     |INT_C
 * |        |          |This bit is set if INT_C is enabled and if INT_C# pin is in low level
 * |        |          |The INT_C# pin is not supported
 * |[12]    |RE_TUNE_EVENT|Re-tuning Event
 * |        |          |This bit is set if the Re-Tuning Request changes from 0 to 1. Re-Tuning request is not supported. 
 * |[13]    |FX_EVENT  |FX Event
 * |        |          |0x0 (FALSE) = No Event
 * |        |          |0x1 (TRUE) = FX Event is detected
 * |        |          |This status is set when R[14] of response register is set to 1 and Response Type R1/R5 is set to 0 in Transfer Mode register
 * |        |          |This interrupt is used with response check function.
 * |[14]    |CQE_EVENT |Command Queuing Event
 * |        |          |0x0 (FALSE) = No Event
 * |        |          |0x1 (TRUE) = Command Queuing Event is detected
 * |        |          |This status is set if Command Queuing/Crypto related event has occurred in eMMC/SD mode
 * |        |          |Read CQHCI's CQIS/CRNQIS register for more details
 * |[15]    |ERR_INTERRUPT|Error Interrupt
 * |        |          |0x0 (FALSE) = No Error
 * |        |          |0x1 (TRUE) = Error
 * |        |          |If any of the bits in the Error Interrupt Status register are set, then this bit is set.
 * @var SDH_T::ERROR_INT_STAT_R
 * Offset: 0x32  Error Interrupt Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_TOUT_ERR|Command Timeout Error
 * |        |          |In SD/eMMC Mode, this bit is set only if no response is returned within 64 SD clock cycles from the end bit of the command
 * |        |          |If the Host Controller detects a CMD line conflict, along with Command CRC Error bit, this bit is set to 1, without waiting for 64 SD/eMMC card clock cycles.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Time out 
 * |[1]     |CMD_CRC_ERR|Command CRC Error
 * |        |          |Command CRC Error is generated in SD/eMMC mode for following two cases.
 * |        |          |If a response is returned and the Command Timeout Error is set to 0 (indicating no timeout), this bit is set to 1 when detecting a CRC error in the command response.
 * |        |          |The Host Controller detects a CMD line conflict by monitoring the CMD line when a command is issued
 * |        |          |If the Host Controller drives the CMD line to 1 level, but detects 0 level on the CMD line at the next SD clock edge, then the Host Controller aborts the command (stop driving CMD line) and set this bit to 1
 * |        |          |The Command Timeout Error is also set to 1 to distinguish a CMD line conflict.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = CRC error generated 
 * |[2]     |CMD_END_BIT_ERR|Command End Bit Error
 * |        |          |This bit is set when detecting that the end bit of a command response is 0 in SD/eMMC mode.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = End Bit error generated 
 * |[3]     |CMD_IDX_ERR|Command Index Error
 * |        |          |This bit is set if a Command Index error occurs in the command respons in SD/eMMC mode.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[4]     |DATA_TOUT_ERR|Data Timeout Error
 * |        |          |This bit is set in SD/eMMC mode when detecting one of the following timeout conditions:
 * |        |          |Busy timeout for R1b, R5b type
 * |        |          |Busy timeout after Write CRC status
 * |        |          |Write CRC Status timeout
 * |        |          |Read Data timeout
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Time out 
 * |[5]     |DATA_CRC_ERR|Data CRC Error
 * |        |          |This error occurs in SD/eMMC mode when detecting CRC error when transferring read data which uses the DAT line, when detecting the Write CRC status having a value of other than 010 or when write CRC status timeout.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[6]     |DATA_END_BIT_ERR|Data End Bit Error
 * |        |          |This error occurs in SD/eMMC mode either when detecting 0 at the end bit position of read data that uses the DAT line or at the end bit position of the CRC status.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[7]     |CUR_LMT_ERR|Current Limit Error
 * |        |          |By setting the SD Bus Power bit in the Power Control register, the Host Controller is requested to supply power for the SD Bus
 * |        |          |If the Host Controller supports the Current Limit function, it can be protected from an illegal card by stopping power supply to the card in which case this bit indicates a failure status
 * |        |          |A reading of 1 for this bit means that the Host Controller is not supplying power to the SD card due to some failure
 * |        |          |A reading of 0 for this bit means that the Host Controller is supplying power and no error has occurred
 * |        |          |The Host Controller may require some sampling time to detect the current limit
 * |        |          |DWC_mshc Host Controller does not support this function, this bit is always set to 0.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Power Fail 
 * |[8]     |AUTO_CMD_ERR|Auto CMD Error
 * |        |          |This error status is used by Auto CMD12 and Auto CMD23 in SD/eMMC mode
 * |        |          |This bit is set when detecting that any of the bits D00 to D05 in Auto CMD Error Status register has changed from 0 to 1
 * |        |          |D07 is effective in case of Auto CMD12
 * |        |          |Auto CMD Error Status register is valid while this bit is set to 1 and may be cleared by clearing of this bit.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[9]     |ADMA_ERR  |ADMA Error
 * |        |          |This bit is set when the Host Controller detects error during ADMA-based data transfer
 * |        |          |The error could be due to following reasons:
 * |        |          |Error response received from System bus (Master I/F)
 * |        |          |ADMA3,ADMA2 Descriptors invalid
 * |        |          |CQE Task or Transfer descriptors invalid
 * |        |          |When the error occurs, the state of the ADMA is saved in the ADMA Error Status register.
 * |        |          |In eMMC CQE mode:
 * |        |          |The Host Controller generates this Interrupt when it detects an invalid descriptor data (Valid=0) at the ST_FDS state
 * |        |          |ADMA Error State in the ADMA Error Status indicates that an error has occurred in ST_FDS state
 * |        |          |The Host Driver may find that Valid bit is not set at the error descriptor.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[10]    |TUNING_ERR|Tuning Error
 * |        |          |This bit is set when an unrecoverable error is detected in a tuning circuit except during the tuning procedure (occurrence of an error during tuning procedure is indicated by Sampling Clock Select in the Host Control 2 register)
 * |        |          |By detecting Tuning Error, Host Driver needs to abort a command executing and perform tuning
 * |        |          |To reset tuning circuit, Sampling Clock Select is set to 0 before executing tuning procedure
 * |        |          |The Tuning Error is higher priority than the other error interrupts generated during data transfer
 * |        |          |By detecting Tuning Error, the Host Driver must discard data transferred by a current read/write command and retry data transfer after the Host Controller retrieved from the tuning circuit error
 * |        |          |This is applicable in SD/eMMC mode.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[11]    |RESP_ERR  |Response Error
 * |        |          |Host Controller Version 4.00 supports response error check function to avoid overhead of response error check by Host Driver during DMA execution
 * |        |          |If Response Error Check Enable is set to 1 in the Transfer Mode register, Host Controller Checks R1 or R5 response
 * |        |          |If an error is detected in a response, this bit is set to 1.This is applicable in SD/eMMC mode.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[12]    |BOOT_ACK_ERR|Boot Acknowledgement Error
 * |        |          |This bit is set when there is a timeout for boot acknowledgement or when detecting boot ack status having a value other than 010
 * |        |          |This is applicable only when boot acknowledgement is expected in eMMC mode.
 * |        |          |0x0 (FALSE) = No error
 * |        |          |0x1 (TRUE) = Error 
 * |[13]    |VENDOR_ERR1|This bit (VENDOR_ERR1) of the ERROR_INT_STAT_R register is reserved and always returns 0.
 * |[14]    |VENDOR_ERR2|This bit (VENDOR_ERR2) of the ERROR_INT_STAT_R register is reserved and always returns 0.
 * |[15]    |VENDOR_ERR3|This bit (VENDOR_ERR3) of the ERROR_INT_STAT_R register is reserved and always returns 0.
 * @var SDH_T::NORMAL_INT_STAT_EN_R
 * Offset: 0x34  Normal Interrupt Status Enable Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_COMPLETE_STAT_EN|Command Complete Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[1]     |XFER_COMPLETE_STAT_EN|Transfer Complete Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[2]     |BGAP_EVENT_STAT_EN|Block Gap Event Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[3]     |DMA_INTERRUPT_STAT_EN|DMA Interrupt Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[4]     |BUF_WR_READY_STAT_EN|Buffer Write Ready Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[5]     |BUF_RD_READY_STAT_EN|Buffer Read Ready Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[6]     |CARD_INSERTION_STAT_EN|Card Insertion Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[7]     |CARD_REMOVAL_STAT_EN|Card Removal Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[8]     |CARD_INTERRUPT_STAT_EN|Card Interrupt Status Enable
 * |        |          |If this bit is set to 0, the Host Controller clears the interrupt request to the System
 * |        |          |The Card Interrupt detection is stopped when this bit is cleared and restarted when this bit is set to 1
 * |        |          |The Host Driver may clear the Card Interrupt Status Enable before servicing the Card Interrupt and may set this bit again after all interrupt requests from the card are cleared to prevent inadvertent interrupts.
 * |        |          |By setting this bit to 0, interrupt input must be masked by implementation so that the interrupt input is not affected by external signal in any state (for example, floating).
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[9]     |INT_A_STAT_EN|INT_A Status Enable
 * |        |          |If this bit is set to 0, the Host Controller clears the interrupt request to the System
 * |        |          |The Host Driver may clear this bit before servicing the INT_A and may set this bit again after all interrupt requests to INT_A pin are cleared to prevent inadvertent interrupts.
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[10]    |INT_B_STAT_EN|INT_B Status Enable
 * |        |          |If this bit is set to 0, the Host Controller clears the interrupt request to the System
 * |        |          |The Host Driver may clear this bit before servicing the INT_B and may set this bit again after all interrupt requests to INT_B pin are cleared to prevent inadvertent interrupts.
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[11]    |INT_C_STAT_EN|INT_C Status Enable
 * |        |          |If this bit is set to 0, the Host Controller clears the interrupt request to the System
 * |        |          |The Host Driver may clear this bit before servicing the INT_C and may set this bit again after all interrupt requests to INT_C pin are cleared to prevent inadvertent interrupts.
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[12]    |RE_TUNE_EVENT_STAT_EN|Re-Tuning Event Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: UHS-I only.
 * |[13]    |FX_EVENT_STAT_EN|FX Event Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |This bit is added from Version 4.10.
 * |[14]    |CQE_EVENT_STAT_EN|CQE Event Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[15]    |RSVD_15   |This bit of the NORMAL_INT_STAT_EN_R register is reserved and always returns 0
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * @var SDH_T::ERROR_INT_STAT_EN_R
 * Offset: 0x36  Error Interrupt Status Enable Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_TOUT_ERR_STAT_EN|Command Timeout Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[1]     |CMD_CRC_ERR_STAT_EN|Command CRC Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[2]     |CMD_END_BIT_ERR_STAT_EN|Command End Bit Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[3]     |CMD_IDX_ERR_STAT_EN|Command Index Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[4]     |DATA_TOUT_ERR_STAT_EN|Data Timeout Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[5]     |DATA_CRC_ERR_STAT_EN|Data CRC Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[6]     |DATA_END_BIT_ERR_STAT_EN|Data End Bit Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[7]     |CUR_LMT_ERR_STAT_EN|Current Limit Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[8]     |AUTO_CMD_ERR_STAT_EN|Auto CMD Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[9]     |ADMA_ERR_STAT_EN|ADMA Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[10]    |TUNING_ERR_STAT_EN|Tuning Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: UHS-I Mode only.
 * |[11]    |RESP_ERR_STAT_EN|Response Error Status Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD Mode only.
 * |[12]    |BOOT_ACK_ERR_STAT_EN|Boot Acknowledgment Error
 * |        |          |Setting this bit to 1 enables setting of Boot Acknowledgment Error in Error Interrupt Status register (ERROR_INT_STAT_R).
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: eMMC Mode only.
 * |[13]    |VENDOR_ERR_STAT_EN1|The 13th bit of Error Interrupt Status Enable register is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[14]    |VENDOR_ERR_STAT_EN2|The 14th bit of Error Interrupt Status Enable register is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled 
 * |[15]    |VENDOR_ERR_STAT_EN3|The 15th bit of Error Interrupt Status Enable register is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * @var SDH_T::NORMAL_INT_SIGNAL_EN_R
 * Offset: 0x38  Normal Interrupt Signal Enable Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_COMPLETE_SIGNAL_EN|Command Complete Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[1]     |XFER_COMPLETE_SIGNAL_EN|Transfer Complete Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[2]     |BGAP_EVENT_SIGNAL_EN|Block Gap Event Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[3]     |DMA_INTERRUPT_SIGNAL_EN|DMA Interrupt Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[4]     |BUF_WR_READY_SIGNAL_EN|Buffer Write Ready Signal Enable
 * |        |          |0x0 (FALSE): Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[5]     |BUF_RD_READY_SIGNAL_EN|Buffer Read Ready Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[6]     |CARD_INSERTION_SIGNAL_EN|Card Insertion Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[7]     |CARD_REMOVAL_SIGNAL_EN|Card Removal Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[8]     |CARD_INTERRUPT_SIGNAL_EN|Card Interrupt Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[9]     |INT_A_SIGNAL_EN|INT_A Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[10]    |INT_B_SIGNAL_EN|INT_B Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[11]    |INT_C_SIGNAL_EN|INT_C Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[12]    |RE_TUNE_EVENT_SIGNAL_EN|Re-Tuning Event Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: UHS-I only.
 * |[13]    |FX_EVENT_SIGNAL_EN|FX Event Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[14]    |CQE_EVENT_SIGNAL_EN|Command Queuing Engine Event Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[15]    |RSVD_15   |This bit of the NORMAL_INT_STAT_R register is reserved and always returns 0.
 * @var SDH_T::ERROR_INT_SIGNAL_EN_R
 * Offset: 0x3a  Error Interrupt Signal Enable Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_TOUT_ERR_SIGNAL_EN|Command Timeout Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[1]     |CMD_CRC_ERR_SIGNAL_EN|Command CRC Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[2]     |CMD_END_BIT_ERR_SIGNAL_EN|Command End Bit Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[3]     |CMD_IDX_ERR_SIGNAL_EN|Command Index Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[4]     |DATA_TOUT_ERR_SIGNAL_EN|Data Timeout Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[5]     |DATA_CRC_ERR_SIGNAL_EN|Data CRC Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[6]     |DATA_END_BIT_ERR_SIGNAL_EN|Data End Bit Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[7]     |CUR_LMT_ERR_SIGNAL_EN|Current Limit Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[8]     |AUTO_CMD_ERR_SIGNAL_EN|Auto CMD Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD/eMMC Mode only.
 * |[9]     |ADMA_ERR_SIGNAL_EN|ADMA Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[10]    |TUNING_ERR_SIGNAL_EN|Tuning Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: UHS-I Mode only.
 * |[11]    |RESP_ERR_SIGNAL_EN|Response Error Signal Enable
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: SD Mode only.
 * |[12]    |BOOT_ACK_ERR_SIGNAL_EN|Boot Acknowledgment Error
 * |        |          |Setting this bit to 1 enables generating interrupt signal when Boot Acknowledgement Error in Error Interrupt Status register is set.
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |        |          |Note: eMMC Mode only.
 * |[13]    |VENDOR_ERR_SIGNAL_EN1|The 14th bit of Error Interrupt Signal Enable is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[14]    |VENDOR_ERR_SIGNAL_EN2|The 15th bit of Error Interrupt Signal Enable is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * |[15]    |VENDOR_ERR_SIGNAL_EN3|The 16th bit of Error Interrupt Signal Enable is reserved
 * |        |          |0x0 (FALSE) = Masked
 * |        |          |0x1 (TRUE) = Enabled
 * @var SDH_T::AUTO_CMD_STAT_R
 * Offset: 0x3c  Auto CMD Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |AUTO_CMD12_NOT_EXEC|Auto CMD12 Not Executed
 * |        |          |If multiple memory block data transfer is not started due to a command error, this bit is not set because it is not necessary to issue an Auto CMD12
 * |        |          |Setting this bit to 1 means that the Host Controller cannot issue Auto CMD12 to stop multiple memory block data transfer, due to some error
 * |        |          |If this bit is set to 1, error status bits (D04-D01) is meaningless.
 * |        |          |This bit is set to 0 when Auto CMD Error is generated by Auto CMD23.
 * |        |          |0x0 (FALSE) = Not Executed
 * |        |          |0x1 (TRUE) = Executed
 * |[1]     |AUTO_CMD_TOUT_ERR|Auto CMD Timeout Error
 * |        |          |This bit is set if no response is returned with 64 SDCLK cycles from the end bit of the command.
 * |        |          |If this bit is set to 1, error status bits (D04-D01) are meaningless.
 * |        |          |0x0 (FALSE) = Time out
 * |        |          |0x1 (TRUE) = No Error
 * |[2]     |AUTO_CMD_CRC_ERR|Auto CMD CRC Error
 * |        |          |This bit is set when detecting a CRC error in the command response.
 * |        |          |0x0 (FALSE) = CRC Error Generated
 * |        |          |0x1 (TRUE) = No Error
 * |[3]     |AUTO_CMD_EBIT_ERR|Auto CMD End Bit Error
 * |        |          |This bit is set when detecting that the end bit of command response is 0.
 * |        |          |0x0 (FALSE) = End Bit Error Generated
 * |        |          |0x1 (TRUE) = No Error
 * |[4]     |AUTO_CMD_IDX_ERR|Auto CMD Index Error
 * |        |          |This bit is set if the command index error occurs in response to a command.
 * |        |          |0x1 (TRUE) = Error
 * |        |          |0x0 (FALSE) = No Error 
 * |[5]     |AUTO_CMD_RESP_ERR|Auto CMD Response Error
 * |        |          |This bit is set when Response Error Check Enable in the Transfer Mode register is set to 1 and an error is detected in R1 response of either Auto CMD12 or CMD13
 * |        |          |This status is ignored if any bit between D00 to D04 is set to 1.
 * |        |          |0x1 (TRUE) = Error
 * |        |          |0x0 (FALSE) = No Error 
 * |[6]     |RSVD_6    |This bit of the AUTO_CMD_STAR_R register is reserved and always returns 0.
 * |[7]     |CMD_NOT_ISSUED_AUTO_CMD12|Command Not Issued By Auto CMD12 Error
 * |        |          |If this bit is set to 1, CMD_wo_DAT is not executed due to an Auto CMD12 Error (D04-D01) in this register.
 * |        |          |This bit is set to 0 when Auto CMD Error is generated by Auto CMD23.
 * |        |          |0x1 (TRUE) = Not Issued
 * |        |          |0x0 (FALSE) = No Error
 * |[15:8]  |RSVD_15_8 |These bits of the AUTO_CMD_STAT_R register are reserved and always return 0.
 * @var SDH_T::HOST_CTRL2_R
 * Offset: 0x3e  Host Control 2 Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[2:0]   |UHS_MODE_SEL|UHS Mode/eMMC Speed Mode Select
 * |        |          |These bits are used to select UHS mode in the SD mode of operation
 * |        |          |In eMMC mode, these bits are used to select eMMC Speed mode.
 * |        |          |UHS Mode (SD/UHS-II mode only):
 * |        |          |0x0 = SDR12
 * |        |          |0x1 = SDR25
 * |        |          |0x2 = SDR50
 * |        |          |0x3 = SDR104
 * |        |          |0x4 = DDR50
 * |        |          |0x5 = Reserved
 * |        |          |0x6 = Reserved
 * |        |          |0x7 = UHS-II
 * |        |          |eMMC Speed Mode (eMMC mode only):
 * |        |          |0x0 = Legacy
 * |        |          |0x1 = High Speed SDR
 * |        |          |0x2 = Reserved
 * |        |          |0x3 = HS200
 * |        |          |0x4 = High Speed DDR
 * |        |          |0x5 = Reserved
 * |        |          |0x6 = Reserved
 * |        |          |0x7 = HS400
 * |        |          |Note: This Host Controller not supports DDR50 and UHS-II mode(SD/UHS-II mode only), please do not set this bit to 0x4 and 0x7.
 * |        |          |Note: This Host Controller not supports High Speed DDR and HS400 mode(eMMC mode only), please do not set this bit to 0x4 and 0x7.
 * |[3]     |SIGNALING_EN|1.8V Signaling Enable
 * |        |          |This bit controls voltage regulator for I/O cell in UHS-I/eMMC speed modes
 * |        |          |Setting this bit from 0 to 1 starts changing the signal voltage from 3.3V to 1.8V
 * |        |          |Host Controller clears this bit if switching to 1.8 signaling fails
 * |        |          |The value is reflected on the uhs1_swvolt_en pin.
 * |        |          |Note: This bit must be set for all UHS-I speed modes (SDR12/SDR25/SDR50/SDR104/DDR50).
 * |[5:4]   |DRV_STRENGTH_SEL|Driver Strength Select
 * |        |          |This bit is used to select the Host Controller output driver in 1.8V signaling UHS-I/eMMC speed modes
 * |        |          |The bit depends on setting of Preset Value Enable
 * |        |          |The value is reflected on the uhs1_drv_sth pin.
 * |[6]     |EXEC_TUNING|Execute Tuning
 * |        |          |This bit is set to 1 to start the tuning procedure in UHS-I/eMMC speed modes and this bit is automatically cleared when tuning procedure is completed.
 * |[7]     |SAMPLE_CLK_SEL|Sampling Clock Select
 * |        |          |This bit is used by the Host Controller to select the sampling clock in SD/eMMC mode to receive CMD and DAT
 * |        |          |This bit is set by the tuning procedure and is valid after the completion of tuning (when Execute Tuning is cleared)
 * |        |          |Setting this bit to 1 means that tuning is completed successfully and setting this bit to 0 means that tuning has failed
 * |        |          |The value is reflected on the sample_cclk_sel pin.
 * |        |          |0x0 (FALSE) = Fixed clock is used to sample data
 * |        |          |0x1 (TRUE) = Tuned clock is used to sample data
 * |[8]     |UHS2_IF_ENABLE|UHS-II Interface Enable
 * |        |          |This bit is used to enable the UHS-II Interface. The value is reflected on the uhs2_if_en pin.
 * |        |          |0x0 (FALSE) = SD/eMMC Interface Enabled
 * |        |          |0x1 (TRUE) = UHS-II Interface Enabled
 * |        |          |Note: This Host Controller not supports UHS-II mode, please do not set this bit to 0x1. 
 * |[9]     |RSVD_9    |This bit of the HOST_CTRL2_R register is reserved and always returns 0.
 * |[10]    |ADMA2_LEN_MODE|ADMA2 Length Mode
 * |        |          |This bit selects ADMA2 Length mode to be either 16-bit or 26-bit.
 * |        |          |0x0 (FALSE) = 16-bit Data Length Mode
 * |        |          |0x1 (TRUE) = 26-bit Data Length Mode 
 * |[11]    |CMD23_ENABLE|CMD23 Enable
 * |        |          |If the card supports CMD23, this bit is set to 1
 * |        |          |This bit is used to select Auto CMD23 or Auto CMD12 for ADMA3 data transfer.
 * |        |          |0x0 (FALSE) = Auto CMD23 is disabled
 * |        |          |0x1 (TRUE) = Auto CMD23 is enabled
 * |[12]    |HOST_VER4_ENABLE|Host Version 4 Enable
 * |        |          |This bit selects either Version 3.00 compatible mode or Version 4 mode.
 * |        |          |Functions of following fields are modified for Host Version 4 mode:
 * |        |          |SDMA Address: SDMA uses ADMA System Address (05Fh-058h) instead of SDMA System Address register (003h-000h)
 * |        |          | ADMA2/ADMA3 selection: ADMA3 is selected by DMA select in Host Control 1 register
 * |        |          |64-bit ADMA Descriptor Size: 128-bit descriptor is used instead of 96-bit descriptor when 64-bit Addressing is set to 1
 * |        |          |Selection of 32-bit/64-bit System Addressing: Either 32-bit or 64-bit system addressing is selected by 64-bit Addressing bit in this register
 * |        |          |32-bit Block Count: SDMA System Address register (003h-000h) is modified to 32-bit Block Count register
 * |        |          |Note: It is recommended not to program ADMA3 Integrated Descriptor Address registers while operating in Host version less than 4 mode (Host Version 4 Enable = 0).
 * |        |          |0x0 (FALSE) = Version 3.00 compatible mode
 * |        |          |0x1 (TRUE) = Version 4 mode 
 * |[13]    |ADDRESSING|64-bit Addressing
 * |        |          |This bit is effective when Host Version 4 Enable is set to 1.
 * |        |          |0x0 (FALSE) = 32 bits addressing
 * |        |          |0x1 (TRUE) = 64 bits addressing 
 * |[14]    |ASYNC_INT_ENABLE|Asynchronous Interrupt Enable
 * |        |          |This bit can be set if a card supports asynchronous interrupts and Asynchronous Interrupt Support is set to 1 in the Capabilities register
 * |        |          |0x0 (FALSE) = Disabled
 * |        |          |0x1 (TRUE) = Enabled 
 * |[15]    |PRESET_VAL_ENABLE|Preset Value Enable
 * |        |          |This bit enables automatic selection of SDCLK frequency and Driver strength Preset Value registers
 * |        |          |When Preset Value Enable is set, SDCLK frequency generation (Frequency Select and Clock Generator Select) and the driver strength selection are performed by the controller
 * |        |          |These values are selected from set of Preset Value registers based on selected speed mode.
 * |        |          |Note: For more information, see the FAQ on Preset Register in the DWC_mshc Databook.
 * |        |          |0x0 (FALSE) = SDCLK and Driver Strength are controlled by Host Driver
 * |        |          |0x1 (TRUE) = Automatic Selection by Preset Value are Enabled
 * @var SDH_T::CAPABILITIES1_R
 * Offset: 0x40  Capabilities 1 Register - 0 to 31
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[5:0]   |TOUT_CLK_FREQ|Timeout Clock Frequency
 * |        |          |This bit shows the base clock frequency used to detect Data Timeout Error
 * |        |          |The Timeout Clock unit defines the unit of timeout clock frequency
 * |        |          |It can be kHz or MHz.
 * |        |          |0x00 = Get information through another method
 * |        |          |0x01 = 1 kHz / 1 MHz
 * |        |          |0x02 = 2 kHz / 2 MHz
 * |        |          |0x03 = 3 kHz / 3 MHz
 * |        |          |...........
 * |        |          |0x3F = 63 kHz / 63 MHz 
 * |[6]     |RSVD_6    |This bit of the CAPABILITIES1_R register is reserved and always returns 0.
 * |[7]     |TOUT_CLK_UNIT|Timeout Clock Unit
 * |        |          |This bit shows the unit of base clock frequency used to detect Data Timeout Error.
 * |        |          |0x0 ( kHz) = kHz
 * |        |          |0x1 ( MHz) = MHz
 * |[15:8]  |BASE_CLK_FREQ|Base Clock Frequency for SD clock
 * |        |          |These bits indicate the base (maximum) clock frequency for the SD Clock
 * |        |          |The definition of these bits depend on the Host Controller Version.
 * |        |          |6-Bit Base Clock Frequency: This mode is supported by the Host Controller version 1.00 and 2.00
 * |        |          |The upper 2 bits are not effective and are always 0
 * |        |          |The unit values are 1 MHz
 * |        |          |The supported clock range is 10 MHz to 63 MHz.
 * |        |          |0x00 = Get information through another method
 * |        |          |0x01 = 1 MHz
 * |        |          |0x02 = 2 MHz
 * |        |          |.............
 * |        |          |0x3F = 63 MHz
 * |        |          |0x40-0xFF = Not Supported
 * |        |          |8-Bit Base Clock Frequency: This mode is supported by the Host Controller version 3.00
 * |        |          |The unit values are 1 MHz
 * |        |          |The supported clock range is 10 MHz to 255 MHz.
 * |        |          |0x00 = Get information through another method
 * |        |          |0x01 = 1 MHz
 * |        |          |0x02 = 2 MHz
 * |        |          |............
 * |        |          |0xFF = 255 MHz
 * |        |          |If the frequency is 16.5 MHz, the larger value is set to 0001001b (17 MHz) because the Host Driver uses this value to calculate the clock divider value and it does not exceed the upper limit of the SD Clock frequency
 * |        |          |If these bits are all 0, the Host system has to get information using a different method
 * |[17:16] |MAX_BLK_LEN|Maximum Block Length
 * |        |          |This bit indicates the maximum block size that the Host driver can read and write to the buffer in the Host Controller
 * |        |          |The buffer transfers this block size without wait cycles
 * |        |          |The transfer block length is always 512 bytes for the SD Memory irrespective of this bit
 * |        |          |0x0 (ZERO) = 512 Byte
 * |        |          |0x1 (ONE) = 1024 Byte
 * |        |          |0x2 (TWO) = 2048 Byte
 * |        |          |0x3 (THREE) = Reserved
 * |[18]    |Embedded_8_BIT|8-bit Support for Embedded Device
 * |        |          |This bit indicates whether the Host Controller is capable of using an 8-bit bus width mode
 * |        |          |This bit is not effective when the Slot Type is set to 10b.
 * |        |          |0x0 (FALSE) = 8-bit Bus Width not Supported
 * |        |          |0x1 (TRUE) = 8-bit Bus Width Supported
 * |[19]    |ADMA2_SUPPORT|ADMA2 Support
 * |        |          |This bit indicates whether the Host Controller is capable of using ADMA2.
 * |        |          |0x0 (FALSE) = ADMA2 not Supported
 * |        |          |0x1 (TRUE) = ADMA2 Supported
 * |[20]    |RSVD_20   |This bit of the CAPABILITIES1_R is a reserved and always returns 0.
 * |[21]    |HIGH_SPEED_SUPPORT|High Speed Support
 * |        |          |This bit indicates whether the Host Controller and the Host System supports High Speed mode and they can supply the SD Clock frequency from 25 MHz to 50 MHz.
 * |        |          |0x0 (FALSE) = High Speed not Supported
 * |        |          |0x1 (TRUE) = High Speed Supported
 * |[22]    |SDMA_SUPPORT|SDMA Support
 * |        |          |This bit indicates whether the Host Controller is capable of using SDMA to transfer data between the system memory and the Host Controller directly.
 * |        |          |0x0 (FALSE) = SDMA not Supported
 * |        |          |0x1 (TRUE) = SDMA Supported
 * |[23]    |SUS_RES_SUPPORT|Suspense/Resume Support
 * |        |          |This bit indicates whether the Host Controller supports Suspend/Resume functionality
 * |        |          |If this bit is 0, the Host Driver does not issue either Suspend or Resume commands because the Suspend and Resume mechanism is not supported.
 * |        |          |0x0 (FALSE) = Not Supported
 * |        |          |0x1 (TRUE) = Supported
 * |[24]    |VOLT_33   |Voltage Support for 3.3V
 * |        |          |0x0 (FALSE) = 3.3V Not Supported
 * |        |          |0x1 (TRUE) = 3.3V Supported
 * |[25]    |VOLT_30   |Voltage Support for SD 3.0V or Embedded 1.2V
 * |        |          |0x0 (FALSE) = SD 3.0V or Embedded 1.2V Not Supported
 * |        |          |0x1 (TRUE) = SD 3.0V or Embedded Supported
 * |[26]    |VOLT_18   |Voltage Support for 1.8V
 * |        |          |0x0 (FALSE) = 1.8V Not Supported
 * |        |          |0x1 (TRUE) = 1.8V Supported
 * |[27]    |SYS_ADDR_64_V4|64-bit System Address Support for V4
 * |        |          |This bit sets the Host Controller to support 64-bit System Addressing of V4 mode
 * |        |          |When this bit is set to 1, full or part of 64-bit address must be used to decode the Host Controller Registers so that Host Controller Registers can be placed above system memory area
 * |        |          |64-bit address decode of Host Controller registers is effective regardless of setting to 64-bit Addressing in Host Control 2.
 * |        |          |If this bit is set to 1, 64-bit DMA Addressing for version 4 is enabled by setting Host Version 4 Enable (HOST_VER4_ENABLE = 1) and by setting 64-bit Addressing (ADDRESSING =1) in the Host Control 2 register
 * |        |          |SDMA can be used and ADMA2 uses 128-bit Descriptor.
 * |        |          |0x0 (FALSE) = 64-bit System Address for V4 is Not Supported
 * |        |          |0x1 (TRUE) = 64-bit System Address for V4 is Supported
 * |[28]    |SYS_ADDR_64_V3|64-bit System Address Support for V3
 * |        |          |This bit sets the Host controller to support 64-bit System Addressing of V3 mode.
 * |        |          |SDMA cannot be used in 64-bit Addressing in Version 3 Mode.
 * |        |          |If this bit is set to 1, 64-bit ADMA2 with using 96-bit Descriptor can be enabled by setting Host Version 4 Enable (HOST_VER4_ENABLE = 0) and DMA select (DMA_SEL = 11b).
 * |        |          |0x0 (FALSE) = 64-bit System Address for V3 is Not Supported
 * |        |          |0x1 (TRUE) = 64-bit System Address for V3 is Supported
 * |[29]    |ASYNC_INT_SUPPORT|Asynchronous Interrupt Support
 * |        |          |0x0 (FALSE) = Asynchronous Interrupt Not Supported
 * |        |          |0x1 (TRUE) = Asynchronous Interrupt Supported
 * |        |          |Note: SD Mode only.
 * |[31:30] |SLOT_TYPE_R|Slot Type
 * |        |          |These bits indicate usage of a slot by a specific Host System.
 * |        |          |0x0 (REMOVABLE_SLOT) = Removable Card Slot
 * |        |          |0x1 (EMBEDDED_SLOT) = Embedded Slot for one Device
 * |        |          |0x2 (SHARED_SLOT) = Shared Bus Slot (SD mode)
 * |        |          |0x3 (UHS2_EMBEDDED_SLOT) = UHS-II Multiple Embedded Devices
 * @var SDH_T::CAPABILITIES2_R
 * Offset: 0x44  Capabilities Register - 32 to 63
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |SDR50_SUPPORT|SDR50 Support
 * |        |          |This bit indicates that SDR50 is supported
 * |        |          |The bit 13 (USE_TUNING_SDR50) indicates whether SDR50 requires tuning or not.
 * |        |          |Note: UHS-I only.
 * |[1]     |SDR104_SUPPORT|SDR104 Support
 * |        |          |This bit mentions that SDR104 requires tuning.
 * |        |          |Note: UHS-I only.
 * |[2]     |DDR50_SUPPORT|DDR50 Support
 * |        |          |Note: UHS-I only.
 * |[3]     |UHS2_SUPPORT|UHS-II Support
 * |        |          |This bit indicates whether Host Controller supports UHS-II.
 * |        |          |Note: UHS-I only.
 * |[4]     |DRV_TYPEA |Driver Type A Support
 * |        |          |This bit indicates support of Driver Type A for 1.8 Signaling.
 * |        |          |Note: UHS-I only.
 * |[5]     |DRV_TYPEC |Driver Type C Support
 * |        |          |This bit indicates support of Driver Type C for 1.8 Signaling.
 * |        |          |Note: UHS-I only.
 * |[6]     |DRV_TYPED |Driver Type D Support
 * |        |          |This bit indicates support of Driver Type D for 1.8 Signaling.
 * |        |          |Note: UHS-I only.
 * |[7]     |RSVD_39   |This bit (RSVD_39) of the CAPABILITIES2_R register is reserved and always returns 0.
 * |[11:8]  |RETUNE_CNT|Timer Count for Re-Tuning
 * |        |          |0x0 = Re-Tuning Timer disabled
 * |        |          |0x1 = 1 seconds
 * |        |          |0x2 = 2 seconds
 * |        |          |0x3 = 4 seconds
 * |        |          |........
 * |        |          |0xB = 1024 seconds
 * |        |          |0xC = Reserved
 * |        |          |0xD = Reserved
 * |        |          |0xE = Reserved
 * |        |          |0xF = Get information from other source
 * |        |          |Note: UHS-I only.
 * |[12]    |RSVD_44   |This bit (RSVD_44) of the CAPABILITIES2_R register is reserved and always returns 0.
 * |[13]    |USE_TUNING_SDR50|Use Tuning for SDR50
 * |        |          |Note: UHS-I only.
 * |[15:14] |RE_TUNING_MODES|Re-Tuning Modes
 * |        |          |These bits select the re-tuning method and limit the maximum data length.
 * |        |          |Note: UHS-I only.
 * |[23:16] |CLK_MUL   |Clock Multiplier
 * |        |          |These bits indicate the clock multiplier of the programmable clock generator
 * |        |          |Setting these bits to 0 means that the Host Controller does not support a programmable clock generator.
 * |        |          |0x0 = Clock Multiplier is not Supported
 * |        |          |0x1 = Clock Multiplier M = 2.
 * |        |          |0x2 = Clock Multiplier M = 3.
 * |        |          |.........
 * |        |          |0xFF = Clock Multiplier M = 256. 
 * |[26:24] |RSVD_56_58|These bits (RSVD_56_58) of the CAPABILITIES2_R register are reserved and always return 0.
 * |[27]    |ADMA3_SUPPORT|ADMA3 Support
 * |        |          |This bit indicates whether the Host Controller is capable of using ADMA3.
 * |[28]    |VDD2_18V_SUPPORT|1.8V VDD2 Support
 * |        |          |This bit indicates support of VDD2 for the Host System.
 * |        |          |0x0 (FALSE) = 1.8V VDD2 is not Supported
 * |        |          |0x1 (TRUE) = 1.8V VDD2 is Supported 
 * |[29]    |RSVD_61   |This bit (RSVD_61) of the CAPABILITIES2_R register is reserved and always returns 0.
 * |[31:30] |RSVD_62_63|These bits (RSVD_62_63) of the CAPABILITIES2_R register are reserved bits and always return 0.
 * @var SDH_T::CURR_CAPABILITIES1_R
 * Offset: 0x48  Maximum Current Capabilities Register - 0 to 31
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |MAX_CUR_33V|Maximum Current for 3.3V
 * |        |          |This bit specifies the Maximum Current for 3.3V VDD1 power supply for the card.
 * |        |          |0 = Get information through another method
 * |        |          |1 = 4mA
 * |        |          |2 = 8mA
 * |        |          |3 = 13mA
 * |        |          |.......
 * |        |          |255 = 1020mA 
 * |[15:8]  |MAX_CUR_30V|Maximum Current for 3.0V
 * |        |          |This bit specifies the Maximum Current for 3.0V VDD1 power supply for the card.
 * |        |          |0 = Get information through another method
 * |        |          |1 = 4mA
 * |        |          |2 = 8mA
 * |        |          |3 = 13mA
 * |        |          |.......
 * |        |          |255 = 1020mA 
 * |[23:16] |MAX_CUR_18V|Maximum Current for 1.8V
 * |        |          |This bit specifies the Maximum Current for 1.8V VDD1 power supply for the card.
 * |        |          |0 = Get information through another method
 * |        |          |1 = 4mA
 * |        |          |2 = 8mA
 * |        |          |3 = 13mA
 * |        |          |.......
 * |        |          |255 = 1020mA 
 * |[31:24] |RSVD_31_24|These bits of the CURR_CAPABILITIES1_R register are reserved and always return 0.
 * @var SDH_T::CURR_CAPABILITIES2_R
 * Offset: 0x4c  Maximum Current Capabilities Register - 32 to 63
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |MAX_CUR_VDD2_18V|Maximum Current for 1.8V VDD2
 * |        |          |This bit specifies the Maximum Current for 1.8V VDD2 power supply for the UHS-II card.
 * |        |          |0 = Get information through another method
 * |        |          |1 = 4mA
 * |        |          |2 = 8mA
 * |        |          |3 = 13mA
 * |        |          |.......
 * |        |          |255 = 1020mA 
 * |[31:8]  |RSVD_63_40|These bits of the CURR_CAPABILITIES2_R register are reserved and always return 0.
 * @var SDH_T::FORCE_AUTO_CMD_STAT_R
 * Offset: 0x50  Force Event Register for Auto CMD Error Status register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |FORCE_AUTO_CMD12_NOT_EXEC|Force Event for Auto CMD12 Not Executed
 * |        |          |0x1 (TRUE) = Auto CMD12 Not Executed Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[1]     |FORCE_AUTO_CMD_TOUT_ERR|Force Event for Auto CMD Timeout Error
 * |        |          |0x1 (TRUE) = Auto CMD Timeout Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[2]     |FORCE_AUTO_CMD_CRC_ERR|Force Event for Auto CMD CRC Error
 * |        |          |0x1 (TRUE) = Auto CMD CRC Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[3]     |FORCE_AUTO_CMD_EBIT_ERR|Force Event for Auto CMD End Bit Error
 * |        |          |0x1 (TRUE) = Auto CMD End Bit Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[4]     |FORCE_AUTO_CMD_IDX_ERR|Force Event for Auto CMD Index Error
 * |        |          |0x1 (TRUE) = Auto CMD Index Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[5]     |FORCE_AUTO_CMD_RESP_ERR|Force Event for Auto CMD Response Error
 * |        |          |0x1 (TRUE) = Auto CMD Response Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[6]     |RSVD_6    |This bit of the FORCE_AUTO_CMD_STAT_R register are reserved and always return 0.
 * |[7]     |FORCE_CMD_NOT_ISSUED_AUTO_CMD12|Force Event for Command Not Issued By Auto CMD12 Error
 * |        |          |0x1 (TRUE) = Command Not Issued By Auto CMD12 Error Status is set
 * |        |          |0x0 (FALSE) = Not Affected 
 * |[15:8]  |RSVD_15_8 |These bits of the FORCE_AUTO_CMD_STAT_R register are reserved and always return 0.
 * @var SDH_T::FORCE_ERROR_INT_STAT_R
 * Offset: 0x52  Force Event Register for Error Interrupt Status
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |FORCE_CMD_TOUT_ERR|Force Event for Command Timeout Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Command Timeout Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[1]     |FORCE_CMD_CRC_ERR|Force Event for Command CRC Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Command CRC Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[2]     |FORCE_CMD_END_BIT_ERR|Force Event for Command End Bit Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Command End Bit Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[3]     |FORCE_CMD_IDX_ERR|Force Event for Command Index Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Command Index Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[4]     |FORCE_DATA_TOUT_ERR|Force Event for Data Timeout Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Data Timeout Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[5]     |FORCE_DATA_CRC_ERR|Force Event for Data CRC Error 
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Data CRC Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[6]     |FORCE_DATA_END_BIT_ERR|Force Event for Data End Bit Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Data End Bit Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[7]     |FORCE_CUR_LMT_ERR|Force Event for Current Limit Error 
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Current Limit Error Status is set 
 * |[8]     |FORCE_AUTO_CMD_ERR|Force Event for Auto CMD Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Auto CMD Error Status is set
 * |        |          |Note: SD/eMMC Mode only.
 * |[9]     |FORCE_ADMA_ERR|Force Event for ADMA Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = ADMA Error Status is set 
 * |[10]    |FORCE_TUNING_ERR|Force Event for Tuning Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Tuning Error Status is set
 * |        |          |Note: UHS-I Mode only.
 * |[11]    |FORCE_RESP_ERR|Force Event for Response Error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Response Error Status is set
 * |        |          |Note: SD Mode only.
 * |[12]    |FORCE_BOOT_ACK_ERR|Force Event for Boot Ack error
 * |        |          |0x0 (FALSE) = Not Affected
 * |        |          |0x1 (TRUE) = Boot ack Error Status is set 
 * |[13]    |FORCE_VENDOR_ERR1|This bit (FORCE_VENDOR_ERR1) of the FORCE_ERROR_INT_STAT_R register is reserved and always returns 0. 
 * |[14]    |FORCE_VENDOR_ERR2|This bit (FORCE_VENDOR_ERR2) of the FORCE_ERROR_INT_STAT_R register is reserved and always returns 0. 
 * |[15]    |FORCE_VENDOR_ERR3|This bit (FORCE_VENDOR_ERR3) of the FORCE_ERROR_INT_STAT_R register is reserved and always returns 0. 
 * @var SDH_T::ADMA_ERR_STAT_R
 * Offset: 0x54  ADMA Error Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[1:0]   |ADMA_ERR_STATES|ADMA Error States
 * |        |          |0x0 (ST_STOP) = Stop DMA - SYS_ADR register points to a location next to the error descriptor
 * |        |          |0x1 (ST_FDS) = Fetch Descriptor - SYS_ADR register points to the error descriptor
 * |        |          |0x2 (UNUSED) = Never set this state
 * |        |          |0x3 (ST_TFR) = Transfer Data - SYS_ADR register points to a location next to the error descriptor
 * |        |          |These bits indicate the state of ADMA when an error occurs during ADMA data transfer.
 * |[2]     |ADMA_LEN_ERR|ADMA Length Mismatch Error States
 * |        |          |0x0 (NO_ERR) = No Error
 * |        |          |0x1 (ERROR) = Error
 * |        |          |This error occurs in the following instances:
 * |        |          |While the Block Count Enable is being set, the total data length specified by the Descriptor table is different from that specified by the Block Count and Block Length
 * |        |          | When the total data length cannot be divided by the block length 
 * |[7:3]   |RSVD_7_3  |These bits of the ADMA_ERR_STAT_R register are reserved and always return 0.
 * @var SDH_T::ADMA_SA_LOW_R
 * Offset: 0x58  ADMA System Address Register - Low
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |ADMA_SA_LOW|ADMA System Address
 * |        |          |These bits indicate the lower 32 bits of the ADMA system address.
 * |        |          |SDMA = If Host Version 4 Enable is set to 1, this register stores the system address of the data location
 * |        |          | ADMA2 = This register stores the byte address of the executing command of the descriptor table
 * |        |          |ADMA3 = This register is set by ADMA3
 * |        |          |ADMA2 increments the address of this register that points to the next line, every time a Descriptor line is fetched.
 * @var SDH_T::PRESET_INIT_R
 * Offset: 0x5c  Preset Value for Initialization
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when the Host Controller supports a programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_INIT_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate that the Driver strength is supported by 1.8V signaling bus speed modes
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_DS_R
 * Offset: 0x62  Preset Value for Default Speed
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_DS_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported by 1.8V signaling bus speed modes
 * |        |          |This field is meaningless for the Default speed mode as it uses 3.3V signaling
 * @var SDH_T::PRESET_HS_R
 * Offset: 0x64  Preset Value for High Speed
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_HS_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported by 1.8V signaling bus speed modes
 * |        |          |This field is meaningless for High speed mode as it uses 3.3V signaling
 * @var SDH_T::PRESET_SDR12_R
 * Offset: 0x66  Preset Value for SDR12
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generato
 * |[13:11] |RSVD_13_11|These bits of the PRESET_SDR12_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported for the SDR12 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_SDR25_R
 * Offset: 0x68  Preset Value for SDR25
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_SDR25_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported for the SDR25 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_SDR50_R
 * Offset: 0x6a  Preset Value for SDR50
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |This bit is effective when Host Controller supports programmable clock generator
 * |[13:11] |RSVD_13_11|These bits of the PRESET_SDR50_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported for the SDR50 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_SDR104_R
 * Offset: 0x6c  Preset Value for SDR104
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_SDR104_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate the Driver strength value supported for the SDR104 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_DDR50_R
 * Offset: 0x6e  Preset Value for DDR50
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |10-bit preset value to be set in SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This bit is effective when Host Controller supports programmable clock generator.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_DDR50_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate Driver strength value supported for DDR50 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::PRESET_UHS2_R
 * Offset: 0x74  Preset Value for UHS-II
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[9:0]   |FREQ_SEL_VAL|SDCLK/RCLK Frequency Select Value
 * |        |          |These bits specify a 10-bit preset value that must be set in the SDCLK/RCLK Frequency Select field of the Clock Control register described by a Host System.
 * |[10]    |CLK_GEN_SEL_VAL|Clock Generator Select Value
 * |        |          |0x0 (FALSE) = Host Controller Ver2.0 Compatible Clock Generator
 * |        |          |0x1 (PROG) = Programmable Clock Generator
 * |        |          |This Host Controller not supports programmable clock generator, please do not set this bit to 0x1.
 * |[13:11] |RSVD_13_11|These bits of the PRESET_UHS2_R register are reserved and always return 0.
 * |[15:14] |DRV_SEL_VAL|Driver Strength Select Value
 * |        |          |0x0 (TYPEB) = Driver Type B is selected
 * |        |          |0x1 (TYPEA) = Driver Type A is selected
 * |        |          |0x2 (TYPEC) = Driver Type C is selected
 * |        |          |0x3 (TYPED) = Driver Type D is selected
 * |        |          |These bits indicate Driver strength value supported for UHS2 bus speed mode
 * |        |          |These bits are meaningless for 3.3V signaling.
 * @var SDH_T::ADMA_ID_LOW_R
 * Offset: 0x78  ADMA3 Integrated Descriptor Address Register - Low
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |ADMA_ID_LOW|ADMA Integrated Descriptor Address
 * |        |          |These bits indicate the lower 32-bit of the ADMA Integrated Descriptor   address
 * |        |          |The start address of Integrated Descriptor is set to these register   bits
 * |        |          |The ADMA3 fetches one Descriptor Address and increments these bits to   indicate the next Descriptor address
 * @var SDH_T::P_EMBEDDED_CNTRL
 * Offset: 0xe6  Pointer for Embedded Control
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[11:0]  |REG_OFFSET_ADDR|Offset   Address of Embedded Control register. 
 * @var SDH_T::P_VENDOR_SPECIFIC_AREA
 * Offset: 0xe8  Pointer for Vendor Specific Area 1
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[11:0]  |REG_OFFSET_ADDR|Base offset Address for Vendor-Specific registers. 
 * @var SDH_T::P_VENDOR2_SPECIFIC_AREA
 * Offset: 0xea  Pointer for Vendor Specific Area 2
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[15:0]  |REG_OFFSET_ADDR|Base offset Address for Command Queuing registers.
 * @var SDH_T::SLOT_INTR_STATUS_R
 * Offset: 0xfc  Slot Interrupt Status Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |INTR_SLOT |Interrupt signal for each Slot
 * |        |          |These status bits indicate the logical OR of Interrupt signal and Wakeup signal for each slot
 * |        |          |A maximum of 8 slots can be defined
 * |        |          |If one interrupt signal is associated with multiple slots, the Host Driver can identify the interrupt that is generated by reading these bits
 * |        |          |By a power on reset or by setting Software Reset For All bit, the interrupt signals are de-asserted and this status reads 00h.
 * |        |          |Bit 00 = Slot 1
 * |        |          |Bit 01 = Slot 2
 * |        |          |Bit 02 = Slot 3
 * |        |          |..........
 * |        |          |..........
 * |        |          |Bit 07 = Slot 8
 * |        |          |Note: MSHC Host Controller support single card slot. This register shall always return 0.
 * @var SDH_T::HOST_CNTRL_VERS_R
 * Offset: 0xfe  Host Controller Version
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |SPEC_VERSION_NUM|Specification Version Number
 * |        |          |0x0 (VER_1_00) = SD Host Controller Specification Version 1.00
 * |        |          |0x1 (VER_2_00) = SD Host Controller Specification Version 2.00
 * |        |          |0x2 (VER_3_00) = SD Host Controller Specification Version 3.00
 * |        |          |0x3 (VER_4_00) = SD Host Controller Specification Version 4.00
 * |        |          |0x4 (VER_4_10) = SD Host Controller Specification Version 4.10
 * |        |          |0x5 (VER_4_20) = SD Host Controller Specification Version 4.20
 * |        |          |These bits indicate the Host controller specification version
 * |        |          |The upper and lower 4-bits indicate the version
 * |        |          |Values 0x06-0xFF are reserved
 * |[15:8]  |VENDOR_VERSION_NUM|Vendor Version Number
 * |        |          |This field is reserved for the vendor version number. Host Driver must not use this status.
 */
   __IO uint32_t SDMASA_R;                  /*!< [0x0000]   SDMA System Address Register                                     */
   __IO uint16_t BLOCKSIZE_R;            	/*!< [0x0004]   Block Size Register                                              */
   __IO uint16_t BLOCKCOUNT_R;           	/*!< [0x0006]   16-bit Block Count Register                                      */
   __IO uint32_t ARGUMENT_R;             	/*!< [0x0008]   Argument Register                                                */
   __IO uint16_t XFER_MODE_R;            	/*!< [0x000c]   Transfer Mode Register                                           */
   __IO uint16_t CMD_R;                  	/*!< [0x000e]   Command Register                                                 */
   __IO uint32_t RESP01_R;               	/*!< [0x0010]   Response Register 0/1                                            */
   __IO uint32_t RESP23_R;               	/*!< [0x0014]   Response Register 2/3                                            */
   __IO uint32_t RESP45_R;               	/*!< [0x0018]   Response Register 4/5                                            */
   __IO uint32_t RESP67_R;               	/*!< [0x001c]   Response Register 6/7                                            */
   __IO uint32_t BUF_DATA_R;             	/*!< [0x0020]   Buffer Data Port Register                                        */
   __IO uint32_t PSTATE_REG;             	/*!< [0x0024]   Present State Register                                           */
   __IO uint8_t HOST_CTRL1_R;            	/*!< [0x0028]   Host Control 1 Register                                          */
   __IO uint8_t PWR_CTRL_R;              	/*!< [0x0029]   Power Control Register                                           */
   __IO uint8_t BGAP_CTRL_R;             	/*!< [0x002a]   Block Gap Control Register                                       */
   __IO uint8_t WUP_CTRL_R;              	/*!< [0x002b]   Wakeup Control Register                                          */
   __IO uint16_t CLK_CTRL_R;             	/*!< [0x002c]   Clock Control Register                                           */
   __IO uint8_t TOUT_CTRL_R;             	/*!< [0x002e]   Timeout Control Register                                         */
   __IO uint8_t SW_RST_R;                	/*!< [0x002f]   Software Reset Register                                          */
   __IO uint16_t NORMAL_INT_STAT_R;      	/*!< [0x0030]   Normal Interrupt Status Register                                 */
   __IO uint16_t ERROR_INT_STAT_R;       	/*!< [0x0032]   Error Interrupt Status Register                                  */
   __IO uint16_t NORMAL_INT_STAT_EN_R;   	/*!< [0x0034]   Normal Interrupt Status Enable Register                          */
   __IO uint16_t ERROR_INT_STAT_EN_R;    	/*!< [0x0036]   Error Interrupt Status Enable Register                           */
   __IO uint16_t NORMAL_INT_SIGNAL_EN_R; 	/*!< [0x0038]   Normal Interrupt Signal Enable Register                          */
   __IO uint16_t ERROR_INT_SIGNAL_EN_R;  	/*!< [0x003a]   Error Interrupt Signal Enable Register                           */
   __I uint16_t AUTO_CMD_STAT_R;         	/*!< [0x003c]   Auto CMD Status Register                                         */
   __IO uint16_t HOST_CTRL2_R;           	/*!< [0x003e]   Host Control 2 Register                                          */
   __I uint16_t CAPABILITIES1_R;         	/*!< [0x0040]   Capabilities 1 Register - 0 to 31                                */
   __I uint16_t CAPABILITIES2_R;         	/*!< [0x0044]   Capabilities Register - 32 to 63                                 */
   __I uint16_t CURR_CAPABILITIES1_R;    	/*!< [0x0048]   Maximum Current Capabilities Register - 0 to 31                  */
   __I uint16_t CURR_CAPABILITIES2_R;    	/*!< [0x004c]   Maximum Current Capabilities Register - 32 to 63                 */
   __IO uint16_t FORCE_AUTO_CMD_STAT_R;  	/*!< [0x0050]   Force Event Register for Auto CMD Error Status register          */
   __IO uint16_t FORCE_ERROR_INT_STAT_R; 	/*!< [0x0052]   Force Event Register for Error Interrupt Status                  */
   __IO uint32_t ADMA_ERR_STAT_R;        	/*!< [0x0054]   ADMA Error Status Register                                       */
   __IO uint32_t ADMA_SA_LOW_R;          	/*!< [0x0058]   ADMA System Address Register - Low                               */
   __IO uint32_t ADMA_SA_HIGH_R;            /*!< [0x005c]   ADMA System Address Register - HIGH                              */
   __IO uint16_t PRESET_INIT_R;          	/*!< [0x0060]   Preset Value for Initialization                                  */
   __IO uint16_t PRESET_DS_R;            	/*!< [0x0062]   Preset Value for Default Speed                                   */
   __IO uint16_t PRESET_HS_R;            	/*!< [0x0064]   Preset Value for High Speed                                      */
   __IO uint16_t PRESET_SDR12_R;         	/*!< [0x0066]   Preset Value for SDR12                                           */
   __IO uint16_t PRESET_SDR25_R;         	/*!< [0x0068]   Preset Value for SDR25                                           */
   __IO uint16_t PRESET_SDR50_R;         	/*!< [0x006a]   Preset Value for SDR50                                           */                                                                   
   __IO uint16_t PRESET_SDR104_R;        	/*!< [0x006c]   Preset Value for SDR104                                          */
   __IO uint16_t PRESET_DDR50_R;         	/*!< [0x006e]   Preset Value for DDR50                                           */
   /// @cond HIDDEN_SYMBOLS
   __I  uint32_t RESERVE0[1];
   /// @endcond //HIDDEN_SYMBOLS
   __IO uint16_t PRESET_UHS2_R;          	/*!< [0x0074]   ADMA3 Integrated Descriptor Address Register - Low               */
   /// @cond HIDDEN_SYMBOLS
   __I  uint16_t RESERVE1[1];
   /// @endcond //HIDDEN_SYMBOLS
   __IO uint32_t ADMA_ID_LOW_R;          	/*!< [0x0078]   Command Queuing Capabilities register                            */
   /// @cond HIDDEN_SYMBOLS
   __I  uint16_t RESERVE2[55];
   /// @endcond //HIDDEN_SYMBOLS
   __I  uint16_t P_EMBEDDED_CNTRL;       	/*!< [0x00e6]   Pointer for Embedded Control                                     */
   __I  uint16_t P_VENDOR_SPECIFIC_AREA; 	/*!< [0x00e8]   Pointer for Vendor Specific Area 1                               */
   __I  uint16_t P_VENDOR2_SPECIFIC_AREA;	/*!< [0x00ea]   Pointer for Vendor Specific Area 2                               */
   /// @cond HIDDEN_SYMBOLS
   __I  uint16_t RESERVE3[9];
   /// @endcond //HIDDEN_SYMBOLS
   __I  uint16_t SLOT_INTR_STATUS_R;     	/*!< [0x00fc]   Slot Interrupt Status Register                                   */
   __I  uint16_t HOST_CNTRL_VERS_R;      	/*!< [0x00fe]   Host Controller Version                                          */        
} SDH_T;

typedef struct
{

/**
 * @var VENDOR_SPECIFIC_AREA_T::MSHC_VER_ID_R
 * Offset: 0x500  MSHC version
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |MSHC_VER_ID|Current release number
 * |        |          |This field indicates the Synopsys DesignWare Cores DWC_mshc/DWC_mshc_lite current release number that is read by an application.
 * |        |          |An application reading this register in conjunction with the MSHC_VER_TYPE_R register, gathers details of the current release.
 * @var VENDOR_SPECIFIC_AREA_T::MSHC_VER_TYPE_R
 * Offset: 0x504  MSHC version type
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[31:0]  |MSHC_VER_TYPE|Current release type
 * |        |          |This field indicates the Synopsys DesignWare Cores DWC_mshc/DWC_mshc_lite current release type that is read by an application.
 * |        |          |An application reading this register in conjunction with the MSHC_VER_ID_R register, gathers details of the current release.
 * @var VENDOR_SPECIFIC_AREA_T::MSHC_CTRL_R
 * Offset: 0x508  MSHC Control register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CMD_CONFLICT_CHECK|Command conflict check
 * |        |          |0x0 (DISABLE_CMD_CONFLICT_CHK) = Disable command conflict check
 * |        |          |0x1 (CMD_CONFLICT_CHK_LAT1) = Check for command conflict after 1 card clock cycle
 * |        |          |This bit enables command conflict check.
 * |        |          |Note: DWC_mshc controller monitors the CMD line whenever a command is issued and checks whether the value driven on sd_cmd_out matches the value on sd_cmd_in at next subsequent edge of cclk_tx to determine command conflict error
 * |        |          |This bit is cleared only if the feed back delay (including I/O Pad delay) is more than (t_card_clk_period - t_setup), where t_setup is the setup time of a flop in DWC_mshc
 * |        |          |The I/O pad delay is consistent across CMD and DATA lines, and it is within the value:
 * |        |          |(2*t_card_clk_period - t_setup) 
 * |[3:1]   |RSVD1     |These bits (RSVD1) of the MSHC_CTRL_R register are reserved and always return 0.
 * |[4]     |SW_CG_DIS |Internal clock gating disable control
 * |        |          |0x0 (ENABLE) = Internal clock gates are active and clock gating is controlled internally
 * |        |          |0x1 (DISABLE) = Internal clock gating is disabled, clocks are not gated internally
 * |        |          |This bit must be used to disable IP's internal clock gating when required
 * |        |          |when disabled clocks are not gated
 * |        |          |Clocks to the core (except hclk) must be stopped when programming this bit
 * @var VENDOR_SPECIFIC_AREA_T::MBIU_CTRL_R
 * Offset: 0x510  MBIU Control register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |UNDEFL_INCR_EN|Undefined INCR Burst
 * |        |          |0x0 (FALSE) = Undefined INCR type burst is the least preferred burst on AHB Master I/F
 * |        |          |0x1 (TRUE) = Undefined INCR type burst is the most preferred burst on AHB Master I/F
 * |        |          |Controls generation of undefined length INCR transfer on Master interface.
 * |[1]     |BURST_INCR4_EN|INCR4 Burst
 * |        |          |0x0 (FALSE) = AHB INCR4 burst type is not generated on Master I/F
 * |        |          |0x1 (TRUE) = AHB INCR4 burst type can be generated on Master I/F
 * |        |          |Controls generation of INCR4 transfers on Master interface.
 * |[2]     |BURST_INCR8_EN|INCR8 Burst
 * |        |          |0x0 (FALSE) = AHB INCR8 burst type is not generated on Master I/F
 * |        |          |0x1 (TRUE) = AHB INCR8 burst type can be generated on Master I/F
 * |        |          |Controls generation of INCR8 transfers on Master interface.
 * |[3]     |BURST_INCR16_EN|INCR16 Burst
 * |        |          |0x0 (FALSE) = AHB INCR16 burst type is not generated on Master I/F
 * |        |          |0x1 (TRUE) = AHB INCR16 burst type can be generated on Master I/F
 * |        |          |Controls generation of INCR16 transfers on Master interface.
 * |[7:4]   |RSVD      |Reserved.
 * @var VENDOR_SPECIFIC_AREA_T::EMMC_CTRL_R
 * Offset: 0x52C  eMMC Control register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |CARD_IS_EMMC|eMMC Card present
 * |        |          |0x1 (EMMC_CARD) = Card connected to MSHC is an eMMC card
 * |        |          |0x0 (NON_EMMC_CARD) = Card connected to MSHC is a non-eMMC card
 * |        |          |This bit indicates the type of card connected
 * |        |          |An application program this bit based on the card connected to MSHC
 * |[1]     |DISABLE_DATA_CRC_CHK|Disable Data CRC Check
 * |        |          |0x1 (DISABLE) = DATA CRC check is disabled
 * |        |          |0x0 (ENABLE) = DATA CRC check is enabled
 * |        |          |This bit controls masking of CRC16 error for Card Write in eMMC mode
 * |        |          |This is useful in bus testing (CMD19) for an eMMC device
 * |        |          |In bus testing, an eMMC card does not send CRC status for a block, which may generate CRC error
 * |        |          |This CRC error can be masked using this bit during bus testing.
 * |[2]     |EMMC_RST_N|EMMC Device Reset signal control
 * |        |          |0x1 (RST_DEASSERT) = Reset to eMMC device is deasserted
 * |        |          |0x0 (RST_ASSERT) = Reset to eMMC device asserted (active low)
 * |        |          |This register field controls the sd_rst_n output of DWC_mshc 
 * |[3]     |EMMC_RST_N_OE|Output Enable control for EMMC Device Reset signal PAD control
 * |        |          |0x1 (ENABLE) = sd_rst_n_oe is 1
 * |        |          |0x0 (DISABLE) = sd_rst_n_oe is 0
 * |        |          |This field drived sd_rst_n_oe output of DWC_mshc 
 * |[15:11] |RSVD      |These bits (RSVD) of the EMMC_CTRL_R register are reserved and always return 0.
 * @var VENDOR_SPECIFIC_AREA_T::BOOT_CTRL_R
 * Offset: 0x530  eMMC Boot Control register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |MAN_BOOT_EN|Mandatory Boot Enable
 * |        |          |This bit is used to initiate the mandatory boot operation
 * |        |          |The application sets this bit along with VALIDATE_BOOT bit
 * |        |          |Writing 0 is ignored
 * |        |          |The DWC_mshc clears this bit after the boot transfer is completed or terminated.
 * |        |          |0x1 (MAN_BOOT_EN) = Mandatory boot enable
 * |        |          |0x0 (MAN_BOOT_DIS) = Mandatory boot disable
 * |        |          |Testable = read only
 * |[6:1]   |RSVD_6_1  |These bits (RSVD _6_1) of the BOOT_CTRL_R register are reserved and always return 0.
 * |[7]     |VALIDATE_BOOT|Validate Mandatory Boot Enable bit
 * |        |          |0x1 (TRUE) = Validate Mandatory boot enable bit
 * |        |          |0x0 (FALSE) = Ignore Mandatory boot Enable bit
 * |        |          |This bit is used to validate the MAN_BOOT_EN bit.
 * |[8]     |BOOT_ACK_ENABLE|Boot Acknowledge Enable
 * |        |          |0x1 (TRUE) = Boot Ack enable
 * |        |          |0x0 (FALSE) = Boot Ack disable
 * |        |          |When this bit set, DWC_mshc checks for boot acknowledge start pattern of 0-1-0 during boot operation
 * |        |          |This bit is applicable for both mandatory and alternate boot mode.
 * |[11:9]  |RSVD_11_9 |These bits (RSVD_11_9) of the BOOT_CTRL_R register are reserved and always return 0.
 * |[15:12] |BOOT_TOUT_CNT|Boot Ack Timeout Counter Value
 * |        |          |This value determines the interval by which boot ack timeout (50 ms) is detected when boot ack is expected during boot operation.
 * |        |          |0xF = Reserved
 * |        |          |0xE = TMCLK x 2^27
 * |        |          |.. - ............
 * |        |          |0x1 = TMCLK x 2^14
 * |        |          |0x0 = TMCLK x 2^13 
 * @var VENDOR_SPECIFIC_AREA_T::AT_CTRL_R
 * Offset: 0x540  Tuning and Auto-tuning control register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[0]     |AT_EN     |Setting this bit enables Auto tuning engine This bit is enabled by default when core is configured with mode3 retuning support Clear this bit to 0 when core is configured to have Mode3 re-tuning but SW wishes to disable mode3 re-tuning
 * |        |          |This field should be programmed only when CLK_CTRL_R.SD_CLK_EN is 0.
 * |        |          |0x1 (AT_ENABLE) = AutoTuning is enabled
 * |        |          |0x0 (AT_DISABLE) = AutoTuning is disabled 
 * |[1]     |CI_SEL    |Select the interval when the corrected center phase select code can be driven on tuning_cclk_sel output.
 * |        |          |0x0 (WHEN_IN_BLK_GAP) = Driven in block gap interval
 * |        |          |0x1 (WHEN_IN_IDLE) = Driven at the end of the transfer 
 * |[2]     |SWIN_TH_EN|Sampling window Threshold enable
 * |        |          |Selects the tuning mode
 * |        |          |Field should be programmed only when SAMPLE_CLK_SEL is '0'
 * |        |          |0x1 (THRESHOLD_MODE) = Tuning engine selects the first complete sampling window that meets the threshold set by SWIN_TH_VAL field
 * |        |          |0x0 (LARGEST_WIN_MODE) = Tuning engine sweeps all taps and settles at the largest window 
 * |[3]     |RPT_TUNE_ERR|Framing errors are not generated when executing tuning. This debug bit allows users to report these errors.
 * |        |          |0x1 (DEBUG_ERRORS) = Debug mode for reporting framing errors
 * |        |          |0x0 (ERRORS_DISABLED) = Default mode where as per SD-HCI no errors are reported. 
 * |[4]     |SW_TUNE_EN|Enable software-managed tuning flow
 * |        |          |0x1 (SW_TUNING_ENABLE) = Software-managed tuning enabled
 * |        |          |AT_STAT_R.CENTER_PH_CODE Field is now writable.
 * |        |          |0x0 (SW_TUNING_DISABLE) = Software-managed tuning disabled. 
 * |[7:5]   |RSDV2     |These bits (RSVD2) of the AT_CTRL_R register are reserved and always return 0.
 * |[11:8]  |WIN_EDGE_SEL|This field sets the phase for Left and Right edges for drift monitoring [Left edge offset + Right edge offset] must not be less than total taps of delayLine.
 * |        |          |0x0 = User selection disabled. Tuning calculated edges are used.
 * |        |          |0x1 = Right edge Phase is center + 2 stages, Left edge Phase is center - 2 stages.
 * |        |          |0x2 = Right edge Phase is center + 3 stages, Left edge Phase is center - 3 stages.
 * |        |          |...
 * |        |          |0xF = Right edge Phase is center + 16 stages, Left edge Phase is center - 16 stages. 
 * |[15:12] |RSDV3     |These bits (RSVD3) of the AT_CTRL_R register are reserved and always return 0.
 * |[16]    |TUNE_CLK_STOP_EN|Clock stopping control for Tuning and auto-tuning circuit
 * |        |          |When enabled, clock gate control output of DWC_mshc (clk2card_on) is pulled low before changing phase select codes on tuning_cclk_sel and autotuning_cclk_sel
 * |        |          |This effectively stops the Device/Card clock, cclk_rx and also drift_cclk_rx
 * |        |          |Changing phase code when clocks are stopped ensures glitch free phase switching
 * |        |          |Set this bit to 0 if the PHY or delayline can guarantee glitch free switching.
 * |        |          |0x1 (ENABLE_CLK_STOPPING) = Clocks stopped during phase code change
 * |        |          |0x0 (DISABLE_CLK_STOPPING) = Clocks not stopped. PHY ensures glitch free phase switching. 
 * |[18:17] |PRE_CHANGE_DLY|Maximum Latency specification between cclk_tx and cclk_rx
 * |        |          |0x0 (LATENCY_LT_1) = Less than 1-cycle latency
 * |        |          |0x1 (LATENCY_LT_2) = Less than 2-cycle latency
 * |        |          |0x2 (LATENCY_LT_3) = Less than 3-cycle latency
 * |        |          |0x3 (LATENCY_LT_4) = Less than 4-cycle latency
 * |[20:19] |POST_CHANGE_DLY|Time taken for phase switching and stable clock output
 * |        |          |Specifies the maximum time (in terms of cclk cycles) that the delay line can take to switch its output phase after a change in tuning_cclk_sel or autotuning_cclk_sel.
 * |        |          |0x0 (LATENCY_LT_1) = Less than 1-cycle latency
 * |        |          |0x1 (LATENCY_LT_2) = Less than 2-cycle latency
 * |        |          |0x2 (LATENCY_LT_3) = Less than 3-cycle latency
 * |        |          |0x3 (LATENCY_LT_4) = Less than 4-cycle latency 
 * |[27:24] |SWIN_TH_VAL|Sampling window threshold value setting
 * |        |          |The maximum value that can be set here depends on the length of delayline used for tuning
 * |        |          |A delayLine with 128 taps can use values from 0x0 to 0x7F.
 * |        |          |This field is valid only when SWIN_TH_EN is '1'. Should be programmed only when SAMPLE_CLK_SEL is '0'
 * |        |          |0x0 = Threshold values is 0x1, windows of length 1 tap and above can be selected as sampling window.
 * |        |          |0x1 = Threshold values is 0x2, windows of length 2 taps and above can be selected as sampling window.
 * |        |          |0x2 = Threshold values is 0x1, windows of length 3 taps and above can be selected as sampling window.
 * |        |          |........
 * |        |          |0x7F = Threshold values is 0x1, windows of length 127 taps and above can be selected as sampling window
 * @var VENDOR_SPECIFIC_AREA_T::AT_STAT_R
 * Offset: 0x544  Tuning and Auto-tuning status register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field     |Descriptions
 * | :----: | :----:   | :---- |
 * |[7:0]   |CENTER_PH_CODE|Centered Phase code
 * |        |          |Reading this field returns the current value on tuning_cclk_sel output Setting AT_CTRL_R.SW_TUNE_EN enables software to write to this field and its contents are reflected on tuning_cclk_sel.
 * |        |          |Testable = read only
 * |[15:8]  |R_EDGE_PH_CODE|Right Edge Phase code
 * |        |          |Reading this field returns the phase code value used by Auto-tuning engine to sample data on Right edge of sampling window
 * |[23:16] |L_EDGE_PH_CODE|Left Edge Phase code
 * |        |          |Reading this field returns the phase code value used by Auto-tuning engine to sample data on Left edge of sampling window
 * |[31:24] |RSDV1     |These bits of the AT_STAT_R register are reserved and always return 0.
 */
    __IO uint32_t MSHC_VER_ID_R;
    __IO uint32_t MSHC_VER_TYPE_R;
    __IO uint8_t MSHC_CTRL_R;
    /// @cond HIDDEN_SYMBOLS
    __I  uint8_t RESERVE0[5];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint8_t MBIU_CTRL_R;
    /// @cond HIDDEN_SYMBOLS
    __I  uint8_t RESERVE1[28];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint16_t EMMC_CTRL_R;
    __IO uint16_t BOOT_CTRL_R;
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[4];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t AT_CTRL_R;
    __IO uint32_t AT_STAT_R;
} VENDOR_SPECIFIC_AREA_T;

/**
    @addtogroup SDH_CONST SDH Bit Field Definition
    Constant Definitions for SDH Controller
@{ */

#define SDH_SDMASA_R_BLOCKCNT_SDMASA_Pos (0)                                               /*!< SDH_T::SDMASA_R: BLOCKCNT_SDMASA Position*/
#define SDH_SDMASA_R_BLOCKCNT_SDMASA_Msk (0xfffffffful << SDH_SDMASA_R_BLOCKCNT_SDMASA_Pos) /*!< SDH_T::SDMASA_R: BLOCKCNT_SDMASA Mask  */

#define SDH_BLOCKSIZE_R_XFER_BLOCK_SIZE_Pos (0)                                            /*!< SDH_T::BLOCKSIZE_R: XFER_BLOCK_SIZE Position*/
#define SDH_BLOCKSIZE_R_XFER_BLOCK_SIZE_Msk (0xffful << SDH_BLOCKSIZE_R_XFER_BLOCK_SIZE_Pos) /*!< SDH_T::BLOCKSIZE_R: XFER_BLOCK_SIZE Mask*/

#define SDH_BLOCKSIZE_R_SDMA_BUF_BDARY_Pos (12)                                            /*!< SDH_T::BLOCKSIZE_R: SDMA_BUF_BDARY Position*/
#define SDH_BLOCKSIZE_R_SDMA_BUF_BDARY_Msk (0x7ul << SDH_BLOCKSIZE_R_SDMA_BUF_BDARY_Pos)   /*!< SDH_T::BLOCKSIZE_R: SDMA_BUF_BDARY Mask*/

#define SDH_BLOCKSIZE_R_RSVD_BLOCKSIZE15_Pos (15)                                          /*!< SDH_T::BLOCKSIZE_R: RSVD_BLOCKSIZE15 Position*/
#define SDH_BLOCKSIZE_R_RSVD_BLOCKSIZE15_Msk (0x1ul << SDH_BLOCKSIZE_R_RSVD_BLOCKSIZE15_Pos) /*!< SDH_T::BLOCKSIZE_R: RSVD_BLOCKSIZE15 Mask*/

#define SDH_BLOCKCOUNT_R_BLOCK_CNT_Pos   (0)                                               /*!< SDH_T::BLOCKCOUNT_R: BLOCK_CNT Position*/
#define SDH_BLOCKCOUNT_R_BLOCK_CNT_Msk   (0xfffful << SDH_BLOCKCOUNT_R_BLOCK_CNT_Pos)      /*!< SDH_T::BLOCKCOUNT_R: BLOCK_CNT Mask    */

#define SDH_ARGUMENT_R_ARGUMENT_Pos      (0)                                               /*!< SDH_T::ARGUMENT_R: ARGUMENT Position   */
#define SDH_ARGUMENT_R_ARGUMENT_Msk      (0xfffffffful << SDH_ARGUMENT_R_ARGUMENT_Pos)     /*!< SDH_T::ARGUMENT_R: ARGUMENT Mask       */

#define SDH_XFER_MODE_R_DMA_ENABLE_Pos   (0)                                               /*!< SDH_T::XFER_MODE_R: DMA_ENABLE Position*/
#define SDH_XFER_MODE_R_DMA_ENABLE_Msk   (0x1ul << SDH_XFER_MODE_R_DMA_ENABLE_Pos)         /*!< SDH_T::XFER_MODE_R: DMA_ENABLE Mask    */

#define SDH_XFER_MODE_R_BLOCK_COUNT_ENABLE_Pos (1)                                         /*!< SDH_T::XFER_MODE_R: BLOCK_COUNT_ENABLE Position*/
#define SDH_XFER_MODE_R_BLOCK_COUNT_ENABLE_Msk (0x1ul << SDH_XFER_MODE_R_BLOCK_COUNT_ENABLE_Pos) /*!< SDH_T::XFER_MODE_R: BLOCK_COUNT_ENABLE Mask*/

#define SDH_XFER_MODE_R_AUTO_CMD_ENABLE_Pos (2)                                            /*!< SDH_T::XFER_MODE_R: AUTO_CMD_ENABLE Position*/
#define SDH_XFER_MODE_R_AUTO_CMD_ENABLE_Msk (0x3ul << SDH_XFER_MODE_R_AUTO_CMD_ENABLE_Pos) /*!< SDH_T::XFER_MODE_R: AUTO_CMD_ENABLE Mask*/

#define SDH_XFER_MODE_R_DATA_XFER_DIR_Pos (4)                                              /*!< SDH_T::XFER_MODE_R: DATA_XFER_DIR Position*/
#define SDH_XFER_MODE_R_DATA_XFER_DIR_Msk (0x1ul << SDH_XFER_MODE_R_DATA_XFER_DIR_Pos)     /*!< SDH_T::XFER_MODE_R: DATA_XFER_DIR Mask */

#define SDH_XFER_MODE_R_MULTI_BLK_SEL_Pos (5)                                              /*!< SDH_T::XFER_MODE_R: MULTI_BLK_SEL Position*/
#define SDH_XFER_MODE_R_MULTI_BLK_SEL_Msk (0x1ul << SDH_XFER_MODE_R_MULTI_BLK_SEL_Pos)     /*!< SDH_T::XFER_MODE_R: MULTI_BLK_SEL Mask */

#define SDH_XFER_MODE_R_RESP_TYPE_Pos    (6)                                               /*!< SDH_T::XFER_MODE_R: RESP_TYPE Position */
#define SDH_XFER_MODE_R_RESP_TYPE_Msk    (0x1ul << SDH_XFER_MODE_R_RESP_TYPE_Pos)          /*!< SDH_T::XFER_MODE_R: RESP_TYPE Mask     */

#define SDH_XFER_MODE_R_RESP_ERR_CHK_ENABLE_Pos (7)                                        /*!< SDH_T::XFER_MODE_R: RESP_ERR_CHK_ENABLE Position*/
#define SDH_XFER_MODE_R_RESP_ERR_CHK_ENABLE_Msk (0x1ul << SDH_XFER_MODE_R_RESP_ERR_CHK_ENABLE_Pos) /*!< SDH_T::XFER_MODE_R: RESP_ERR_CHK_ENABLE Mask*/

#define SDH_XFER_MODE_R_RESP_INT_DISABLE_Pos (8)                                           /*!< SDH_T::XFER_MODE_R: RESP_INT_DISABLE Position*/
#define SDH_XFER_MODE_R_RESP_INT_DISABLE_Msk (0x1ul << SDH_XFER_MODE_R_RESP_INT_DISABLE_Pos) /*!< SDH_T::XFER_MODE_R: RESP_INT_DISABLE Mask*/

#define SDH_XFER_MODE_R_RSVD_Pos         (9)                                               /*!< SDH_T::XFER_MODE_R: RSVD Position      */
#define SDH_XFER_MODE_R_RSVD_Msk         (0x7ful << SDH_XFER_MODE_R_RSVD_Pos)              /*!< SDH_T::XFER_MODE_R: RSVD Mask          */

#define SDH_CMD_R_RESP_TYPE_SELECT_Pos   (0)                                               /*!< SDH_T::CMD_R: RESP_TYPE_SELECT Position*/
#define SDH_CMD_R_RESP_TYPE_SELECT_Msk   (0x3ul << SDH_CMD_R_RESP_TYPE_SELECT_Pos)         /*!< SDH_T::CMD_R: RESP_TYPE_SELECT Mask    */

#define SDH_CMD_R_SUB_CMD_FLAG_Pos       (2)                                               /*!< SDH_T::CMD_R: SUB_CMD_FLAG Position    */
#define SDH_CMD_R_SUB_CMD_FLAG_Msk       (0x1ul << SDH_CMD_R_SUB_CMD_FLAG_Pos)             /*!< SDH_T::CMD_R: SUB_CMD_FLAG Mask        */

#define SDH_CMD_R_CMD_CRC_CHK_ENABLE_Pos (3)                                               /*!< SDH_T::CMD_R: CMD_CRC_CHK_ENABLE Position*/
#define SDH_CMD_R_CMD_CRC_CHK_ENABLE_Msk (0x1ul << SDH_CMD_R_CMD_CRC_CHK_ENABLE_Pos)       /*!< SDH_T::CMD_R: CMD_CRC_CHK_ENABLE Mask  */

#define SDH_CMD_R_CMD_IDX_CHK_ENABLE_Pos (4)                                               /*!< SDH_T::CMD_R: CMD_IDX_CHK_ENABLE Position*/
#define SDH_CMD_R_CMD_IDX_CHK_ENABLE_Msk (0x1ul << SDH_CMD_R_CMD_IDX_CHK_ENABLE_Pos)       /*!< SDH_T::CMD_R: CMD_IDX_CHK_ENABLE Mask  */

#define SDH_CMD_R_DATA_PRESENT_SEL_Pos   (5)                                               /*!< SDH_T::CMD_R: DATA_PRESENT_SEL Position*/
#define SDH_CMD_R_DATA_PRESENT_SEL_Msk   (0x1ul << SDH_CMD_R_DATA_PRESENT_SEL_Pos)         /*!< SDH_T::CMD_R: DATA_PRESENT_SEL Mask    */

#define SDH_CMD_R_CMD_TYPE_Pos           (6)                                               /*!< SDH_T::CMD_R: CMD_TYPE Position        */
#define SDH_CMD_R_CMD_TYPE_Msk           (0x3ul << SDH_CMD_R_CMD_TYPE_Pos)                 /*!< SDH_T::CMD_R: CMD_TYPE Mask            */

#define SDH_CMD_R_CMD_INDEX_Pos          (8)                                               /*!< SDH_T::CMD_R: CMD_INDEX Position       */
#define SDH_CMD_R_CMD_INDEX_Msk          (0x3ful << SDH_CMD_R_CMD_INDEX_Pos)               /*!< SDH_T::CMD_R: CMD_INDEX Mask           */

#define SDH_CMD_R_RSVD_Pos               (14)                                              /*!< SDH_T::CMD_R: RSVD Position            */
#define SDH_CMD_R_RSVD_Msk               (0x3ul << SDH_CMD_R_RSVD_Pos)                     /*!< SDH_T::CMD_R: RSVD Mask                */

#define SDH_RESP01_R_RESP01_Pos          (0)                                               /*!< SDH_T::RESP01_R: RESP01 Position       */
#define SDH_RESP01_R_RESP01_Msk          (0xfffffffful << SDH_RESP01_R_RESP01_Pos)         /*!< SDH_T::RESP01_R: RESP01 Mask           */

#define SDH_RESP23_R_RESP23_Pos          (0)                                               /*!< SDH_T::RESP23_R: RESP23 Position       */
#define SDH_RESP23_R_RESP23_Msk          (0xfffffffful << SDH_RESP23_R_RESP23_Pos)         /*!< SDH_T::RESP23_R: RESP23 Mask           */

#define SDH_RESP45_R_RESP45_Pos          (0)                                               /*!< SDH_T::RESP45_R: RESP45 Position       */
#define SDH_RESP45_R_RESP45_Msk          (0xfffffffful << SDH_RESP45_R_RESP45_Pos)         /*!< SDH_T::RESP45_R: RESP45 Mask           */

#define SDH_RESP67_R_RESP67_Pos          (0)                                               /*!< SDH_T::RESP67_R: RESP67 Position       */
#define SDH_RESP67_R_RESP67_Msk          (0xfffffffful << SDH_RESP67_R_RESP67_Pos)         /*!< SDH_T::RESP67_R: RESP67 Mask           */

#define SDH_BUF_DATA_R_BUF_DATA_Pos      (0)                                               /*!< SDH_T::BUF_DATA_R: BUF_DATA Position   */
#define SDH_BUF_DATA_R_BUF_DATA_Msk      (0xfffffffful << SDH_BUF_DATA_R_BUF_DATA_Pos)     /*!< SDH_T::BUF_DATA_R: BUF_DATA Mask       */

#define SDH_PSTATE_REG_CMD_INHIBIT_Pos   (0)                                               /*!< SDH_T::PSTATE_REG: CMD_INHIBIT Position*/
#define SDH_PSTATE_REG_CMD_INHIBIT_Msk   (0x1ul << SDH_PSTATE_REG_CMD_INHIBIT_Pos)         /*!< SDH_T::PSTATE_REG: CMD_INHIBIT Mask    */

#define SDH_PSTATE_REG_CMD_INHIBIT_DAT_Pos (1)                                             /*!< SDH_T::PSTATE_REG: CMD_INHIBIT_DAT Position*/
#define SDH_PSTATE_REG_CMD_INHIBIT_DAT_Msk (0x1ul << SDH_PSTATE_REG_CMD_INHIBIT_DAT_Pos)   /*!< SDH_T::PSTATE_REG: CMD_INHIBIT_DAT Mask*/

#define SDH_PSTATE_REG_DAT_LINE_ACTIVE_Pos (2)                                             /*!< SDH_T::PSTATE_REG: DAT_LINE_ACTIVE Position*/
#define SDH_PSTATE_REG_DAT_LINE_ACTIVE_Msk (0x1ul << SDH_PSTATE_REG_DAT_LINE_ACTIVE_Pos)   /*!< SDH_T::PSTATE_REG: DAT_LINE_ACTIVE Mask*/

#define SDH_PSTATE_REG_RE_TUNE_REQ_Pos   (3)                                               /*!< SDH_T::PSTATE_REG: RE_TUNE_REQ Position*/
#define SDH_PSTATE_REG_RE_TUNE_REQ_Msk   (0x1ul << SDH_PSTATE_REG_RE_TUNE_REQ_Pos)         /*!< SDH_T::PSTATE_REG: RE_TUNE_REQ Mask    */

#define SDH_PSTATE_REG_DAT_7_4_Pos       (4)                                               /*!< SDH_T::PSTATE_REG: DAT_7_4 Position    */
#define SDH_PSTATE_REG_DAT_7_4_Msk       (0xful << SDH_PSTATE_REG_DAT_7_4_Pos)             /*!< SDH_T::PSTATE_REG: DAT_7_4 Mask        */

#define SDH_PSTATE_REG_WR_XFER_ACTIVE_Pos (8)                                              /*!< SDH_T::PSTATE_REG: WR_XFER_ACTIVE Position*/
#define SDH_PSTATE_REG_WR_XFER_ACTIVE_Msk (0x1ul << SDH_PSTATE_REG_WR_XFER_ACTIVE_Pos)     /*!< SDH_T::PSTATE_REG: WR_XFER_ACTIVE Mask */

#define SDH_PSTATE_REG_RD_XFER_ACTIVE_Pos (9)                                              /*!< SDH_T::PSTATE_REG: RD_XFER_ACTIVE Position*/
#define SDH_PSTATE_REG_RD_XFER_ACTIVE_Msk (0x1ul << SDH_PSTATE_REG_RD_XFER_ACTIVE_Pos)     /*!< SDH_T::PSTATE_REG: RD_XFER_ACTIVE Mask */

#define SDH_PSTATE_REG_BUF_WR_ENABLE_Pos (10)                                              /*!< SDH_T::PSTATE_REG: BUF_WR_ENABLE Position*/
#define SDH_PSTATE_REG_BUF_WR_ENABLE_Msk (0x1ul << SDH_PSTATE_REG_BUF_WR_ENABLE_Pos)       /*!< SDH_T::PSTATE_REG: BUF_WR_ENABLE Mask  */

#define SDH_PSTATE_REG_BUF_RD_ENABLE_Pos (11)                                              /*!< SDH_T::PSTATE_REG: BUF_RD_ENABLE Position*/
#define SDH_PSTATE_REG_BUF_RD_ENABLE_Msk (0x1ul << SDH_PSTATE_REG_BUF_RD_ENABLE_Pos)       /*!< SDH_T::PSTATE_REG: BUF_RD_ENABLE Mask  */

#define SDH_PSTATE_REG_RSVD_15_12_Pos    (12)                                              /*!< SDH_T::PSTATE_REG: RSVD_15_12 Position */
#define SDH_PSTATE_REG_RSVD_15_12_Msk    (0xful << SDH_PSTATE_REG_RSVD_15_12_Pos)          /*!< SDH_T::PSTATE_REG: RSVD_15_12 Mask     */

#define SDH_PSTATE_REG_CARD_INSERTED_Pos (16)                                              /*!< SDH_T::PSTATE_REG: CARD_INSERTED Position*/
#define SDH_PSTATE_REG_CARD_INSERTED_Msk (0x1ul << SDH_PSTATE_REG_CARD_INSERTED_Pos)       /*!< SDH_T::PSTATE_REG: CARD_INSERTED Mask  */

#define SDH_PSTATE_REG_CARD_STABLE_Pos   (17)                                              /*!< SDH_T::PSTATE_REG: CARD_STABLE Position*/
#define SDH_PSTATE_REG_CARD_STABLE_Msk   (0x1ul << SDH_PSTATE_REG_CARD_STABLE_Pos)         /*!< SDH_T::PSTATE_REG: CARD_STABLE Mask    */

#define SDH_PSTATE_REG_CARD_DETECT_PIN_LEVEL_Pos (18)                                      /*!< SDH_T::PSTATE_REG: CARD_DETECT_PIN_LEVEL Position*/
#define SDH_PSTATE_REG_CARD_DETECT_PIN_LEVEL_Msk (0x1ul << SDH_PSTATE_REG_CARD_DETECT_PIN_LEVEL_Pos) /*!< SDH_T::PSTATE_REG: CARD_DETECT_PIN_LEVEL Mask*/

#define SDH_PSTATE_REG_WR_PROTECT_SW_LVL_Pos (19)                                          /*!< SDH_T::PSTATE_REG: WR_PROTECT_SW_LVL Position*/
#define SDH_PSTATE_REG_WR_PROTECT_SW_LVL_Msk (0x1ul << SDH_PSTATE_REG_WR_PROTECT_SW_LVL_Pos) /*!< SDH_T::PSTATE_REG: WR_PROTECT_SW_LVL Mask*/

#define SDH_PSTATE_REG_DAT_3_0_Pos       (20)                                              /*!< SDH_T::PSTATE_REG: DAT_3_0 Position    */
#define SDH_PSTATE_REG_DAT_3_0_Msk       (0xful << SDH_PSTATE_REG_DAT_3_0_Pos)             /*!< SDH_T::PSTATE_REG: DAT_3_0 Mask        */

#define SDH_PSTATE_REG_CMD_LINE_LVL_Pos  (24)                                              /*!< SDH_T::PSTATE_REG: CMD_LINE_LVL Position*/
#define SDH_PSTATE_REG_CMD_LINE_LVL_Msk  (0x1ul << SDH_PSTATE_REG_CMD_LINE_LVL_Pos)        /*!< SDH_T::PSTATE_REG: CMD_LINE_LVL Mask   */

#define SDH_PSTATE_REG_HOST_REG_VOL_Pos  (25)                                              /*!< SDH_T::PSTATE_REG: HOST_REG_VOL Position*/
#define SDH_PSTATE_REG_HOST_REG_VOL_Msk  (0x1ul << SDH_PSTATE_REG_HOST_REG_VOL_Pos)        /*!< SDH_T::PSTATE_REG: HOST_REG_VOL Mask   */

#define SDH_PSTATE_REG_RSVD_26_Pos       (26)                                              /*!< SDH_T::PSTATE_REG: RSVD_26 Position    */
#define SDH_PSTATE_REG_RSVD_26_Msk       (0x1ul << SDH_PSTATE_REG_RSVD_26_Pos)             /*!< SDH_T::PSTATE_REG: RSVD_26 Mask        */

#define SDH_PSTATE_REG_CMD_ISSUE_ERR_Pos (27)                                              /*!< SDH_T::PSTATE_REG: CMD_ISSUE_ERR Position*/
#define SDH_PSTATE_REG_CMD_ISSUE_ERR_Msk (0x1ul << SDH_PSTATE_REG_CMD_ISSUE_ERR_Pos)       /*!< SDH_T::PSTATE_REG: CMD_ISSUE_ERR Mask  */

#define SDH_PSTATE_REG_SUB_CMD_STAT_Pos  (28)                                              /*!< SDH_T::PSTATE_REG: SUB_CMD_STAT Position*/
#define SDH_PSTATE_REG_SUB_CMD_STAT_Msk  (0x1ul << SDH_PSTATE_REG_SUB_CMD_STAT_Pos)        /*!< SDH_T::PSTATE_REG: SUB_CMD_STAT Mask   */

#define SDH_PSTATE_REG_IN_DORMANT_ST_Pos (29)                                              /*!< SDH_T::PSTATE_REG: IN_DORMANT_ST Position*/
#define SDH_PSTATE_REG_IN_DORMANT_ST_Msk (0x1ul << SDH_PSTATE_REG_IN_DORMANT_ST_Pos)       /*!< SDH_T::PSTATE_REG: IN_DORMANT_ST Mask  */

#define SDH_PSTATE_REG_LANE_SYNC_Pos     (30)                                              /*!< SDH_T::PSTATE_REG: LANE_SYNC Position  */
#define SDH_PSTATE_REG_LANE_SYNC_Msk     (0x1ul << SDH_PSTATE_REG_LANE_SYNC_Pos)           /*!< SDH_T::PSTATE_REG: LANE_SYNC Mask      */

#define SDH_PSTATE_REG_UHS2_IF_DETECT_Pos (31)                                             /*!< SDH_T::PSTATE_REG: UHS2_IF_DETECT Position*/
#define SDH_PSTATE_REG_UHS2_IF_DETECT_Msk (0x1ul << SDH_PSTATE_REG_UHS2_IF_DETECT_Pos)     /*!< SDH_T::PSTATE_REG: UHS2_IF_DETECT Mask */

#define SDH_HOST_CTRL1_R_LED_CTRL_Pos    (0)                                               /*!< SDH_T::HOST_CTRL1_R: LED_CTRL Position */
#define SDH_HOST_CTRL1_R_LED_CTRL_Msk    (0x1ul << SDH_HOST_CTRL1_R_LED_CTRL_Pos)          /*!< SDH_T::HOST_CTRL1_R: LED_CTRL Mask     */

#define SDH_HOST_CTRL1_R_DAT_XFER_WIDTH_Pos (1)                                            /*!< SDH_T::HOST_CTRL1_R: DAT_XFER_WIDTH Position*/
#define SDH_HOST_CTRL1_R_DAT_XFER_WIDTH_Msk (0x1ul << SDH_HOST_CTRL1_R_DAT_XFER_WIDTH_Pos) /*!< SDH_T::HOST_CTRL1_R: DAT_XFER_WIDTH Mask*/

#define SDH_HOST_CTRL1_R_HIGH_SPEED_EN_Pos (2)                                             /*!< SDH_T::HOST_CTRL1_R: HIGH_SPEED_EN Position*/
#define SDH_HOST_CTRL1_R_HIGH_SPEED_EN_Msk (0x1ul << SDH_HOST_CTRL1_R_HIGH_SPEED_EN_Pos)   /*!< SDH_T::HOST_CTRL1_R: HIGH_SPEED_EN Mask*/

#define SDH_HOST_CTRL1_R_DMA_SEL_Pos     (3)                                               /*!< SDH_T::HOST_CTRL1_R: DMA_SEL Position  */
#define SDH_HOST_CTRL1_R_DMA_SEL_Msk     (0x3ul << SDH_HOST_CTRL1_R_DMA_SEL_Pos)           /*!< SDH_T::HOST_CTRL1_R: DMA_SEL Mask      */

#define SDH_HOST_CTRL1_R_EXT_DAT_XFER_Pos (5)                                              /*!< SDH_T::HOST_CTRL1_R: EXT_DAT_XFER Position*/
#define SDH_HOST_CTRL1_R_EXT_DAT_XFER_Msk (0x1ul << SDH_HOST_CTRL1_R_EXT_DAT_XFER_Pos)     /*!< SDH_T::HOST_CTRL1_R: EXT_DAT_XFER Mask */

#define SDH_HOST_CTRL1_R_CARD_DETECT_TEST_LVL_Pos (6)                                      /*!< SDH_T::HOST_CTRL1_R: CARD_DETECT_TEST_LVL Position*/
#define SDH_HOST_CTRL1_R_CARD_DETECT_TEST_LVL_Msk (0x1ul << SDH_HOST_CTRL1_R_CARD_DETECT_TEST_LVL_Pos) /*!< SDH_T::HOST_CTRL1_R: CARD_DETECT_TEST_LVL Mask*/

#define SDH_HOST_CTRL1_R_CARD_DETECT_SIG_SEL_Pos (7)                                       /*!< SDH_T::HOST_CTRL1_R: CARD_DETECT_SIG_SEL Position*/
#define SDH_HOST_CTRL1_R_CARD_DETECT_SIG_SEL_Msk (0x1ul << SDH_HOST_CTRL1_R_CARD_DETECT_SIG_SEL_Pos) /*!< SDH_T::HOST_CTRL1_R: CARD_DETECT_SIG_SEL Mask*/

#define SDH_PWR_CTRL_R_SD_BUS_PWR_VDD1_Pos (0)                                             /*!< SDH_T::PWR_CTRL_R: SD_BUS_PWR_VDD1 Position*/
#define SDH_PWR_CTRL_R_SD_BUS_PWR_VDD1_Msk (0x1ul << SDH_PWR_CTRL_R_SD_BUS_PWR_VDD1_Pos)   /*!< SDH_T::PWR_CTRL_R: SD_BUS_PWR_VDD1 Mask*/

#define SDH_PWR_CTRL_R_SD_BUS_VOL_VDD1_Pos (1)                                             /*!< SDH_T::PWR_CTRL_R: SD_BUS_VOL_VDD1 Position*/
#define SDH_PWR_CTRL_R_SD_BUS_VOL_VDD1_Msk (0x7ul << SDH_PWR_CTRL_R_SD_BUS_VOL_VDD1_Pos)   /*!< SDH_T::PWR_CTRL_R: SD_BUS_VOL_VDD1 Mask*/

#define SDH_PWR_CTRL_R_SD_BUS_PWR_VDD2_Pos (4)                                             /*!< SDH_T::PWR_CTRL_R: SD_BUS_PWR_VDD2 Position*/
#define SDH_PWR_CTRL_R_SD_BUS_PWR_VDD2_Msk (0x1ul << SDH_PWR_CTRL_R_SD_BUS_PWR_VDD2_Pos)   /*!< SDH_T::PWR_CTRL_R: SD_BUS_PWR_VDD2 Mask*/

#define SDH_PWR_CTRL_R_SD_BUS_VOL_VDD2_Pos (5)                                             /*!< SDH_T::PWR_CTRL_R: SD_BUS_VOL_VDD2 Position*/
#define SDH_PWR_CTRL_R_SD_BUS_VOL_VDD2_Msk (0x7ul << SDH_PWR_CTRL_R_SD_BUS_VOL_VDD2_Pos)   /*!< SDH_T::PWR_CTRL_R: SD_BUS_VOL_VDD2 Mask*/

#define SDH_BGAP_CTRL_R_STOP_BG_REQ_Pos  (0)                                               /*!< SDH_T::BGAP_CTRL_R: STOP_BG_REQ Position*/
#define SDH_BGAP_CTRL_R_STOP_BG_REQ_Msk  (0x1ul << SDH_BGAP_CTRL_R_STOP_BG_REQ_Pos)        /*!< SDH_T::BGAP_CTRL_R: STOP_BG_REQ Mask   */

#define SDH_BGAP_CTRL_R_CONTINUE_REQ_Pos (1)                                               /*!< SDH_T::BGAP_CTRL_R: CONTINUE_REQ Position*/
#define SDH_BGAP_CTRL_R_CONTINUE_REQ_Msk (0x1ul << SDH_BGAP_CTRL_R_CONTINUE_REQ_Pos)       /*!< SDH_T::BGAP_CTRL_R: CONTINUE_REQ Mask  */

#define SDH_BGAP_CTRL_R_RD_WAIT_CTRL_Pos (2)                                               /*!< SDH_T::BGAP_CTRL_R: RD_WAIT_CTRL Position*/
#define SDH_BGAP_CTRL_R_RD_WAIT_CTRL_Msk (0x1ul << SDH_BGAP_CTRL_R_RD_WAIT_CTRL_Pos)       /*!< SDH_T::BGAP_CTRL_R: RD_WAIT_CTRL Mask  */

#define SDH_BGAP_CTRL_R_INT_AT_BGAP_Pos  (3)                                               /*!< SDH_T::BGAP_CTRL_R: INT_AT_BGAP Position*/
#define SDH_BGAP_CTRL_R_INT_AT_BGAP_Msk  (0x1ul << SDH_BGAP_CTRL_R_INT_AT_BGAP_Pos)        /*!< SDH_T::BGAP_CTRL_R: INT_AT_BGAP Mask   */

#define SDH_BGAP_CTRL_R_RSVD_7_4_Pos     (4)                                               /*!< SDH_T::BGAP_CTRL_R: RSVD_7_4 Position  */
#define SDH_BGAP_CTRL_R_RSVD_7_4_Msk     (0xful << SDH_BGAP_CTRL_R_RSVD_7_4_Pos)           /*!< SDH_T::BGAP_CTRL_R: RSVD_7_4 Mask      */

#define SDH_WUP_CTRL_R_CARD_INT_Pos      (0)                                               /*!< SDH_T::WUP_CTRL_R: CARD_INT Position   */
#define SDH_WUP_CTRL_R_CARD_INT_Msk      (0x1ul << SDH_WUP_CTRL_R_CARD_INT_Pos)            /*!< SDH_T::WUP_CTRL_R: CARD_INT Mask       */

#define SDH_WUP_CTRL_R_CARD_INSERT_Pos   (1)                                               /*!< SDH_T::WUP_CTRL_R: CARD_INSERT Position*/
#define SDH_WUP_CTRL_R_CARD_INSERT_Msk   (0x1ul << SDH_WUP_CTRL_R_CARD_INSERT_Pos)         /*!< SDH_T::WUP_CTRL_R: CARD_INSERT Mask    */

#define SDH_WUP_CTRL_R_CARD_REMOVAL_Pos  (2)                                               /*!< SDH_T::WUP_CTRL_R: CARD_REMOVAL Position*/
#define SDH_WUP_CTRL_R_CARD_REMOVAL_Msk  (0x1ul << SDH_WUP_CTRL_R_CARD_REMOVAL_Pos)        /*!< SDH_T::WUP_CTRL_R: CARD_REMOVAL Mask   */

#define SDH_WUP_CTRL_R_RSVD_7_3_Pos      (3)                                               /*!< SDH_T::WUP_CTRL_R: RSVD_7_3 Position   */
#define SDH_WUP_CTRL_R_RSVD_7_3_Msk      (0x1ful << SDH_WUP_CTRL_R_RSVD_7_3_Pos)           /*!< SDH_T::WUP_CTRL_R: RSVD_7_3 Mask       */

#define SDH_CLK_CTRL_R_INTERNAL_CLK_EN_Pos (0)                                             /*!< SDH_T::CLK_CTRL_R: INTERNAL_CLK_EN Position*/
#define SDH_CLK_CTRL_R_INTERNAL_CLK_EN_Msk (0x1ul << SDH_CLK_CTRL_R_INTERNAL_CLK_EN_Pos)   /*!< SDH_T::CLK_CTRL_R: INTERNAL_CLK_EN Mask*/

#define SDH_CLK_CTRL_R_INTERNAL_CLK_STABLE_Pos (1)                                         /*!< SDH_T::CLK_CTRL_R: INTERNAL_CLK_STABLE Position*/
#define SDH_CLK_CTRL_R_INTERNAL_CLK_STABLE_Msk (0x1ul << SDH_CLK_CTRL_R_INTERNAL_CLK_STABLE_Pos) /*!< SDH_T::CLK_CTRL_R: INTERNAL_CLK_STABLE Mask*/

#define SDH_CLK_CTRL_R_SD_CLK_EN_Pos     (2)                                               /*!< SDH_T::CLK_CTRL_R: SD_CLK_EN Position  */
#define SDH_CLK_CTRL_R_SD_CLK_EN_Msk     (0x1ul << SDH_CLK_CTRL_R_SD_CLK_EN_Pos)           /*!< SDH_T::CLK_CTRL_R: SD_CLK_EN Mask      */

#define SDH_CLK_CTRL_R_PLL_ENABLE_Pos    (3)                                               /*!< SDH_T::CLK_CTRL_R: PLL_ENABLE Position */
#define SDH_CLK_CTRL_R_PLL_ENABLE_Msk    (0x1ul << SDH_CLK_CTRL_R_PLL_ENABLE_Pos)          /*!< SDH_T::CLK_CTRL_R: PLL_ENABLE Mask     */

#define SDH_CLK_CTRL_R_RSVD_4_Pos        (4)                                               /*!< SDH_T::CLK_CTRL_R: RSVD_4 Position     */
#define SDH_CLK_CTRL_R_RSVD_4_Msk        (0x1ul << SDH_CLK_CTRL_R_RSVD_4_Pos)              /*!< SDH_T::CLK_CTRL_R: RSVD_4 Mask         */

#define SDH_CLK_CTRL_R_CLK_GEN_SELECT_Pos (5)                                              /*!< SDH_T::CLK_CTRL_R: CLK_GEN_SELECT Position*/
#define SDH_CLK_CTRL_R_CLK_GEN_SELECT_Msk (0x1ul << SDH_CLK_CTRL_R_CLK_GEN_SELECT_Pos)     /*!< SDH_T::CLK_CTRL_R: CLK_GEN_SELECT Mask */

#define SDH_CLK_CTRL_R_UPPER_FREQ_SEL_Pos (6)                                              /*!< SDH_T::CLK_CTRL_R: UPPER_FREQ_SEL Position*/
#define SDH_CLK_CTRL_R_UPPER_FREQ_SEL_Msk (0x3ul << SDH_CLK_CTRL_R_UPPER_FREQ_SEL_Pos)     /*!< SDH_T::CLK_CTRL_R: UPPER_FREQ_SEL Mask */

#define SDH_CLK_CTRL_R_FREQ_SEL_Pos      (8)                                               /*!< SDH_T::CLK_CTRL_R: FREQ_SEL Position   */
#define SDH_CLK_CTRL_R_FREQ_SEL_Msk      (0xfful << SDH_CLK_CTRL_R_FREQ_SEL_Pos)           /*!< SDH_T::CLK_CTRL_R: FREQ_SEL Mask       */

#define SDH_TOUT_CTRL_R_TOUT_CNT_Pos     (0)                                               /*!< SDH_T::TOUT_CTRL_R: TOUT_CNT Position  */
#define SDH_TOUT_CTRL_R_TOUT_CNT_Msk     (0xful << SDH_TOUT_CTRL_R_TOUT_CNT_Pos)           /*!< SDH_T::TOUT_CTRL_R: TOUT_CNT Mask      */

#define SDH_TOUT_CTRL_R_RSVD_7_4_Pos     (4)                                               /*!< SDH_T::TOUT_CTRL_R: RSVD_7_4 Position  */
#define SDH_TOUT_CTRL_R_RSVD_7_4_Msk     (0xful << SDH_TOUT_CTRL_R_RSVD_7_4_Pos)           /*!< SDH_T::TOUT_CTRL_R: RSVD_7_4 Mask      */

#define SDH_SW_RST_R_SW_RST_ALL_Pos      (0)                                               /*!< SDH_T::SW_RST_R: SW_RST_ALL Position   */
#define SDH_SW_RST_R_SW_RST_ALL_Msk      (0x1ul << SDH_SW_RST_R_SW_RST_ALL_Pos)            /*!< SDH_T::SW_RST_R: SW_RST_ALL Mask       */

#define SDH_SW_RST_R_SW_RST_CMD_Pos      (1)                                               /*!< SDH_T::SW_RST_R: SW_RST_CMD Position   */
#define SDH_SW_RST_R_SW_RST_CMD_Msk      (0x1ul << SDH_SW_RST_R_SW_RST_CMD_Pos)            /*!< SDH_T::SW_RST_R: SW_RST_CMD Mask       */

#define SDH_SW_RST_R_SW_RST_DAT_Pos      (2)                                               /*!< SDH_T::SW_RST_R: SW_RST_DAT Position   */
#define SDH_SW_RST_R_SW_RST_DAT_Msk      (0x1ul << SDH_SW_RST_R_SW_RST_DAT_Pos)            /*!< SDH_T::SW_RST_R: SW_RST_DAT Mask       */

#define SDH_SW_RST_R_RSVD_7_3_Pos        (3)                                               /*!< SDH_T::SW_RST_R: RSVD_7_3 Position     */
#define SDH_SW_RST_R_RSVD_7_3_Msk        (0x1ful << SDH_SW_RST_R_RSVD_7_3_Pos)             /*!< SDH_T::SW_RST_R: RSVD_7_3 Mask         */

#define SDH_NORMAL_INT_STAT_R_CMD_COMPLETE_Pos (0)                                         /*!< SDH_T::NORMAL_INT_STAT_R: CMD_COMPLETE Position*/
#define SDH_NORMAL_INT_STAT_R_CMD_COMPLETE_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_CMD_COMPLETE_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: CMD_COMPLETE Mask*/

#define SDH_NORMAL_INT_STAT_R_XFER_COMPLETE_Pos (1)                                        /*!< SDH_T::NORMAL_INT_STAT_R: XFER_COMPLETE Position*/
#define SDH_NORMAL_INT_STAT_R_XFER_COMPLETE_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_XFER_COMPLETE_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: XFER_COMPLETE Mask*/

#define SDH_NORMAL_INT_STAT_R_BGAP_EVENT_Pos (2)                                           /*!< SDH_T::NORMAL_INT_STAT_R: BGAP_EVENT Position*/
#define SDH_NORMAL_INT_STAT_R_BGAP_EVENT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_BGAP_EVENT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: BGAP_EVENT Mask*/

#define SDH_NORMAL_INT_STAT_R_DMA_INTERRUPT_Pos (3)                                        /*!< SDH_T::NORMAL_INT_STAT_R: DMA_INTERRUPT Position*/
#define SDH_NORMAL_INT_STAT_R_DMA_INTERRUPT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_DMA_INTERRUPT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: DMA_INTERRUPT Mask*/

#define SDH_NORMAL_INT_STAT_R_BUF_WR_READY_Pos (4)                                         /*!< SDH_T::NORMAL_INT_STAT_R: BUF_WR_READY Position*/
#define SDH_NORMAL_INT_STAT_R_BUF_WR_READY_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_BUF_WR_READY_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: BUF_WR_READY Mask*/

#define SDH_NORMAL_INT_STAT_R_BUF_RD_READY_Pos (5)                                         /*!< SDH_T::NORMAL_INT_STAT_R: BUF_RD_READY Position*/
#define SDH_NORMAL_INT_STAT_R_BUF_RD_READY_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_BUF_RD_READY_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: BUF_RD_READY Mask*/

#define SDH_NORMAL_INT_STAT_R_CARD_INSERTION_Pos (6)                                       /*!< SDH_T::NORMAL_INT_STAT_R: CARD_INSERTION Position*/
#define SDH_NORMAL_INT_STAT_R_CARD_INSERTION_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_CARD_INSERTION_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: CARD_INSERTION Mask*/

#define SDH_NORMAL_INT_STAT_R_CARD_REMOVAL_Pos (7)                                         /*!< SDH_T::NORMAL_INT_STAT_R: CARD_REMOVAL Position*/
#define SDH_NORMAL_INT_STAT_R_CARD_REMOVAL_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_CARD_REMOVAL_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: CARD_REMOVAL Mask*/

#define SDH_NORMAL_INT_STAT_R_CARD_INTERRUPT_Pos (8)                                       /*!< SDH_T::NORMAL_INT_STAT_R: CARD_INTERRUPT Position*/
#define SDH_NORMAL_INT_STAT_R_CARD_INTERRUPT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_CARD_INTERRUPT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: CARD_INTERRUPT Mask*/

#define SDH_NORMAL_INT_STAT_R_INT_A_Pos  (9)                                               /*!< SDH_T::NORMAL_INT_STAT_R: INT_A Position*/
#define SDH_NORMAL_INT_STAT_R_INT_A_Msk  (0x1ul << SDH_NORMAL_INT_STAT_R_INT_A_Pos)        /*!< SDH_T::NORMAL_INT_STAT_R: INT_A Mask   */

#define SDH_NORMAL_INT_STAT_R_INT_B_Pos  (10)                                              /*!< SDH_T::NORMAL_INT_STAT_R: INT_B Position*/
#define SDH_NORMAL_INT_STAT_R_INT_B_Msk  (0x1ul << SDH_NORMAL_INT_STAT_R_INT_B_Pos)        /*!< SDH_T::NORMAL_INT_STAT_R: INT_B Mask   */

#define SDH_NORMAL_INT_STAT_R_INT_C_Pos  (11)                                              /*!< SDH_T::NORMAL_INT_STAT_R: INT_C Position*/
#define SDH_NORMAL_INT_STAT_R_INT_C_Msk  (0x1ul << SDH_NORMAL_INT_STAT_R_INT_C_Pos)        /*!< SDH_T::NORMAL_INT_STAT_R: INT_C Mask   */

#define SDH_NORMAL_INT_STAT_R_RE_TUNE_EVENT_Pos (12)                                       /*!< SDH_T::NORMAL_INT_STAT_R: RE_TUNE_EVENT Position*/
#define SDH_NORMAL_INT_STAT_R_RE_TUNE_EVENT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_RE_TUNE_EVENT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: RE_TUNE_EVENT Mask*/

#define SDH_NORMAL_INT_STAT_R_FX_EVENT_Pos (13)                                            /*!< SDH_T::NORMAL_INT_STAT_R: FX_EVENT Position*/
#define SDH_NORMAL_INT_STAT_R_FX_EVENT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_FX_EVENT_Pos)   /*!< SDH_T::NORMAL_INT_STAT_R: FX_EVENT Mask*/

#define SDH_NORMAL_INT_STAT_R_CQE_EVENT_Pos (14)                                           /*!< SDH_T::NORMAL_INT_STAT_R: CQE_EVENT Position*/
#define SDH_NORMAL_INT_STAT_R_CQE_EVENT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_CQE_EVENT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: CQE_EVENT Mask*/

#define SDH_NORMAL_INT_STAT_R_ERR_INTERRUPT_Pos (15)                                       /*!< SDH_T::NORMAL_INT_STAT_R: ERR_INTERRUPT Position*/
#define SDH_NORMAL_INT_STAT_R_ERR_INTERRUPT_Msk (0x1ul << SDH_NORMAL_INT_STAT_R_ERR_INTERRUPT_Pos) /*!< SDH_T::NORMAL_INT_STAT_R: ERR_INTERRUPT Mask*/

#define SDH_ERROR_INT_STAT_R_CMD_TOUT_ERR_Pos (0)                                          /*!< SDH_T::ERROR_INT_STAT_R: CMD_TOUT_ERR Position*/
#define SDH_ERROR_INT_STAT_R_CMD_TOUT_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_CMD_TOUT_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: CMD_TOUT_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_CMD_CRC_ERR_Pos (1)                                           /*!< SDH_T::ERROR_INT_STAT_R: CMD_CRC_ERR Position*/
#define SDH_ERROR_INT_STAT_R_CMD_CRC_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_CMD_CRC_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: CMD_CRC_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_CMD_END_BIT_ERR_Pos (2)                                       /*!< SDH_T::ERROR_INT_STAT_R: CMD_END_BIT_ERR Position*/
#define SDH_ERROR_INT_STAT_R_CMD_END_BIT_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_CMD_END_BIT_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: CMD_END_BIT_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_CMD_IDX_ERR_Pos (3)                                           /*!< SDH_T::ERROR_INT_STAT_R: CMD_IDX_ERR Position*/
#define SDH_ERROR_INT_STAT_R_CMD_IDX_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_CMD_IDX_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: CMD_IDX_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_DATA_TOUT_ERR_Pos (4)                                         /*!< SDH_T::ERROR_INT_STAT_R: DATA_TOUT_ERR Position*/
#define SDH_ERROR_INT_STAT_R_DATA_TOUT_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_DATA_TOUT_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: DATA_TOUT_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_DATA_CRC_ERR_Pos (5)                                          /*!< SDH_T::ERROR_INT_STAT_R: DATA_CRC_ERR Position*/
#define SDH_ERROR_INT_STAT_R_DATA_CRC_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_DATA_CRC_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: DATA_CRC_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_DATA_END_BIT_ERR_Pos (6)                                      /*!< SDH_T::ERROR_INT_STAT_R: DATA_END_BIT_ERR Position*/
#define SDH_ERROR_INT_STAT_R_DATA_END_BIT_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_DATA_END_BIT_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: DATA_END_BIT_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_CUR_LMT_ERR_Pos (7)                                           /*!< SDH_T::ERROR_INT_STAT_R: CUR_LMT_ERR Position*/
#define SDH_ERROR_INT_STAT_R_CUR_LMT_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_CUR_LMT_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: CUR_LMT_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_AUTO_CMD_ERR_Pos (8)                                          /*!< SDH_T::ERROR_INT_STAT_R: AUTO_CMD_ERR Position*/
#define SDH_ERROR_INT_STAT_R_AUTO_CMD_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_AUTO_CMD_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: AUTO_CMD_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_ADMA_ERR_Pos (9)                                              /*!< SDH_T::ERROR_INT_STAT_R: ADMA_ERR Position*/
#define SDH_ERROR_INT_STAT_R_ADMA_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_ADMA_ERR_Pos)     /*!< SDH_T::ERROR_INT_STAT_R: ADMA_ERR Mask */

#define SDH_ERROR_INT_STAT_R_TUNING_ERR_Pos (10)                                           /*!< SDH_T::ERROR_INT_STAT_R: TUNING_ERR Position*/
#define SDH_ERROR_INT_STAT_R_TUNING_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_TUNING_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: TUNING_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_RESP_ERR_Pos (11)                                             /*!< SDH_T::ERROR_INT_STAT_R: RESP_ERR Position*/
#define SDH_ERROR_INT_STAT_R_RESP_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_RESP_ERR_Pos)     /*!< SDH_T::ERROR_INT_STAT_R: RESP_ERR Mask */

#define SDH_ERROR_INT_STAT_R_BOOT_ACK_ERR_Pos (12)                                         /*!< SDH_T::ERROR_INT_STAT_R: BOOT_ACK_ERR Position*/
#define SDH_ERROR_INT_STAT_R_BOOT_ACK_ERR_Msk (0x1ul << SDH_ERROR_INT_STAT_R_BOOT_ACK_ERR_Pos) /*!< SDH_T::ERROR_INT_STAT_R: BOOT_ACK_ERR Mask*/

#define SDH_ERROR_INT_STAT_R_VENDOR_ERR1_Pos (13)                                          /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR1 Position*/
#define SDH_ERROR_INT_STAT_R_VENDOR_ERR1_Msk (0x1ul << SDH_ERROR_INT_STAT_R_VENDOR_ERR1_Pos) /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR1 Mask*/

#define SDH_ERROR_INT_STAT_R_VENDOR_ERR2_Pos (14)                                          /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR2 Position*/
#define SDH_ERROR_INT_STAT_R_VENDOR_ERR2_Msk (0x1ul << SDH_ERROR_INT_STAT_R_VENDOR_ERR2_Pos) /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR2 Mask*/

#define SDH_ERROR_INT_STAT_R_VENDOR_ERR3_Pos (15)                                          /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR3 Position*/
#define SDH_ERROR_INT_STAT_R_VENDOR_ERR3_Msk (0x1ul << SDH_ERROR_INT_STAT_R_VENDOR_ERR3_Pos) /*!< SDH_T::ERROR_INT_STAT_R: VENDOR_ERR3 Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_CMD_COMPLETE_STAT_EN_Pos (0)                              /*!< SDH_T::NORMAL_INT_STAT_EN_R: CMD_COMPLETE_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_CMD_COMPLETE_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_CMD_COMPLETE_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: CMD_COMPLETE_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_XFER_COMPLETE_STAT_EN_Pos (1)                             /*!< SDH_T::NORMAL_INT_STAT_EN_R: XFER_COMPLETE_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_XFER_COMPLETE_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_XFER_COMPLETE_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: XFER_COMPLETE_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_BGAP_EVENT_STAT_EN_Pos (2)                                /*!< SDH_T::NORMAL_INT_STAT_EN_R: BGAP_EVENT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_BGAP_EVENT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_BGAP_EVENT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: BGAP_EVENT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_DMA_INTERRUPT_STAT_EN_Pos (3)                             /*!< SDH_T::NORMAL_INT_STAT_EN_R: DMA_INTERRUPT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_DMA_INTERRUPT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_DMA_INTERRUPT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: DMA_INTERRUPT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_BUF_WR_READY_STAT_EN_Pos (4)                              /*!< SDH_T::NORMAL_INT_STAT_EN_R: BUF_WR_READY_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_BUF_WR_READY_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_BUF_WR_READY_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: BUF_WR_READY_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_BUF_RD_READY_STAT_EN_Pos (5)                              /*!< SDH_T::NORMAL_INT_STAT_EN_R: BUF_RD_READY_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_BUF_RD_READY_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_BUF_RD_READY_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: BUF_RD_READY_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_CARD_INSERTION_STAT_EN_Pos (6)                            /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_INSERTION_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_CARD_INSERTION_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_CARD_INSERTION_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_INSERTION_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_CARD_REMOVAL_STAT_EN_Pos (7)                              /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_REMOVAL_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_CARD_REMOVAL_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_CARD_REMOVAL_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_REMOVAL_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_CARD_INTERRUPT_STAT_EN_Pos (8)                            /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_INTERRUPT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_CARD_INTERRUPT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_CARD_INTERRUPT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: CARD_INTERRUPT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_INT_A_STAT_EN_Pos (9)                                     /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_A_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_INT_A_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_INT_A_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_A_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_INT_B_STAT_EN_Pos (10)                                    /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_B_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_INT_B_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_INT_B_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_B_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_INT_C_STAT_EN_Pos (11)                                    /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_C_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_INT_C_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_INT_C_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: INT_C_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_RE_TUNE_EVENT_STAT_EN_Pos (12)                            /*!< SDH_T::NORMAL_INT_STAT_EN_R: RE_TUNE_EVENT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_RE_TUNE_EVENT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_RE_TUNE_EVENT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: RE_TUNE_EVENT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_FX_EVENT_STAT_EN_Pos (13)                                 /*!< SDH_T::NORMAL_INT_STAT_EN_R: FX_EVENT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_FX_EVENT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_FX_EVENT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: FX_EVENT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_CQE_EVENT_STAT_EN_Pos (14)                                /*!< SDH_T::NORMAL_INT_STAT_EN_R: CQE_EVENT_STAT_EN Position*/
#define SDH_NORMAL_INT_STAT_EN_R_CQE_EVENT_STAT_EN_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_CQE_EVENT_STAT_EN_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: CQE_EVENT_STAT_EN Mask*/

#define SDH_NORMAL_INT_STAT_EN_R_RSVD_15_Pos (15)                                          /*!< SDH_T::NORMAL_INT_STAT_EN_R: RSVD_15 Position*/
#define SDH_NORMAL_INT_STAT_EN_R_RSVD_15_Msk (0x1ul << SDH_NORMAL_INT_STAT_EN_R_RSVD_15_Pos) /*!< SDH_T::NORMAL_INT_STAT_EN_R: RSVD_15 Mask*/

#define SDH_ERROR_INT_STAT_EN_R_CMD_TOUT_ERR_STAT_EN_Pos (0)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_TOUT_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_CMD_TOUT_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_CMD_TOUT_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_TOUT_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_CMD_CRC_ERR_STAT_EN_Pos (1)                                /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_CRC_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_CMD_CRC_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_CMD_CRC_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_CRC_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_CMD_END_BIT_ERR_STAT_EN_Pos (2)                            /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_END_BIT_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_CMD_END_BIT_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_CMD_END_BIT_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_END_BIT_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_CMD_IDX_ERR_STAT_EN_Pos (3)                                /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_IDX_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_CMD_IDX_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_CMD_IDX_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: CMD_IDX_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_DATA_TOUT_ERR_STAT_EN_Pos (4)                              /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_TOUT_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_DATA_TOUT_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_DATA_TOUT_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_TOUT_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_DATA_CRC_ERR_STAT_EN_Pos (5)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_CRC_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_DATA_CRC_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_DATA_CRC_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_CRC_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_DATA_END_BIT_ERR_STAT_EN_Pos (6)                           /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_END_BIT_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_DATA_END_BIT_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_DATA_END_BIT_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: DATA_END_BIT_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_CUR_LMT_ERR_STAT_EN_Pos (7)                                /*!< SDH_T::ERROR_INT_STAT_EN_R: CUR_LMT_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_CUR_LMT_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_CUR_LMT_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: CUR_LMT_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_AUTO_CMD_ERR_STAT_EN_Pos (8)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: AUTO_CMD_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_AUTO_CMD_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_AUTO_CMD_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: AUTO_CMD_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_ADMA_ERR_STAT_EN_Pos (9)                                   /*!< SDH_T::ERROR_INT_STAT_EN_R: ADMA_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_ADMA_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_ADMA_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: ADMA_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_TUNING_ERR_STAT_EN_Pos (10)                                /*!< SDH_T::ERROR_INT_STAT_EN_R: TUNING_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_TUNING_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_TUNING_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: TUNING_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_RESP_ERR_STAT_EN_Pos (11)                                  /*!< SDH_T::ERROR_INT_STAT_EN_R: RESP_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_RESP_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_RESP_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: RESP_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_BOOT_ACK_ERR_STAT_EN_Pos (12)                              /*!< SDH_T::ERROR_INT_STAT_EN_R: BOOT_ACK_ERR_STAT_EN Position*/
#define SDH_ERROR_INT_STAT_EN_R_BOOT_ACK_ERR_STAT_EN_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_BOOT_ACK_ERR_STAT_EN_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: BOOT_ACK_ERR_STAT_EN Mask*/

#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN1_Pos (13)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN1 Position*/
#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN1_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN1_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN1 Mask*/

#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN2_Pos (14)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN2 Position*/
#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN2_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN2_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN2 Mask*/

#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN3_Pos (15)                               /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN3 Position*/
#define SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN3_Msk (0x1ul << SDH_ERROR_INT_STAT_EN_R_VENDOR_ERR_STAT_EN3_Pos) /*!< SDH_T::ERROR_INT_STAT_EN_R: VENDOR_ERR_STAT_EN3 Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_CMD_COMPLETE_SIGNAL_EN_Pos (0)                          /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CMD_COMPLETE_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_CMD_COMPLETE_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_CMD_COMPLETE_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CMD_COMPLETE_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_XFER_COMPLETE_SIGNAL_EN_Pos (1)                         /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: XFER_COMPLETE_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_XFER_COMPLETE_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_XFER_COMPLETE_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: XFER_COMPLETE_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_BGAP_EVENT_SIGNAL_EN_Pos (2)                            /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BGAP_EVENT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_BGAP_EVENT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_BGAP_EVENT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BGAP_EVENT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_DMA_INTERRUPT_SIGNAL_EN_Pos (3)                         /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: DMA_INTERRUPT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_DMA_INTERRUPT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_DMA_INTERRUPT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: DMA_INTERRUPT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_BUF_WR_READY_SIGNAL_EN_Pos (4)                          /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BUF_WR_READY_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_BUF_WR_READY_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_BUF_WR_READY_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BUF_WR_READY_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_BUF_RD_READY_SIGNAL_EN_Pos (5)                          /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BUF_RD_READY_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_BUF_RD_READY_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_BUF_RD_READY_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: BUF_RD_READY_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INSERTION_SIGNAL_EN_Pos (6)                        /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_INSERTION_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INSERTION_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INSERTION_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_INSERTION_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_REMOVAL_SIGNAL_EN_Pos (7)                          /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_REMOVAL_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_REMOVAL_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_CARD_REMOVAL_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_REMOVAL_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INTERRUPT_SIGNAL_EN_Pos (8)                        /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_INTERRUPT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INTERRUPT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_CARD_INTERRUPT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CARD_INTERRUPT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_A_SIGNAL_EN_Pos (9)                                 /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_A_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_A_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_INT_A_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_A_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_B_SIGNAL_EN_Pos (10)                                /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_B_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_B_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_INT_B_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_B_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_C_SIGNAL_EN_Pos (11)                                /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_C_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_INT_C_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_INT_C_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: INT_C_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_RE_TUNE_EVENT_SIGNAL_EN_Pos (12)                        /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: RE_TUNE_EVENT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_RE_TUNE_EVENT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_RE_TUNE_EVENT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: RE_TUNE_EVENT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_FX_EVENT_SIGNAL_EN_Pos (13)                             /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: FX_EVENT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_FX_EVENT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_FX_EVENT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: FX_EVENT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_CQE_EVENT_SIGNAL_EN_Pos (14)                            /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CQE_EVENT_SIGNAL_EN Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_CQE_EVENT_SIGNAL_EN_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_CQE_EVENT_SIGNAL_EN_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: CQE_EVENT_SIGNAL_EN Mask*/

#define SDH_NORMAL_INT_SIGNAL_EN_R_RSVD_15_Pos (15)                                        /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: RSVD_15 Position*/
#define SDH_NORMAL_INT_SIGNAL_EN_R_RSVD_15_Msk (0x1ul << SDH_NORMAL_INT_SIGNAL_EN_R_RSVD_15_Pos) /*!< SDH_T::NORMAL_INT_SIGNAL_EN_R: RSVD_15 Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_TOUT_ERR_SIGNAL_EN_Pos (0)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_TOUT_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_TOUT_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_CMD_TOUT_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_TOUT_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_CRC_ERR_SIGNAL_EN_Pos (1)                            /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_CRC_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_CRC_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_CMD_CRC_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_CRC_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_END_BIT_ERR_SIGNAL_EN_Pos (2)                        /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_END_BIT_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_END_BIT_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_CMD_END_BIT_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_END_BIT_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_IDX_ERR_SIGNAL_EN_Pos (3)                            /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_IDX_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_CMD_IDX_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_CMD_IDX_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CMD_IDX_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_TOUT_ERR_SIGNAL_EN_Pos (4)                          /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_TOUT_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_TOUT_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_DATA_TOUT_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_TOUT_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_CRC_ERR_SIGNAL_EN_Pos (5)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_CRC_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_CRC_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_DATA_CRC_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_CRC_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_END_BIT_ERR_SIGNAL_EN_Pos (6)                       /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_END_BIT_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_DATA_END_BIT_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_DATA_END_BIT_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: DATA_END_BIT_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_CUR_LMT_ERR_SIGNAL_EN_Pos (7)                            /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CUR_LMT_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_CUR_LMT_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_CUR_LMT_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: CUR_LMT_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_AUTO_CMD_ERR_SIGNAL_EN_Pos (8)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: AUTO_CMD_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_AUTO_CMD_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_AUTO_CMD_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: AUTO_CMD_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_ADMA_ERR_SIGNAL_EN_Pos (9)                               /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: ADMA_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_ADMA_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_ADMA_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: ADMA_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_TUNING_ERR_SIGNAL_EN_Pos (10)                            /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: TUNING_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_TUNING_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_TUNING_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: TUNING_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_RESP_ERR_SIGNAL_EN_Pos (11)                              /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: RESP_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_RESP_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_RESP_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: RESP_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_BOOT_ACK_ERR_SIGNAL_EN_Pos (12)                          /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: BOOT_ACK_ERR_SIGNAL_EN Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_BOOT_ACK_ERR_SIGNAL_EN_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_BOOT_ACK_ERR_SIGNAL_EN_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: BOOT_ACK_ERR_SIGNAL_EN Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN1_Pos (13)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN1 Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN1_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN1_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN1 Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN2_Pos (14)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN2 Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN2_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN2_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN2 Mask*/

#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN3_Pos (15)                           /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN3 Position*/
#define SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN3_Msk (0x1ul << SDH_ERROR_INT_SIGNAL_EN_R_VENDOR_ERR_SIGNAL_EN3_Pos) /*!< SDH_T::ERROR_INT_SIGNAL_EN_R: VENDOR_ERR_SIGNAL_EN3 Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD12_NOT_EXEC_Pos (0)                                    /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD12_NOT_EXEC Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD12_NOT_EXEC_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD12_NOT_EXEC_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD12_NOT_EXEC Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_TOUT_ERR_Pos (1)                                      /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_TOUT_ERR Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_TOUT_ERR_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD_TOUT_ERR_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_TOUT_ERR Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_CRC_ERR_Pos (2)                                       /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_CRC_ERR Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_CRC_ERR_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD_CRC_ERR_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_CRC_ERR Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_EBIT_ERR_Pos (3)                                      /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_EBIT_ERR Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_EBIT_ERR_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD_EBIT_ERR_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_EBIT_ERR Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_IDX_ERR_Pos (4)                                       /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_IDX_ERR Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_IDX_ERR_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD_IDX_ERR_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_IDX_ERR Mask*/

#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_RESP_ERR_Pos (5)                                      /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_RESP_ERR Position*/
#define SDH_AUTO_CMD_STAT_R_AUTO_CMD_RESP_ERR_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_AUTO_CMD_RESP_ERR_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: AUTO_CMD_RESP_ERR Mask*/

#define SDH_AUTO_CMD_STAT_R_RSVD_6_Pos   (6)                                               /*!< SDH_T::AUTO_CMD_STAT_R: RSVD_6 Position*/
#define SDH_AUTO_CMD_STAT_R_RSVD_6_Msk   (0x1ul << SDH_AUTO_CMD_STAT_R_RSVD_6_Pos)         /*!< SDH_T::AUTO_CMD_STAT_R: RSVD_6 Mask    */

#define SDH_AUTO_CMD_STAT_R_CMD_NOT_ISSUED_AUTO_CMD12_Pos (7)                              /*!< SDH_T::AUTO_CMD_STAT_R: CMD_NOT_ISSUED_AUTO_CMD12 Position*/
#define SDH_AUTO_CMD_STAT_R_CMD_NOT_ISSUED_AUTO_CMD12_Msk (0x1ul << SDH_AUTO_CMD_STAT_R_CMD_NOT_ISSUED_AUTO_CMD12_Pos) /*!< SDH_T::AUTO_CMD_STAT_R: CMD_NOT_ISSUED_AUTO_CMD12 Mask*/

#define SDH_AUTO_CMD_STAT_R_RSVD_15_8_Pos (8)                                              /*!< SDH_T::AUTO_CMD_STAT_R: RSVD_15_8 Position*/
#define SDH_AUTO_CMD_STAT_R_RSVD_15_8_Msk (0xfful << SDH_AUTO_CMD_STAT_R_RSVD_15_8_Pos)    /*!< SDH_T::AUTO_CMD_STAT_R: RSVD_15_8 Mask */

#define SDH_HOST_CTRL2_R_UHS_MODE_SEL_Pos (0)                                              /*!< SDH_T::HOST_CTRL2_R: UHS_MODE_SEL Position*/
#define SDH_HOST_CTRL2_R_UHS_MODE_SEL_Msk (0x7ul << SDH_HOST_CTRL2_R_UHS_MODE_SEL_Pos)     /*!< SDH_T::HOST_CTRL2_R: UHS_MODE_SEL Mask */

#define SDH_HOST_CTRL2_R_SIGNALING_EN_Pos (3)                                              /*!< SDH_T::HOST_CTRL2_R: SIGNALING_EN Position*/
#define SDH_HOST_CTRL2_R_SIGNALING_EN_Msk (0x1ul << SDH_HOST_CTRL2_R_SIGNALING_EN_Pos)     /*!< SDH_T::HOST_CTRL2_R: SIGNALING_EN Mask */

#define SDH_HOST_CTRL2_R_DRV_STRENGTH_SEL_Pos (4)                                          /*!< SDH_T::HOST_CTRL2_R: DRV_STRENGTH_SEL Position*/
#define SDH_HOST_CTRL2_R_DRV_STRENGTH_SEL_Msk (0x3ul << SDH_HOST_CTRL2_R_DRV_STRENGTH_SEL_Pos) /*!< SDH_T::HOST_CTRL2_R: DRV_STRENGTH_SEL Mask*/

#define SDH_HOST_CTRL2_R_EXEC_TUNING_Pos (6)                                               /*!< SDH_T::HOST_CTRL2_R: EXEC_TUNING Position*/
#define SDH_HOST_CTRL2_R_EXEC_TUNING_Msk (0x1ul << SDH_HOST_CTRL2_R_EXEC_TUNING_Pos)       /*!< SDH_T::HOST_CTRL2_R: EXEC_TUNING Mask  */

#define SDH_HOST_CTRL2_R_SAMPLE_CLK_SEL_Pos (7)                                            /*!< SDH_T::HOST_CTRL2_R: SAMPLE_CLK_SEL Position*/
#define SDH_HOST_CTRL2_R_SAMPLE_CLK_SEL_Msk (0x1ul << SDH_HOST_CTRL2_R_SAMPLE_CLK_SEL_Pos) /*!< SDH_T::HOST_CTRL2_R: SAMPLE_CLK_SEL Mask*/

#define SDH_HOST_CTRL2_R_UHS2_IF_ENABLE_Pos (8)                                            /*!< SDH_T::HOST_CTRL2_R: UHS2_IF_ENABLE Position*/
#define SDH_HOST_CTRL2_R_UHS2_IF_ENABLE_Msk (0x1ul << SDH_HOST_CTRL2_R_UHS2_IF_ENABLE_Pos) /*!< SDH_T::HOST_CTRL2_R: UHS2_IF_ENABLE Mask*/

#define SDH_HOST_CTRL2_R_RSVD_9_Pos      (9)                                               /*!< SDH_T::HOST_CTRL2_R: RSVD_9 Position   */
#define SDH_HOST_CTRL2_R_RSVD_9_Msk      (0x1ul << SDH_HOST_CTRL2_R_RSVD_9_Pos)            /*!< SDH_T::HOST_CTRL2_R: RSVD_9 Mask       */

#define SDH_HOST_CTRL2_R_ADMA2_LEN_MODE_Pos (10)                                           /*!< SDH_T::HOST_CTRL2_R: ADMA2_LEN_MODE Position*/
#define SDH_HOST_CTRL2_R_ADMA2_LEN_MODE_Msk (0x1ul << SDH_HOST_CTRL2_R_ADMA2_LEN_MODE_Pos) /*!< SDH_T::HOST_CTRL2_R: ADMA2_LEN_MODE Mask*/

#define SDH_HOST_CTRL2_R_CMD23_ENABLE_Pos (11)                                             /*!< SDH_T::HOST_CTRL2_R: CMD23_ENABLE Position*/
#define SDH_HOST_CTRL2_R_CMD23_ENABLE_Msk (0x1ul << SDH_HOST_CTRL2_R_CMD23_ENABLE_Pos)     /*!< SDH_T::HOST_CTRL2_R: CMD23_ENABLE Mask */

#define SDH_HOST_CTRL2_R_HOST_VER4_ENABLE_Pos (12)                                         /*!< SDH_T::HOST_CTRL2_R: HOST_VER4_ENABLE Position*/
#define SDH_HOST_CTRL2_R_HOST_VER4_ENABLE_Msk (0x1ul << SDH_HOST_CTRL2_R_HOST_VER4_ENABLE_Pos) /*!< SDH_T::HOST_CTRL2_R: HOST_VER4_ENABLE Mask*/

#define SDH_HOST_CTRL2_R_ADDRESSING_Pos  (13)                                              /*!< SDH_T::HOST_CTRL2_R: ADDRESSING Position*/
#define SDH_HOST_CTRL2_R_ADDRESSING_Msk  (0x1ul << SDH_HOST_CTRL2_R_ADDRESSING_Pos)        /*!< SDH_T::HOST_CTRL2_R: ADDRESSING Mask   */

#define SDH_HOST_CTRL2_R_ASYNC_INT_ENABLE_Pos (14)                                         /*!< SDH_T::HOST_CTRL2_R: ASYNC_INT_ENABLE Position*/
#define SDH_HOST_CTRL2_R_ASYNC_INT_ENABLE_Msk (0x1ul << SDH_HOST_CTRL2_R_ASYNC_INT_ENABLE_Pos) /*!< SDH_T::HOST_CTRL2_R: ASYNC_INT_ENABLE Mask*/

#define SDH_HOST_CTRL2_R_PRESET_VAL_ENABLE_Pos (15)                                        /*!< SDH_T::HOST_CTRL2_R: PRESET_VAL_ENABLE Position*/
#define SDH_HOST_CTRL2_R_PRESET_VAL_ENABLE_Msk (0x1ul << SDH_HOST_CTRL2_R_PRESET_VAL_ENABLE_Pos) /*!< SDH_T::HOST_CTRL2_R: PRESET_VAL_ENABLE Mask*/

#define SDH_CAPABILITIES1_R_TOUT_CLK_FREQ_Pos (0)                                          /*!< SDH_T::CAPABILITIES1_R: TOUT_CLK_FREQ Position*/
#define SDH_CAPABILITIES1_R_TOUT_CLK_FREQ_Msk (0x3ful << SDH_CAPABILITIES1_R_TOUT_CLK_FREQ_Pos) /*!< SDH_T::CAPABILITIES1_R: TOUT_CLK_FREQ Mask*/

#define SDH_CAPABILITIES1_R_RSVD_6_Pos   (6)                                               /*!< SDH_T::CAPABILITIES1_R: RSVD_6 Position*/
#define SDH_CAPABILITIES1_R_RSVD_6_Msk   (0x1ul << SDH_CAPABILITIES1_R_RSVD_6_Pos)         /*!< SDH_T::CAPABILITIES1_R: RSVD_6 Mask    */

#define SDH_CAPABILITIES1_R_TOUT_CLK_UNIT_Pos (7)                                          /*!< SDH_T::CAPABILITIES1_R: TOUT_CLK_UNIT Position*/
#define SDH_CAPABILITIES1_R_TOUT_CLK_UNIT_Msk (0x1ul << SDH_CAPABILITIES1_R_TOUT_CLK_UNIT_Pos) /*!< SDH_T::CAPABILITIES1_R: TOUT_CLK_UNIT Mask*/

#define SDH_CAPABILITIES1_R_BASE_CLK_FREQ_Pos (8)                                          /*!< SDH_T::CAPABILITIES1_R: BASE_CLK_FREQ Position*/
#define SDH_CAPABILITIES1_R_BASE_CLK_FREQ_Msk (0xfful << SDH_CAPABILITIES1_R_BASE_CLK_FREQ_Pos) /*!< SDH_T::CAPABILITIES1_R: BASE_CLK_FREQ Mask*/

#define SDH_CAPABILITIES1_R_MAX_BLK_LEN_Pos (16)                                           /*!< SDH_T::CAPABILITIES1_R: MAX_BLK_LEN Position*/
#define SDH_CAPABILITIES1_R_MAX_BLK_LEN_Msk (0x3ul << SDH_CAPABILITIES1_R_MAX_BLK_LEN_Pos) /*!< SDH_T::CAPABILITIES1_R: MAX_BLK_LEN Mask*/

#define SDH_CAPABILITIES1_R_Embedded_8_BIT_Pos (18)                                        /*!< SDH_T::CAPABILITIES1_R: Embedded_8_BIT Position*/
#define SDH_CAPABILITIES1_R_Embedded_8_BIT_Msk (0x1ul << SDH_CAPABILITIES1_R_Embedded_8_BIT_Pos) /*!< SDH_T::CAPABILITIES1_R: Embedded_8_BIT Mask*/

#define SDH_CAPABILITIES1_R_ADMA2_SUPPORT_Pos (19)                                         /*!< SDH_T::CAPABILITIES1_R: ADMA2_SUPPORT Position*/
#define SDH_CAPABILITIES1_R_ADMA2_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES1_R_ADMA2_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES1_R: ADMA2_SUPPORT Mask*/

#define SDH_CAPABILITIES1_R_RSVD_20_Pos  (20)                                              /*!< SDH_T::CAPABILITIES1_R: RSVD_20 Position*/
#define SDH_CAPABILITIES1_R_RSVD_20_Msk  (0x1ul << SDH_CAPABILITIES1_R_RSVD_20_Pos)        /*!< SDH_T::CAPABILITIES1_R: RSVD_20 Mask   */

#define SDH_CAPABILITIES1_R_HIGH_SPEED_SUPPORT_Pos (21)                                    /*!< SDH_T::CAPABILITIES1_R: HIGH_SPEED_SUPPORT Position*/
#define SDH_CAPABILITIES1_R_HIGH_SPEED_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES1_R_HIGH_SPEED_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES1_R: HIGH_SPEED_SUPPORT Mask*/

#define SDH_CAPABILITIES1_R_SDMA_SUPPORT_Pos (22)                                          /*!< SDH_T::CAPABILITIES1_R: SDMA_SUPPORT Position*/
#define SDH_CAPABILITIES1_R_SDMA_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES1_R_SDMA_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES1_R: SDMA_SUPPORT Mask*/

#define SDH_CAPABILITIES1_R_SUS_RES_SUPPORT_Pos (23)                                       /*!< SDH_T::CAPABILITIES1_R: SUS_RES_SUPPORT Position*/
#define SDH_CAPABILITIES1_R_SUS_RES_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES1_R_SUS_RES_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES1_R: SUS_RES_SUPPORT Mask*/

#define SDH_CAPABILITIES1_R_VOLT_33_Pos  (24)                                              /*!< SDH_T::CAPABILITIES1_R: VOLT_33 Position*/
#define SDH_CAPABILITIES1_R_VOLT_33_Msk  (0x1ul << SDH_CAPABILITIES1_R_VOLT_33_Pos)        /*!< SDH_T::CAPABILITIES1_R: VOLT_33 Mask   */

#define SDH_CAPABILITIES1_R_VOLT_30_Pos  (25)                                              /*!< SDH_T::CAPABILITIES1_R: VOLT_30 Position*/
#define SDH_CAPABILITIES1_R_VOLT_30_Msk  (0x1ul << SDH_CAPABILITIES1_R_VOLT_30_Pos)        /*!< SDH_T::CAPABILITIES1_R: VOLT_30 Mask   */

#define SDH_CAPABILITIES1_R_VOLT_18_Pos  (26)                                              /*!< SDH_T::CAPABILITIES1_R: VOLT_18 Position*/
#define SDH_CAPABILITIES1_R_VOLT_18_Msk  (0x1ul << SDH_CAPABILITIES1_R_VOLT_18_Pos)        /*!< SDH_T::CAPABILITIES1_R: VOLT_18 Mask   */

#define SDH_CAPABILITIES1_R_SYS_ADDR_64_V4_Pos (27)                                        /*!< SDH_T::CAPABILITIES1_R: SYS_ADDR_64_V4 Position*/
#define SDH_CAPABILITIES1_R_SYS_ADDR_64_V4_Msk (0x1ul << SDH_CAPABILITIES1_R_SYS_ADDR_64_V4_Pos) /*!< SDH_T::CAPABILITIES1_R: SYS_ADDR_64_V4 Mask*/

#define SDH_CAPABILITIES1_R_SYS_ADDR_64_V3_Pos (28)                                        /*!< SDH_T::CAPABILITIES1_R: SYS_ADDR_64_V3 Position*/
#define SDH_CAPABILITIES1_R_SYS_ADDR_64_V3_Msk (0x1ul << SDH_CAPABILITIES1_R_SYS_ADDR_64_V3_Pos) /*!< SDH_T::CAPABILITIES1_R: SYS_ADDR_64_V3 Mask*/

#define SDH_CAPABILITIES1_R_ASYNC_INT_SUPPORT_Pos (29)                                     /*!< SDH_T::CAPABILITIES1_R: ASYNC_INT_SUPPORT Position*/
#define SDH_CAPABILITIES1_R_ASYNC_INT_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES1_R_ASYNC_INT_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES1_R: ASYNC_INT_SUPPORT Mask*/

#define SDH_CAPABILITIES1_R_SLOT_TYPE_R_Pos (30)                                           /*!< SDH_T::CAPABILITIES1_R: SLOT_TYPE_R Position*/
#define SDH_CAPABILITIES1_R_SLOT_TYPE_R_Msk (0x3ul << SDH_CAPABILITIES1_R_SLOT_TYPE_R_Pos) /*!< SDH_T::CAPABILITIES1_R: SLOT_TYPE_R Mask*/

#define SDH_CAPABILITIES2_R_SDR50_SUPPORT_Pos (0)                                          /*!< SDH_T::CAPABILITIES2_R: SDR50_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_SDR50_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_SDR50_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: SDR50_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_SDR104_SUPPORT_Pos (1)                                         /*!< SDH_T::CAPABILITIES2_R: SDR104_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_SDR104_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_SDR104_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: SDR104_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_DDR50_SUPPORT_Pos (2)                                          /*!< SDH_T::CAPABILITIES2_R: DDR50_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_DDR50_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_DDR50_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: DDR50_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_UHS2_SUPPORT_Pos (3)                                           /*!< SDH_T::CAPABILITIES2_R: UHS2_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_UHS2_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_UHS2_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: UHS2_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_DRV_TYPEA_Pos (4)                                              /*!< SDH_T::CAPABILITIES2_R: DRV_TYPEA Position*/
#define SDH_CAPABILITIES2_R_DRV_TYPEA_Msk (0x1ul << SDH_CAPABILITIES2_R_DRV_TYPEA_Pos)     /*!< SDH_T::CAPABILITIES2_R: DRV_TYPEA Mask */

#define SDH_CAPABILITIES2_R_DRV_TYPEC_Pos (5)                                              /*!< SDH_T::CAPABILITIES2_R: DRV_TYPEC Position*/
#define SDH_CAPABILITIES2_R_DRV_TYPEC_Msk (0x1ul << SDH_CAPABILITIES2_R_DRV_TYPEC_Pos)     /*!< SDH_T::CAPABILITIES2_R: DRV_TYPEC Mask */

#define SDH_CAPABILITIES2_R_DRV_TYPED_Pos (6)                                              /*!< SDH_T::CAPABILITIES2_R: DRV_TYPED Position*/
#define SDH_CAPABILITIES2_R_DRV_TYPED_Msk (0x1ul << SDH_CAPABILITIES2_R_DRV_TYPED_Pos)     /*!< SDH_T::CAPABILITIES2_R: DRV_TYPED Mask */

#define SDH_CAPABILITIES2_R_RSVD_39_Pos  (7)                                               /*!< SDH_T::CAPABILITIES2_R: RSVD_39 Position*/
#define SDH_CAPABILITIES2_R_RSVD_39_Msk  (0x1ul << SDH_CAPABILITIES2_R_RSVD_39_Pos)        /*!< SDH_T::CAPABILITIES2_R: RSVD_39 Mask   */

#define SDH_CAPABILITIES2_R_RETUNE_CNT_Pos (8)                                             /*!< SDH_T::CAPABILITIES2_R: RETUNE_CNT Position*/
#define SDH_CAPABILITIES2_R_RETUNE_CNT_Msk (0xful << SDH_CAPABILITIES2_R_RETUNE_CNT_Pos)   /*!< SDH_T::CAPABILITIES2_R: RETUNE_CNT Mask*/

#define SDH_CAPABILITIES2_R_RSVD_44_Pos  (12)                                              /*!< SDH_T::CAPABILITIES2_R: RSVD_44 Position*/
#define SDH_CAPABILITIES2_R_RSVD_44_Msk  (0x1ul << SDH_CAPABILITIES2_R_RSVD_44_Pos)        /*!< SDH_T::CAPABILITIES2_R: RSVD_44 Mask   */

#define SDH_CAPABILITIES2_R_USE_TUNING_SDR50_Pos (13)                                      /*!< SDH_T::CAPABILITIES2_R: USE_TUNING_SDR50 Position*/
#define SDH_CAPABILITIES2_R_USE_TUNING_SDR50_Msk (0x1ul << SDH_CAPABILITIES2_R_USE_TUNING_SDR50_Pos) /*!< SDH_T::CAPABILITIES2_R: USE_TUNING_SDR50 Mask*/

#define SDH_CAPABILITIES2_R_RE_TUNING_MODES_Pos (14)                                       /*!< SDH_T::CAPABILITIES2_R: RE_TUNING_MODES Position*/
#define SDH_CAPABILITIES2_R_RE_TUNING_MODES_Msk (0x3ul << SDH_CAPABILITIES2_R_RE_TUNING_MODES_Pos) /*!< SDH_T::CAPABILITIES2_R: RE_TUNING_MODES Mask*/

#define SDH_CAPABILITIES2_R_CLK_MUL_Pos  (16)                                              /*!< SDH_T::CAPABILITIES2_R: CLK_MUL Position*/
#define SDH_CAPABILITIES2_R_CLK_MUL_Msk  (0xfful << SDH_CAPABILITIES2_R_CLK_MUL_Pos)       /*!< SDH_T::CAPABILITIES2_R: CLK_MUL Mask   */

#define SDH_CAPABILITIES2_R_RSVD_56_58_Pos (24)                                            /*!< SDH_T::CAPABILITIES2_R: RSVD_56_58 Position*/
#define SDH_CAPABILITIES2_R_RSVD_56_58_Msk (0x7ul << SDH_CAPABILITIES2_R_RSVD_56_58_Pos)   /*!< SDH_T::CAPABILITIES2_R: RSVD_56_58 Mask*/

#define SDH_CAPABILITIES2_R_ADMA3_SUPPORT_Pos (27)                                         /*!< SDH_T::CAPABILITIES2_R: ADMA3_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_ADMA3_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_ADMA3_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: ADMA3_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_VDD2_18V_SUPPORT_Pos (28)                                      /*!< SDH_T::CAPABILITIES2_R: VDD2_18V_SUPPORT Position*/
#define SDH_CAPABILITIES2_R_VDD2_18V_SUPPORT_Msk (0x1ul << SDH_CAPABILITIES2_R_VDD2_18V_SUPPORT_Pos) /*!< SDH_T::CAPABILITIES2_R: VDD2_18V_SUPPORT Mask*/

#define SDH_CAPABILITIES2_R_RSVD_61_Pos  (29)                                              /*!< SDH_T::CAPABILITIES2_R: RSVD_61 Position*/
#define SDH_CAPABILITIES2_R_RSVD_61_Msk  (0x1ul << SDH_CAPABILITIES2_R_RSVD_61_Pos)        /*!< SDH_T::CAPABILITIES2_R: RSVD_61 Mask   */

#define SDH_CAPABILITIES2_R_RSVD_62_63_Pos (30)                                            /*!< SDH_T::CAPABILITIES2_R: RSVD_62_63 Position*/
#define SDH_CAPABILITIES2_R_RSVD_62_63_Msk (0x3ul << SDH_CAPABILITIES2_R_RSVD_62_63_Pos)   /*!< SDH_T::CAPABILITIES2_R: RSVD_62_63 Mask*/

#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_33V_Pos (0)                                       /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_33V Position*/
#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_33V_Msk (0xfful << SDH_CURR_CAPABILITIES1_R_MAX_CUR_33V_Pos) /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_33V Mask*/

#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_30V_Pos (8)                                       /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_30V Position*/
#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_30V_Msk (0xfful << SDH_CURR_CAPABILITIES1_R_MAX_CUR_30V_Pos) /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_30V Mask*/

#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_18V_Pos (16)                                      /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_18V Position*/
#define SDH_CURR_CAPABILITIES1_R_MAX_CUR_18V_Msk (0xfful << SDH_CURR_CAPABILITIES1_R_MAX_CUR_18V_Pos) /*!< SDH_T::CURR_CAPABILITIES1_R: MAX_CUR_18V Mask*/

#define SDH_CURR_CAPABILITIES1_R_RSVD_31_24_Pos (24)                                       /*!< SDH_T::CURR_CAPABILITIES1_R: RSVD_31_24 Position*/
#define SDH_CURR_CAPABILITIES1_R_RSVD_31_24_Msk (0xfful << SDH_CURR_CAPABILITIES1_R_RSVD_31_24_Pos) /*!< SDH_T::CURR_CAPABILITIES1_R: RSVD_31_24 Mask*/

#define SDH_CURR_CAPABILITIES2_R_MAX_CUR_VDD2_18V_Pos (0)                                  /*!< SDH_T::CURR_CAPABILITIES2_R: MAX_CUR_VDD2_18V Position*/
#define SDH_CURR_CAPABILITIES2_R_MAX_CUR_VDD2_18V_Msk (0xfful << SDH_CURR_CAPABILITIES2_R_MAX_CUR_VDD2_18V_Pos) /*!< SDH_T::CURR_CAPABILITIES2_R: MAX_CUR_VDD2_18V Mask*/

#define SDH_CURR_CAPABILITIES2_R_RSVD_63_40_Pos (8)                                        /*!< SDH_T::CURR_CAPABILITIES2_R: RSVD_63_40 Position*/
#define SDH_CURR_CAPABILITIES2_R_RSVD_63_40_Msk (0xfffffful << SDH_CURR_CAPABILITIES2_R_RSVD_63_40_Pos) /*!< SDH_T::CURR_CAPABILITIES2_R: RSVD_63_40 Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD12_NOT_EXEC_Pos (0)                        /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD12_NOT_EXEC Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD12_NOT_EXEC_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD12_NOT_EXEC_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD12_NOT_EXEC Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_TOUT_ERR_Pos (1)                          /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_TOUT_ERR Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_TOUT_ERR_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_TOUT_ERR_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_TOUT_ERR Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_CRC_ERR_Pos (2)                           /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_CRC_ERR Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_CRC_ERR_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_CRC_ERR_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_CRC_ERR Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_EBIT_ERR_Pos (3)                          /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_EBIT_ERR Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_EBIT_ERR_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_EBIT_ERR_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_EBIT_ERR Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_IDX_ERR_Pos (4)                           /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_IDX_ERR Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_IDX_ERR_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_IDX_ERR_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_IDX_ERR Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_RESP_ERR_Pos (5)                          /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_RESP_ERR Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_RESP_ERR_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_AUTO_CMD_RESP_ERR_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_AUTO_CMD_RESP_ERR Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_RSVD_6_Pos (6)                                           /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: RSVD_6 Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_RSVD_6_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_RSVD_6_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: RSVD_6 Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_CMD_NOT_ISSUED_AUTO_CMD12_Pos (7)                  /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_CMD_NOT_ISSUED_AUTO_CMD12 Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_FORCE_CMD_NOT_ISSUED_AUTO_CMD12_Msk (0x1ul << SDH_FORCE_AUTO_CMD_STAT_R_FORCE_CMD_NOT_ISSUED_AUTO_CMD12_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: FORCE_CMD_NOT_ISSUED_AUTO_CMD12 Mask*/

#define SDH_FORCE_AUTO_CMD_STAT_R_RSVD_15_8_Pos (8)                                        /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: RSVD_15_8 Position*/
#define SDH_FORCE_AUTO_CMD_STAT_R_RSVD_15_8_Msk (0xfful << SDH_FORCE_AUTO_CMD_STAT_R_RSVD_15_8_Pos) /*!< SDH_T::FORCE_AUTO_CMD_STAT_R: RSVD_15_8 Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_TOUT_ERR_Pos (0)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_TOUT_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_TOUT_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_TOUT_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_TOUT_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_CRC_ERR_Pos (1)                               /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_CRC_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_CRC_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_CRC_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_CRC_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_END_BIT_ERR_Pos (2)                           /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_END_BIT_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_END_BIT_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_END_BIT_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_END_BIT_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_IDX_ERR_Pos (3)                               /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_IDX_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_IDX_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_CMD_IDX_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CMD_IDX_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_TOUT_ERR_Pos (4)                             /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_TOUT_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_TOUT_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_TOUT_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_TOUT_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_CRC_ERR_Pos (5)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_CRC_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_CRC_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_CRC_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_CRC_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_END_BIT_ERR_Pos (6)                          /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_END_BIT_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_END_BIT_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_DATA_END_BIT_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_DATA_END_BIT_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CUR_LMT_ERR_Pos (7)                               /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CUR_LMT_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_CUR_LMT_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_CUR_LMT_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_CUR_LMT_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_AUTO_CMD_ERR_Pos (8)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_AUTO_CMD_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_AUTO_CMD_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_AUTO_CMD_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_AUTO_CMD_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_ADMA_ERR_Pos (9)                                  /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_ADMA_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_ADMA_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_ADMA_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_ADMA_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_TUNING_ERR_Pos (10)                               /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_TUNING_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_TUNING_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_TUNING_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_TUNING_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_RESP_ERR_Pos (11)                                 /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_RESP_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_RESP_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_RESP_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_RESP_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_BOOT_ACK_ERR_Pos (12)                             /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_BOOT_ACK_ERR Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_BOOT_ACK_ERR_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_BOOT_ACK_ERR_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_BOOT_ACK_ERR Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR1_Pos (13)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR1 Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR1_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR1_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR1 Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR2_Pos (14)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR2 Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR2_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR2_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR2 Mask*/

#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR3_Pos (15)                              /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR3 Position*/
#define SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR3_Msk (0x1ul << SDH_FORCE_ERROR_INT_STAT_R_FORCE_VENDOR_ERR3_Pos) /*!< SDH_T::FORCE_ERROR_INT_STAT_R: FORCE_VENDOR_ERR3 Mask*/

#define SDH_ADMA_ERR_STAT_R_ADMA_ERR_STATES_Pos (0)                                        /*!< SDH_T::ADMA_ERR_STAT_R: ADMA_ERR_STATES Position*/
#define SDH_ADMA_ERR_STAT_R_ADMA_ERR_STATES_Msk (0x3ul << SDH_ADMA_ERR_STAT_R_ADMA_ERR_STATES_Pos) /*!< SDH_T::ADMA_ERR_STAT_R: ADMA_ERR_STATES Mask*/

#define SDH_ADMA_ERR_STAT_R_ADMA_LEN_ERR_Pos (2)                                           /*!< SDH_T::ADMA_ERR_STAT_R: ADMA_LEN_ERR Position*/
#define SDH_ADMA_ERR_STAT_R_ADMA_LEN_ERR_Msk (0x1ul << SDH_ADMA_ERR_STAT_R_ADMA_LEN_ERR_Pos) /*!< SDH_T::ADMA_ERR_STAT_R: ADMA_LEN_ERR Mask*/

#define SDH_ADMA_ERR_STAT_R_RSVD_7_3_Pos (3)                                               /*!< SDH_T::ADMA_ERR_STAT_R: RSVD_7_3 Position*/
#define SDH_ADMA_ERR_STAT_R_RSVD_7_3_Msk (0x1ful << SDH_ADMA_ERR_STAT_R_RSVD_7_3_Pos)      /*!< SDH_T::ADMA_ERR_STAT_R: RSVD_7_3 Mask  */

#define SDH_ADMA_SA_LOW_R_ADMA_SA_LOW_Pos (0)                                              /*!< SDH_T::ADMA_SA_LOW_R: ADMA_SA_LOW Position*/
#define SDH_ADMA_SA_LOW_R_ADMA_SA_LOW_Msk (0xfffffffful << SDH_ADMA_SA_LOW_R_ADMA_SA_LOW_Pos) /*!< SDH_T::ADMA_SA_LOW_R: ADMA_SA_LOW Mask */

#define SDH_PRESET_INIT_R_FREQ_SEL_VAL_Pos (0)                                             /*!< SDH_T::PRESET_INIT_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_INIT_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_INIT_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_INIT_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_INIT_R_CLK_GEN_SEL_VAL_Pos (10)                                         /*!< SDH_T::PRESET_INIT_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_INIT_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_INIT_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_INIT_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_INIT_R_RSVD_13_11_Pos (11)                                              /*!< SDH_T::PRESET_INIT_R: RSVD_13_11 Position*/
#define SDH_PRESET_INIT_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_INIT_R_RSVD_13_11_Pos)       /*!< SDH_T::PRESET_INIT_R: RSVD_13_11 Mask  */

#define SDH_PRESET_INIT_R_DRV_SEL_VAL_Pos (14)                                             /*!< SDH_T::PRESET_INIT_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_INIT_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_INIT_R_DRV_SEL_VAL_Pos)     /*!< SDH_T::PRESET_INIT_R: DRV_SEL_VAL Mask */

#define SDH_PRESET_DS_R_FREQ_SEL_VAL_Pos (0)                                               /*!< SDH_T::PRESET_DS_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_DS_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_DS_R_FREQ_SEL_VAL_Pos)     /*!< SDH_T::PRESET_DS_R: FREQ_SEL_VAL Mask  */

#define SDH_PRESET_DS_R_CLK_GEN_SEL_VAL_Pos (10)                                           /*!< SDH_T::PRESET_DS_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_DS_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_DS_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_DS_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_DS_R_RSVD_13_11_Pos   (11)                                              /*!< SDH_T::PRESET_DS_R: RSVD_13_11 Position*/
#define SDH_PRESET_DS_R_RSVD_13_11_Msk   (0x7ul << SDH_PRESET_DS_R_RSVD_13_11_Pos)         /*!< SDH_T::PRESET_DS_R: RSVD_13_11 Mask    */

#define SDH_PRESET_DS_R_DRV_SEL_VAL_Pos  (14)                                              /*!< SDH_T::PRESET_DS_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_DS_R_DRV_SEL_VAL_Msk  (0x3ul << SDH_PRESET_DS_R_DRV_SEL_VAL_Pos)        /*!< SDH_T::PRESET_DS_R: DRV_SEL_VAL Mask   */

#define SDH_PRESET_HS_R_FREQ_SEL_VAL_Pos (0)                                               /*!< SDH_T::PRESET_HS_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_HS_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_HS_R_FREQ_SEL_VAL_Pos)     /*!< SDH_T::PRESET_HS_R: FREQ_SEL_VAL Mask  */

#define SDH_PRESET_HS_R_CLK_GEN_SEL_VAL_Pos (10)                                           /*!< SDH_T::PRESET_HS_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_HS_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_HS_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_HS_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_HS_R_RSVD_13_11_Pos   (11)                                              /*!< SDH_T::PRESET_HS_R: RSVD_13_11 Position*/
#define SDH_PRESET_HS_R_RSVD_13_11_Msk   (0x7ul << SDH_PRESET_HS_R_RSVD_13_11_Pos)         /*!< SDH_T::PRESET_HS_R: RSVD_13_11 Mask    */

#define SDH_PRESET_HS_R_DRV_SEL_VAL_Pos  (14)                                              /*!< SDH_T::PRESET_HS_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_HS_R_DRV_SEL_VAL_Msk  (0x3ul << SDH_PRESET_HS_R_DRV_SEL_VAL_Pos)        /*!< SDH_T::PRESET_HS_R: DRV_SEL_VAL Mask   */

#define SDH_PRESET_SDR12_R_FREQ_SEL_VAL_Pos (0)                                            /*!< SDH_T::PRESET_SDR12_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_SDR12_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_SDR12_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR12_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_SDR12_R_CLK_GEN_SEL_VAL_Pos (10)                                        /*!< SDH_T::PRESET_SDR12_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_SDR12_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_SDR12_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR12_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_SDR12_R_RSVD_13_11_Pos (11)                                             /*!< SDH_T::PRESET_SDR12_R: RSVD_13_11 Position*/
#define SDH_PRESET_SDR12_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_SDR12_R_RSVD_13_11_Pos)     /*!< SDH_T::PRESET_SDR12_R: RSVD_13_11 Mask */

#define SDH_PRESET_SDR12_R_DRV_SEL_VAL_Pos (14)                                            /*!< SDH_T::PRESET_SDR12_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_SDR12_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_SDR12_R_DRV_SEL_VAL_Pos)   /*!< SDH_T::PRESET_SDR12_R: DRV_SEL_VAL Mask*/

#define SDH_PRESET_SDR25_R_FREQ_SEL_VAL_Pos (0)                                            /*!< SDH_T::PRESET_SDR25_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_SDR25_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_SDR25_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR25_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_SDR25_R_CLK_GEN_SEL_VAL_Pos (10)                                        /*!< SDH_T::PRESET_SDR25_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_SDR25_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_SDR25_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR25_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_SDR25_R_RSVD_13_11_Pos (11)                                             /*!< SDH_T::PRESET_SDR25_R: RSVD_13_11 Position*/
#define SDH_PRESET_SDR25_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_SDR25_R_RSVD_13_11_Pos)     /*!< SDH_T::PRESET_SDR25_R: RSVD_13_11 Mask */

#define SDH_PRESET_SDR25_R_DRV_SEL_VAL_Pos (14)                                            /*!< SDH_T::PRESET_SDR25_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_SDR25_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_SDR25_R_DRV_SEL_VAL_Pos)   /*!< SDH_T::PRESET_SDR25_R: DRV_SEL_VAL Mask*/

#define SDH_PRESET_SDR50_R_FREQ_SEL_VAL_Pos (0)                                            /*!< SDH_T::PRESET_SDR50_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_SDR50_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_SDR50_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR50_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_SDR50_R_CLK_GEN_SEL_VAL_Pos (10)                                        /*!< SDH_T::PRESET_SDR50_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_SDR50_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_SDR50_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR50_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_SDR50_R_RSVD_13_11_Pos (11)                                             /*!< SDH_T::PRESET_SDR50_R: RSVD_13_11 Position*/
#define SDH_PRESET_SDR50_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_SDR50_R_RSVD_13_11_Pos)     /*!< SDH_T::PRESET_SDR50_R: RSVD_13_11 Mask */

#define SDH_PRESET_SDR50_R_DRV_SEL_VAL_Pos (14)                                            /*!< SDH_T::PRESET_SDR50_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_SDR50_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_SDR50_R_DRV_SEL_VAL_Pos)   /*!< SDH_T::PRESET_SDR50_R: DRV_SEL_VAL Mask*/

#define SDH_PRESET_SDR104_R_FREQ_SEL_VAL_Pos (0)                                           /*!< SDH_T::PRESET_SDR104_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_SDR104_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_SDR104_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR104_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_SDR104_R_CLK_GEN_SEL_VAL_Pos (10)                                       /*!< SDH_T::PRESET_SDR104_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_SDR104_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_SDR104_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR104_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_SDR104_R_RSVD_13_11_Pos (11)                                            /*!< SDH_T::PRESET_SDR104_R: RSVD_13_11 Position*/
#define SDH_PRESET_SDR104_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_SDR104_R_RSVD_13_11_Pos)   /*!< SDH_T::PRESET_SDR104_R: RSVD_13_11 Mask*/

#define SDH_PRESET_SDR104_R_DRV_SEL_VAL_Pos (14)                                           /*!< SDH_T::PRESET_SDR104_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_SDR104_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_SDR104_R_DRV_SEL_VAL_Pos) /*!< SDH_T::PRESET_SDR104_R: DRV_SEL_VAL Mask*/

#define SDH_PRESET_DDR50_R_FREQ_SEL_VAL_Pos (0)                                            /*!< SDH_T::PRESET_DDR50_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_DDR50_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_DDR50_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_DDR50_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_DDR50_R_CLK_GEN_SEL_VAL_Pos (10)                                        /*!< SDH_T::PRESET_DDR50_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_DDR50_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_DDR50_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_DDR50_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_DDR50_R_RSVD_13_11_Pos (11)                                             /*!< SDH_T::PRESET_DDR50_R: RSVD_13_11 Position*/
#define SDH_PRESET_DDR50_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_DDR50_R_RSVD_13_11_Pos)     /*!< SDH_T::PRESET_DDR50_R: RSVD_13_11 Mask */

#define SDH_PRESET_DDR50_R_DRV_SEL_VAL_Pos (14)                                            /*!< SDH_T::PRESET_DDR50_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_DDR50_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_DDR50_R_DRV_SEL_VAL_Pos)   /*!< SDH_T::PRESET_DDR50_R: DRV_SEL_VAL Mask*/

#define SDH_PRESET_UHS2_R_FREQ_SEL_VAL_Pos (0)                                             /*!< SDH_T::PRESET_UHS2_R: FREQ_SEL_VAL Position*/
#define SDH_PRESET_UHS2_R_FREQ_SEL_VAL_Msk (0x3fful << SDH_PRESET_UHS2_R_FREQ_SEL_VAL_Pos) /*!< SDH_T::PRESET_UHS2_R: FREQ_SEL_VAL Mask*/

#define SDH_PRESET_UHS2_R_CLK_GEN_SEL_VAL_Pos (10)                                         /*!< SDH_T::PRESET_UHS2_R: CLK_GEN_SEL_VAL Position*/
#define SDH_PRESET_UHS2_R_CLK_GEN_SEL_VAL_Msk (0x1ul << SDH_PRESET_UHS2_R_CLK_GEN_SEL_VAL_Pos) /*!< SDH_T::PRESET_UHS2_R: CLK_GEN_SEL_VAL Mask*/

#define SDH_PRESET_UHS2_R_RSVD_13_11_Pos (11)                                              /*!< SDH_T::PRESET_UHS2_R: RSVD_13_11 Position*/
#define SDH_PRESET_UHS2_R_RSVD_13_11_Msk (0x7ul << SDH_PRESET_UHS2_R_RSVD_13_11_Pos)       /*!< SDH_T::PRESET_UHS2_R: RSVD_13_11 Mask  */

#define SDH_PRESET_UHS2_R_DRV_SEL_VAL_Pos (14)                                             /*!< SDH_T::PRESET_UHS2_R: DRV_SEL_VAL Position*/
#define SDH_PRESET_UHS2_R_DRV_SEL_VAL_Msk (0x3ul << SDH_PRESET_UHS2_R_DRV_SEL_VAL_Pos)     /*!< SDH_T::PRESET_UHS2_R: DRV_SEL_VAL Mask */

#define SDH_ADMA_ID_LOW_R_ADMA_ID_LOW_Pos (0)                                              /*!< SDH_T::ADMA_ID_LOW_R: ADMA_ID_LOW Position*/
#define SDH_ADMA_ID_LOW_R_ADMA_ID_LOW_Msk (0xfffffffful << SDH_ADMA_ID_LOW_R_ADMA_ID_LOW_Pos) /*!< SDH_T::ADMA_ID_LOW_R: ADMA_ID_LOW Mask */

#define SDH_CQCAP_ITCFVAL_Pos            (0)                                               /*!< SDH_T::CQCAP: ITCFVAL Position         */
#define SDH_CQCAP_ITCFVAL_Msk            (0x3fful << SDH_CQCAP_ITCFVAL_Pos)                /*!< SDH_T::CQCAP: ITCFVAL Mask             */

#define SDH_CQCAP_CQCCAP_RSVD1_Pos       (10)                                              /*!< SDH_T::CQCAP: CQCCAP_RSVD1 Position    */
#define SDH_CQCAP_CQCCAP_RSVD1_Msk       (0x3ul << SDH_CQCAP_CQCCAP_RSVD1_Pos)             /*!< SDH_T::CQCAP: CQCCAP_RSVD1 Mask        */

#define SDH_CQCAP_ITCFMUL_Pos            (12)                                              /*!< SDH_T::CQCAP: ITCFMUL Position         */
#define SDH_CQCAP_ITCFMUL_Msk            (0xful << SDH_CQCAP_ITCFMUL_Pos)                  /*!< SDH_T::CQCAP: ITCFMUL Mask             */

#define SDH_CQCAP_CQCCAP_RSVD2_Pos       (16)                                              /*!< SDH_T::CQCAP: CQCCAP_RSVD2 Position    */
#define SDH_CQCAP_CQCCAP_RSVD2_Msk       (0xffful << SDH_CQCAP_CQCCAP_RSVD2_Pos)           /*!< SDH_T::CQCAP: CQCCAP_RSVD2 Mask        */

#define SDH_CQCAP_CRYPTO_SUPPORT_Pos     (28)                                              /*!< SDH_T::CQCAP: CRYPTO_SUPPORT Position  */
#define SDH_CQCAP_CRYPTO_SUPPORT_Msk     (0x1ul << SDH_CQCAP_CRYPTO_SUPPORT_Pos)           /*!< SDH_T::CQCAP: CRYPTO_SUPPORT Mask      */

#define SDH_CQCAP_CQCCAP_RSVD3_Pos       (29)                                              /*!< SDH_T::CQCAP: CQCCAP_RSVD3 Position    */
#define SDH_CQCAP_CQCCAP_RSVD3_Msk       (0x7ul << SDH_CQCAP_CQCCAP_RSVD3_Pos)             /*!< SDH_T::CQCAP: CQCCAP_RSVD3 Mask        */

#define SDH_P_EMBEDDED_CNTRL_REG_OFFSET_ADDR_Pos (0)                                     /*!< SDH_T::P_EMBEDDED_CNTRL: REG_OFFSET_ADDR Position*/
#define SDH_P_EMBEDDED_CNTRL_REG_OFFSET_ADDR_Msk (0xffful << SDH_P_EMBEDDED_CNTRL_REG_OFFSET_ADDR_Pos) /*!< SDH_T::P_EMBEDDED_CNTRL: REG_OFFSET_ADDR Mask*/

#define SDH_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR_Pos (0)                                 /*!< SDH_T::P_VENDOR_SPECIFIC_AREA: REG_OFFSET_ADDR Position*/
#define SDH_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR_Msk (0xffful << SDH_P_VENDOR_SPECIFIC_AREA_REG_OFFSET_ADDR_Pos) /*!< SDH_T::P_VENDOR_SPECIFIC_AREA: REG_OFFSET_ADDR Mask*/

#define SDH_P_VENDOR2_SPECIFIC_AREA_REG_OFFSET_ADDR_Pos (0)                                /*!< SDH_T::P_VENDOR2_SPECIFIC_AREA: REG_OFFSET_ADDR Position*/
#define SDH_P_VENDOR2_SPECIFIC_AREA_REG_OFFSET_ADDR_Msk (0xfffful << SDH_P_VENDOR2_SPECIFIC_AREA_REG_OFFSET_ADDR_Pos) /*!< SDH_T::P_VENDOR2_SPECIFIC_AREA: REG_OFFSET_ADDR Mask*/

#define SDH_HOST_CNTRL_VERS_R_SPEC_VERSION_NUM_Pos (0)                                     /*!< SDH_T::HOST_CNTRL_VERS_R: SPEC_VERSION_NUM Position*/
#define SDH_HOST_CNTRL_VERS_R_SPEC_VERSION_NUM_Msk (0xfful << SDH_HOST_CNTRL_VERS_R_SPEC_VERSION_NUM_Pos) /*!< SDH_T::HOST_CNTRL_VERS_R: SPEC_VERSION_NUM Mask*/

#define SDH_HOST_CNTRL_VERS_R_VENDOR_VERSION_NUM_Pos (8)                                   /*!< SDH_T::HOST_CNTRL_VERS_R: VENDOR_VERSION_NUM Position*/
#define SDH_HOST_CNTRL_VERS_R_VENDOR_VERSION_NUM_Msk (0xfful << SDH_HOST_CNTRL_VERS_R_VENDOR_VERSION_NUM_Pos) /*!< SDH_T::HOST_CNTRL_VERS_R: VENDOR_VERSION_NUM Mask*/

#define SDH_SLOT_INTR_STATUS_R_INTR_SLOT_Pos (0)                                           /*!< SDH_T::SLOT_INTR_STATUS_R: INTR_SLOT Position*/
#define SDH_SLOT_INTR_STATUS_R_INTR_SLOT_Msk (0xfful << SDH_SLOT_INTR_STATUS_R_INTR_SLOT_Pos) /*!< SDH_T::SLOT_INTR_STATUS_R: INTR_SLOT Mask*/

#define SDH_MSHC_VER_ID_R_MSHC_VER_ID_Pos (0)                                              /*!< SDH_T::MSHC_VER_ID_R: MSHC_VER_ID Position*/
#define SDH_MSHC_VER_ID_R_MSHC_VER_ID_Msk (0xfffffffful << SDH_MSHC_VER_ID_R_MSHC_VER_ID_Pos) /*!< SDH_T::MSHC_VER_ID_R: MSHC_VER_ID Mask */

#define SDH_MSHC_VER_TYPE_R_MSHC_VER_TYPE_Pos (0)                                          /*!< SDH_T::MSHC_VER_TYPE_R: MSHC_VER_TYPE Position*/
#define SDH_MSHC_VER_TYPE_R_MSHC_VER_TYPE_Msk (0xfffffffful << SDH_MSHC_VER_TYPE_R_MSHC_VER_TYPE_Pos) /*!< SDH_T::MSHC_VER_TYPE_R: MSHC_VER_TYPE Mask*/

#define SDH_MSHC_CTRL_R_CMD_CONFLICT_CHECK_Pos (0)                                         /*!< SDH_T::MSHC_CTRL_R: CMD_CONFLICT_CHECK Position*/
#define SDH_MSHC_CTRL_R_CMD_CONFLICT_CHECK_Msk (0x1ul << SDH_MSHC_CTRL_R_CMD_CONFLICT_CHECK_Pos) /*!< SDH_T::MSHC_CTRL_R: CMD_CONFLICT_CHECK Mask*/

#define SDH_MSHC_CTRL_R_RSVD1_Pos        (1)                                               /*!< SDH_T::MSHC_CTRL_R: RSVD1 Position     */
#define SDH_MSHC_CTRL_R_RSVD1_Msk        (0x7ul << SDH_MSHC_CTRL_R_RSVD1_Pos)              /*!< SDH_T::MSHC_CTRL_R: RSVD1 Mask         */

#define SDH_MSHC_CTRL_R_SW_CG_DIS_Pos    (4)                                               /*!< SDH_T::MSHC_CTRL_R: SW_CG_DIS Position */
#define SDH_MSHC_CTRL_R_SW_CG_DIS_Msk    (0x1ul << SDH_MSHC_CTRL_R_SW_CG_DIS_Pos)          /*!< SDH_T::MSHC_CTRL_R: SW_CG_DIS Mask     */

#define SDH_MBIU_CTRL_R_UNDEFL_INCR_EN_Pos (0)                                             /*!< SDH_T::MBIU_CTRL_R: UNDEFL_INCR_EN Position*/
#define SDH_MBIU_CTRL_R_UNDEFL_INCR_EN_Msk (0x1ul << SDH_MBIU_CTRL_R_UNDEFL_INCR_EN_Pos)   /*!< SDH_T::MBIU_CTRL_R: UNDEFL_INCR_EN Mask*/

#define SDH_MBIU_CTRL_R_BURST_INCR4_EN_Pos (1)                                             /*!< SDH_T::MBIU_CTRL_R: BURST_INCR4_EN Position*/
#define SDH_MBIU_CTRL_R_BURST_INCR4_EN_Msk (0x1ul << SDH_MBIU_CTRL_R_BURST_INCR4_EN_Pos)   /*!< SDH_T::MBIU_CTRL_R: BURST_INCR4_EN Mask*/

#define SDH_MBIU_CTRL_R_BURST_INCR8_EN_Pos (2)                                             /*!< SDH_T::MBIU_CTRL_R: BURST_INCR8_EN Position*/
#define SDH_MBIU_CTRL_R_BURST_INCR8_EN_Msk (0x1ul << SDH_MBIU_CTRL_R_BURST_INCR8_EN_Pos)   /*!< SDH_T::MBIU_CTRL_R: BURST_INCR8_EN Mask*/

#define SDH_MBIU_CTRL_R_BURST_INCR16_EN_Pos (3)                                            /*!< SDH_T::MBIU_CTRL_R: BURST_INCR16_EN Position*/
#define SDH_MBIU_CTRL_R_BURST_INCR16_EN_Msk (0x1ul << SDH_MBIU_CTRL_R_BURST_INCR16_EN_Pos) /*!< SDH_T::MBIU_CTRL_R: BURST_INCR16_EN Mask*/

#define SDH_MBIU_CTRL_R_RSVD_Pos         (4)                                               /*!< SDH_T::MBIU_CTRL_R: RSVD Position      */
#define SDH_MBIU_CTRL_R_RSVD_Msk         (0xful << SDH_MBIU_CTRL_R_RSVD_Pos)               /*!< SDH_T::MBIU_CTRL_R: RSVD Mask          */

#define SDH_EMMC_CTRL_R_CARD_IS_EMMC_Pos (0)                                               /*!< SDH_T::EMMC_CTRL_R: CARD_IS_EMMC Position*/
#define SDH_EMMC_CTRL_R_CARD_IS_EMMC_Msk (0x1ul << SDH_EMMC_CTRL_R_CARD_IS_EMMC_Pos)       /*!< SDH_T::EMMC_CTRL_R: CARD_IS_EMMC Mask  */

#define SDH_EMMC_CTRL_R_DISABLE_DATA_CRC_CHK_Pos (1)                                       /*!< SDH_T::EMMC_CTRL_R: DISABLE_DATA_CRC_CHK Position*/
#define SDH_EMMC_CTRL_R_DISABLE_DATA_CRC_CHK_Msk (0x1ul << SDH_EMMC_CTRL_R_DISABLE_DATA_CRC_CHK_Pos) /*!< SDH_T::EMMC_CTRL_R: DISABLE_DATA_CRC_CHK Mask*/

#define SDH_EMMC_CTRL_R_EMMC_RST_N_Pos   (2)                                               /*!< SDH_T::EMMC_CTRL_R: EMMC_RST_N Position*/
#define SDH_EMMC_CTRL_R_EMMC_RST_N_Msk   (0x1ul << SDH_EMMC_CTRL_R_EMMC_RST_N_Pos)         /*!< SDH_T::EMMC_CTRL_R: EMMC_RST_N Mask    */

#define SDH_EMMC_CTRL_R_EMMC_RST_N_OE_Pos (3)                                              /*!< SDH_T::EMMC_CTRL_R: EMMC_RST_N_OE Position*/
#define SDH_EMMC_CTRL_R_EMMC_RST_N_OE_Msk (0x1ul << SDH_EMMC_CTRL_R_EMMC_RST_N_OE_Pos)     /*!< SDH_T::EMMC_CTRL_R: EMMC_RST_N_OE Mask */

#define SDH_EMMC_CTRL_R_RSVD_Pos         (11)                                              /*!< SDH_T::EMMC_CTRL_R: RSVD Position      */
#define SDH_EMMC_CTRL_R_RSVD_Msk         (0x1ful << SDH_EMMC_CTRL_R_RSVD_Pos)              /*!< SDH_T::EMMC_CTRL_R: RSVD Mask          */

#define SDH_BOOT_CTRL_R_MAN_BOOT_EN_Pos  (0)                                               /*!< SDH_T::BOOT_CTRL_R: MAN_BOOT_EN Position*/
#define SDH_BOOT_CTRL_R_MAN_BOOT_EN_Msk  (0x1ul << SDH_BOOT_CTRL_R_MAN_BOOT_EN_Pos)        /*!< SDH_T::BOOT_CTRL_R: MAN_BOOT_EN Mask   */

#define SDH_BOOT_CTRL_R_RSVD_6_1_Pos     (1)                                               /*!< SDH_T::BOOT_CTRL_R: RSVD_6_1 Position  */
#define SDH_BOOT_CTRL_R_RSVD_6_1_Msk     (0x3ful << SDH_BOOT_CTRL_R_RSVD_6_1_Pos)          /*!< SDH_T::BOOT_CTRL_R: RSVD_6_1 Mask      */

#define SDH_BOOT_CTRL_R_VALIDATE_BOOT_Pos (7)                                              /*!< SDH_T::BOOT_CTRL_R: VALIDATE_BOOT Position*/
#define SDH_BOOT_CTRL_R_VALIDATE_BOOT_Msk (0x1ul << SDH_BOOT_CTRL_R_VALIDATE_BOOT_Pos)     /*!< SDH_T::BOOT_CTRL_R: VALIDATE_BOOT Mask */

#define SDH_BOOT_CTRL_R_BOOT_ACK_ENABLE_Pos (8)                                            /*!< SDH_T::BOOT_CTRL_R: BOOT_ACK_ENABLE Position*/
#define SDH_BOOT_CTRL_R_BOOT_ACK_ENABLE_Msk (0x1ul << SDH_BOOT_CTRL_R_BOOT_ACK_ENABLE_Pos) /*!< SDH_T::BOOT_CTRL_R: BOOT_ACK_ENABLE Mask*/

#define SDH_BOOT_CTRL_R_RSVD_11_9_Pos    (9)                                               /*!< SDH_T::BOOT_CTRL_R: RSVD_11_9 Position */
#define SDH_BOOT_CTRL_R_RSVD_11_9_Msk    (0x7ul << SDH_BOOT_CTRL_R_RSVD_11_9_Pos)          /*!< SDH_T::BOOT_CTRL_R: RSVD_11_9 Mask     */

#define SDH_BOOT_CTRL_R_BOOT_TOUT_CNT_Pos (12)                                             /*!< SDH_T::BOOT_CTRL_R: BOOT_TOUT_CNT Position*/
#define SDH_BOOT_CTRL_R_BOOT_TOUT_CNT_Msk (0xful << SDH_BOOT_CTRL_R_BOOT_TOUT_CNT_Pos)     /*!< SDH_T::BOOT_CTRL_R: BOOT_TOUT_CNT Mask */

#define SDH_AT_CTRL_R_AT_EN_Pos          (0)                                               /*!< SDH_T::AT_CTRL_R: AT_EN Position       */
#define SDH_AT_CTRL_R_AT_EN_Msk          (0x1ul << SDH_AT_CTRL_R_AT_EN_Pos)                /*!< SDH_T::AT_CTRL_R: AT_EN Mask           */

#define SDH_AT_CTRL_R_CI_SEL_Pos         (1)                                               /*!< SDH_T::AT_CTRL_R: CI_SEL Position      */
#define SDH_AT_CTRL_R_CI_SEL_Msk         (0x1ul << SDH_AT_CTRL_R_CI_SEL_Pos)               /*!< SDH_T::AT_CTRL_R: CI_SEL Mask          */

#define SDH_AT_CTRL_R_SWIN_TH_EN_Pos     (2)                                               /*!< SDH_T::AT_CTRL_R: SWIN_TH_EN Position  */
#define SDH_AT_CTRL_R_SWIN_TH_EN_Msk     (0x1ul << SDH_AT_CTRL_R_SWIN_TH_EN_Pos)           /*!< SDH_T::AT_CTRL_R: SWIN_TH_EN Mask      */

#define SDH_AT_CTRL_R_RPT_TUNE_ERR_Pos   (3)                                               /*!< SDH_T::AT_CTRL_R: RPT_TUNE_ERR Position*/
#define SDH_AT_CTRL_R_RPT_TUNE_ERR_Msk   (0x1ul << SDH_AT_CTRL_R_RPT_TUNE_ERR_Pos)         /*!< SDH_T::AT_CTRL_R: RPT_TUNE_ERR Mask    */

#define SDH_AT_CTRL_R_SW_TUNE_EN_Pos     (4)                                               /*!< SDH_T::AT_CTRL_R: SW_TUNE_EN Position  */
#define SDH_AT_CTRL_R_SW_TUNE_EN_Msk     (0x1ul << SDH_AT_CTRL_R_SW_TUNE_EN_Pos)           /*!< SDH_T::AT_CTRL_R: SW_TUNE_EN Mask      */

#define SDH_AT_CTRL_R_RSDV2_Pos          (5)                                               /*!< SDH_T::AT_CTRL_R: RSDV2 Position       */
#define SDH_AT_CTRL_R_RSDV2_Msk          (0x7ul << SDH_AT_CTRL_R_RSDV2_Pos)                /*!< SDH_T::AT_CTRL_R: RSDV2 Mask           */

#define SDH_AT_CTRL_R_WIN_EDGE_SEL_Pos   (8)                                               /*!< SDH_T::AT_CTRL_R: WIN_EDGE_SEL Position*/
#define SDH_AT_CTRL_R_WIN_EDGE_SEL_Msk   (0xful << SDH_AT_CTRL_R_WIN_EDGE_SEL_Pos)         /*!< SDH_T::AT_CTRL_R: WIN_EDGE_SEL Mask    */

#define SDH_AT_CTRL_R_RSDV3_Pos          (12)                                              /*!< SDH_T::AT_CTRL_R: RSDV3 Position       */
#define SDH_AT_CTRL_R_RSDV3_Msk          (0xful << SDH_AT_CTRL_R_RSDV3_Pos)                /*!< SDH_T::AT_CTRL_R: RSDV3 Mask           */

#define SDH_AT_CTRL_R_TUNE_CLK_STOP_EN_Pos (16)                                            /*!< SDH_T::AT_CTRL_R: TUNE_CLK_STOP_EN Position*/
#define SDH_AT_CTRL_R_TUNE_CLK_STOP_EN_Msk (0x1ul << SDH_AT_CTRL_R_TUNE_CLK_STOP_EN_Pos)   /*!< SDH_T::AT_CTRL_R: TUNE_CLK_STOP_EN Mask*/

#define SDH_AT_CTRL_R_PRE_CHANGE_DLY_Pos (17)                                              /*!< SDH_T::AT_CTRL_R: PRE_CHANGE_DLY Position*/
#define SDH_AT_CTRL_R_PRE_CHANGE_DLY_Msk (0x3ul << SDH_AT_CTRL_R_PRE_CHANGE_DLY_Pos)       /*!< SDH_T::AT_CTRL_R: PRE_CHANGE_DLY Mask  */

#define SDH_AT_CTRL_R_POST_CHANGE_DLY_Pos (19)                                             /*!< SDH_T::AT_CTRL_R: POST_CHANGE_DLY Position*/
#define SDH_AT_CTRL_R_POST_CHANGE_DLY_Msk (0x3ul << SDH_AT_CTRL_R_POST_CHANGE_DLY_Pos)     /*!< SDH_T::AT_CTRL_R: POST_CHANGE_DLY Mask */

#define SDH_AT_CTRL_R_SWIN_TH_VAL_Pos    (24)                                              /*!< SDH_T::AT_CTRL_R: SWIN_TH_VAL Position */
#define SDH_AT_CTRL_R_SWIN_TH_VAL_Msk    (0xful << SDH_AT_CTRL_R_SWIN_TH_VAL_Pos)          /*!< SDH_T::AT_CTRL_R: SWIN_TH_VAL Mask     */

#define SDH_AT_STAT_R_CENTER_PH_CODE_Pos (0)                                               /*!< SDH_T::AT_STAT_R: CENTER_PH_CODE Position*/
#define SDH_AT_STAT_R_CENTER_PH_CODE_Msk (0xfful << SDH_AT_STAT_R_CENTER_PH_CODE_Pos)      /*!< SDH_T::AT_STAT_R: CENTER_PH_CODE Mask  */

#define SDH_AT_STAT_R_R_EDGE_PH_CODE_Pos (8)                                               /*!< SDH_T::AT_STAT_R: R_EDGE_PH_CODE Position*/
#define SDH_AT_STAT_R_R_EDGE_PH_CODE_Msk (0xfful << SDH_AT_STAT_R_R_EDGE_PH_CODE_Pos)      /*!< SDH_T::AT_STAT_R: R_EDGE_PH_CODE Mask  */

#define SDH_AT_STAT_R_L_EDGE_PH_CODE_Pos (16)                                              /*!< SDH_T::AT_STAT_R: L_EDGE_PH_CODE Position*/
#define SDH_AT_STAT_R_L_EDGE_PH_CODE_Msk (0xfful << SDH_AT_STAT_R_L_EDGE_PH_CODE_Pos)      /*!< SDH_T::AT_STAT_R: L_EDGE_PH_CODE Mask  */

#define SDH_AT_STAT_R_RSDV1_Pos          (24)                                              /*!< SDH_T::AT_STAT_R: RSDV1 Position       */
#define SDH_AT_STAT_R_RSDV1_Msk          (0xfful << SDH_AT_STAT_R_RSDV1_Pos)               /*!< SDH_T::AT_STAT_R: RSDV1 Mask           */

#define SDH_EMBEDDED_CTRL_NUM_CLK_PIN_Pos (0)                                              /*!< SDH_T::EMBEDDED_CTRL: NUM_CLK_PIN Position*/
#define SDH_EMBEDDED_CTRL_NUM_CLK_PIN_Msk (0x7ul << SDH_EMBEDDED_CTRL_NUM_CLK_PIN_Pos)     /*!< SDH_T::EMBEDDED_CTRL: NUM_CLK_PIN Mask */

#define SDH_EMBEDDED_CTRL_RSVD_3_Pos     (3)                                               /*!< SDH_T::EMBEDDED_CTRL: RSVD_3 Position  */
#define SDH_EMBEDDED_CTRL_RSVD_3_Msk     (0x1ul << SDH_EMBEDDED_CTRL_RSVD_3_Pos)           /*!< SDH_T::EMBEDDED_CTRL: RSVD_3 Mask      */

#define SDH_EMBEDDED_CTRL_NUM_INT_PIN_Pos (4)                                              /*!< SDH_T::EMBEDDED_CTRL: NUM_INT_PIN Position*/
#define SDH_EMBEDDED_CTRL_NUM_INT_PIN_Msk (0x3ul << SDH_EMBEDDED_CTRL_NUM_INT_PIN_Pos)     /*!< SDH_T::EMBEDDED_CTRL: NUM_INT_PIN Mask */

#define SDH_EMBEDDED_CTRL_RSVD_7_6_Pos   (6)                                               /*!< SDH_T::EMBEDDED_CTRL: RSVD_7_6 Position*/
#define SDH_EMBEDDED_CTRL_RSVD_7_6_Msk   (0x3ul << SDH_EMBEDDED_CTRL_RSVD_7_6_Pos)         /*!< SDH_T::EMBEDDED_CTRL: RSVD_7_6 Mask    */

#define SDH_EMBEDDED_CTRL_BUS_WIDTH_PRESET_Pos (8)                                         /*!< SDH_T::EMBEDDED_CTRL: BUS_WIDTH_PRESET Position*/
#define SDH_EMBEDDED_CTRL_BUS_WIDTH_PRESET_Msk (0x7ful << SDH_EMBEDDED_CTRL_BUS_WIDTH_PRESET_Pos) /*!< SDH_T::EMBEDDED_CTRL: BUS_WIDTH_PRESET Mask*/

#define SDH_EMBEDDED_CTRL_RSVD_15_Pos    (15)                                              /*!< SDH_T::EMBEDDED_CTRL: RSVD_15 Position */
#define SDH_EMBEDDED_CTRL_RSVD_15_Msk    (0x1ul << SDH_EMBEDDED_CTRL_RSVD_15_Pos)          /*!< SDH_T::EMBEDDED_CTRL: RSVD_15 Mask     */

#define SDH_EMBEDDED_CTRL_CLK_PIN_SEL_Pos (16)                                             /*!< SDH_T::EMBEDDED_CTRL: CLK_PIN_SEL Position*/
#define SDH_EMBEDDED_CTRL_CLK_PIN_SEL_Msk (0x7ul << SDH_EMBEDDED_CTRL_CLK_PIN_SEL_Pos)     /*!< SDH_T::EMBEDDED_CTRL: CLK_PIN_SEL Mask */

#define SDH_EMBEDDED_CTRL_RSVD_19_Pos    (19)                                              /*!< SDH_T::EMBEDDED_CTRL: RSVD_19 Position */
#define SDH_EMBEDDED_CTRL_RSVD_19_Msk    (0x1ul << SDH_EMBEDDED_CTRL_RSVD_19_Pos)          /*!< SDH_T::EMBEDDED_CTRL: RSVD_19 Mask     */

#define SDH_EMBEDDED_CTRL_INT_PIN_SEL_Pos (20)                                             /*!< SDH_T::EMBEDDED_CTRL: INT_PIN_SEL Position*/
#define SDH_EMBEDDED_CTRL_INT_PIN_SEL_Msk (0x7ul << SDH_EMBEDDED_CTRL_INT_PIN_SEL_Pos)     /*!< SDH_T::EMBEDDED_CTRL: INT_PIN_SEL Mask */

#define SDH_EMBEDDED_CTRL_RSVD_23_Pos    (23)                                              /*!< SDH_T::EMBEDDED_CTRL: RSVD_23 Position */
#define SDH_EMBEDDED_CTRL_RSVD_23_Msk    (0x1ul << SDH_EMBEDDED_CTRL_RSVD_23_Pos)          /*!< SDH_T::EMBEDDED_CTRL: RSVD_23 Mask     */

#define SDH_EMBEDDED_CTRL_BACK_END_PWR_CTRL_Pos (24)                                       /*!< SDH_T::EMBEDDED_CTRL: BACK_END_PWR_CTRL Position*/
#define SDH_EMBEDDED_CTRL_BACK_END_PWR_CTRL_Msk (0x7ful << SDH_EMBEDDED_CTRL_BACK_END_PWR_CTRL_Pos) /*!< SDH_T::EMBEDDED_CTRL: BACK_END_PWR_CTRL Mask*/

#define SDH_EMBEDDED_CTRL_RSVD_31_Pos    (31)                                              /*!< SDH_T::EMBEDDED_CTRL: RSVD_31 Position */
#define SDH_EMBEDDED_CTRL_RSVD_31_Msk    (0x1ul << SDH_EMBEDDED_CTRL_RSVD_31_Pos)          /*!< SDH_T::EMBEDDED_CTRL: RSVD_31 Mask     */

/**@}*/ /* SDH_CONST */
/**@}*/ /* end of SDH register group */


/**@}*/ /* end of REGISTER group */

#endif /* __SDH_REG_H__ */
