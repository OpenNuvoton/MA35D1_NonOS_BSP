/**************************************************************************//**
 * @file     gmac_reg.h
 * @brief    GMAC register header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __GMAC_REG_H__
#define __GMAC_REG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**  @addtogroup REGISTER Control Register
   @{
*/

/**  @addtogroup Gigabit Media Access Controller (GMAC)
                 Memory Mapped Structure for GMAC
@{ */

typedef struct {
    /**
     * @var GMAC_T::Config
     * Offset: 0x00  Register 0 (MAC Config Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |PRELEN    |Preamble Length for Transmit Frames
     * |        |          |These bits control the number of preamble bytes that are added to the beginning of every Transmit frame
     * |        |          |The preamble reduction occurs only when the MAC is operating in the full-duplex mode.
     * |        |          |2'b00 = 7 bytes of preamble
     * |        |          |2'b01 = 5 byte of preamble
     * |        |          |2'b10 = 3 bytes of preamble
     * |        |          |2'b11 = reserved
     * |[2]     |RE        |Receiver Enable
     * |        |          |When this bit is set, the receiver state machine of the MAC is enabled for receiving frames from the GMII or MII
     * |        |          |When this bit is reset, the MAC receive state machine is disabled after the completion of the reception of the current frame, and does not receive any further frames from the GMII or MII.
     * |[3]     |TE        |Transmitter Enable
     * |        |          |When this bit is set, the transmit state machine of the MAC is enabled for transmission on the GMII or MII
     * |        |          |When this bit is reset, the MAC transmit state machine is disabled after the completion of the transmission of the current frame, and does not transmit any further frames.
     * |[4]     |DC        |Deferral Check
     * |        |          |When this bit is set, the deferral check function is enabled in the MAC
     * |        |          |The MAC issues a Frame Abort status, along with the excessive deferral error bit set in the transmit frame status, when the transmit state machine is deferred for more than 24,288 bit times in the 10 or 100 Mbps mode.
     * |        |          |If the MAC is configured for 1000 Mbps operation or if the Jumbo frame mode is enabled in the 10 or 100 Mbps mode, the threshold for deferral is 155,680 bits times
     * |        |          |Deferral begins when the transmitter is ready to transmit, but it is prevented because of an active carrier sense signal (CRS) on GMII or MII.
     * |        |          |The defer time is not cumulative
     * |        |          |For example, if the transmitter defers for 10,000 bit times because the CRS signal is active and then the CRS signal becomes inactive, the transmitter transmits and collision happens
     * |        |          |Because of collision, the transmitter needs to back off and then defer again after back off completion
     * |        |          |In such a scenario, the deferral timer is reset to 0 and it is restarted
     * |        |          |When this bit is reset, the deferral check function is disabled and the MAC defers until the CRS signal goes inactive
     * |        |          |This bit is applicable only in the half-duplex mode.
     * |[6:5]   |BL        |Back-Off Limit
     * |        |          |The Back-Off limit determines the random integer number (r) of slot time delays (4,096 bit times for 1000 Mbps and 512 bit times for 10/100 Mbps) for which the MAC waits before rescheduling a transmission attempt during retries after a collision
     * |        |          |This bit is applicable only in the half-duplex mode.
     * |        |          |00 = k is the min of (n, 10).
     * |        |          |01 = k is the min of (n, 8).
     * |        |          |10 = k is the min of (n, 4).
     * |        |          |11 = k is the min of (n, 1).
     * |        |          |where n = retransmission attempt
     * |        |          |The random integer r takes the value in the range 0 <= r < kth power of 2.
     * |[7]     |ACS       |Automatic Pad or CRC Stripping
     * |        |          |When this bit is set, the MAC strips the Pad or FCS field on the incoming frames only if the value of the length field is less than 1,536 bytes
     * |        |          |All received frames with length field greater than or equal to 1,536 bytes are passed to the application without stripping the Pad or FCS field.
     * |        |          |When this bit is reset, the MAC passes all incoming frames, without modifying them, to the Host.
     * |[8]     |LUD       |Link Up or Down
     * |        |          |This bit indicates whether the link is up or down during the transmission of configuration in the RGMII interface:
     * |        |          |0 = Link Down
     * |        |          |1 = Link Up
     * |[9]     |DR        |Disable Retry
     * |        |          |When this bit is set, the MAC attempts only one transmission
     * |        |          |When a collision occurs on the GMII or MII interface, the MAC ignores the current frame transmission and reports a Frame Abort with excessive collision error in the transmit frame status.
     * |        |          |When this bit is reset, the MAC attempts retries based on the settings of the BL field (Bits [6:5])
     * |        |          |This bit is applicable only in the half-duplex mode.
     * |[10]    |IPC       |Checksum Offload
     * |        |          |When this bit is set, the MAC calculates the 16-bit one's complement of the one's complement sum of all received Ethernet frame payloads
     * |        |          |It also checks whether the IPv4 Header checksum (assumed to be bytes 2526 or 2930 (VLAN-tagged) of the received Ethernet frame) is correct for the received frame and gives the status in the receive status word
     * |        |          |The MAC also appends the 16-bit checksum calculated for the IP header datagram payload (bytes after the IPv4 header) and appends it to the Ethernet frame transferred to the application (when Type 2 COE is deselected).
     * |        |          |When this bit is reset, this function is disabled.
     * |        |          |When Type 2 COE is selected, this bit, when set, enables the IPv4 header checksum checking and IPv4 or IPv6 TCP, UDP, or ICMP payload checksum checking
     * |        |          |When this bit is reset, the COE function in the receiver is disabled and the corresponding PCE and IP HCE status bits are always cleared.
     * |[11]    |DM        |Duplex Mode
     * |        |          |When this bit is set, the MAC operates in the full-duplex mode where it can transmit and receive simultaneously
     * |[12]    |LM        |Loopback Mode
     * |        |          |When this bit is set, the MAC operates in the loopback mode at GMII or MII
     * |        |          |The (G)MII Receive clock input (clk_rx_i) is required for the loopback to work properly, because the Transmit clock is not looped-back internally.
     * |[13]    |DO        |Disable Receive Own
     * |        |          |When this bit is set, the MAC disables the reception of frames when the phy_txen_o is asserted in the half-duplex mode
     * |        |          |When this bit is reset, the MAC receives all packets that are given by the PHY while transmitting
     * |        |          |This bit is not applicable if the MAC is operating in the full-duplex mode
     * |[14]    |FES       |Speed
     * |        |          |This bit selects the speed in the RMII or RGMII interface
     * |        |          |0 = 10 Mbps
     * |        |          |1 = 100 Mbps
     * |[15]    |PS        |Port Select
     * |        |          |This bit selects the Ethernet line speed:
     * |        |          |0 = For 1000 Mbps operations
     * |        |          |1 = For 10 or 100 Mbps operations
     * |        |          |In 10 or 100 Mbps operations, this bit, along with FES bit, selects the exact line speed
     * |        |          |The mac_portselect_o signal reflects the value of this bit.
     * |[16]    |DCRS      |Disable Carrier Sense During Transmission
     * |        |          |When set high, this bit makes the MAC transmitter ignore the (G)MII CRS signal during frame transmission in the half-duplex mode
     * |        |          |This request results in no errors generated because of Loss of Carrier or No Carrier during such transmission
     * |        |          |When this bit is low, the MAC transmitter generates such errors because of Carrier Sense and can even abort the transmissions
     * |[19:17] |IFG       |Inter-Frame Gap
     * |        |          |These bits control the minimum IFG between frames during transmission.
     * |        |          |000 =96 bit times
     * |        |          |001 =88 bit times
     * |        |          |010 = 80 bit times
     * |        |          |...
     * |        |          |111 = 40 bit times
     * |        |          |In the half-duplex mode, the minimum IFG can be configured only for 64 bit times (IFG = 100)
     * |        |          |Lower values are not considered
     * |        |          |In the 1000-Mbps mode, the minimum IFG supported is 80 bit times (and above).
     * |[20]    |JE        |Jumbo Frame Enable
     * |        |          |When this bit is set, the MAC allows Jumbo frames of 9,018 bytes (9,022 bytes for VLAN tagged frames) without reporting a giant frame error in the receive frame status.
     * |[21]    |BE        |Frame Burst Enable
     * |        |          |When this bit is set, the MAC allows frame bursting during transmission in the GMII half-duplex mode.
     * |[22]    |JD        |Jabber Disable
     * |        |          |When this bit is set, the MAC disables the jabber timer on the transmitter
     * |        |          |The MAC can transfer frames of up to 16,384 bytes
     * |        |          |When this bit is reset, the MAC cuts off the transmitter if the application sends out more than 2,048 bytes of data (10,240 if JE is set high) during transmission.
     * |[23]    |WD        |Watchdog Disable
     * |        |          |When this bit is set, the MAC disables the watchdog timer on the receiver
     * |        |          |The MAC can receive frames of up to 16,384 bytes
     * |        |          |When this bit is reset, the MAC does not allow a receive frame which more than 2,048 bytes (10,240 if JE is set high) or the value programmed in Register 55 (Watchdog Timeout Register).
     * |        |          |The MAC cuts off any bytes received after the watchdog limit number of bytes.
     * |[24]    |TC        |Transmit Configuration in RGMII
     * |        |          |When set, this bit enables the transmission of duplex mode, link speed, and link up or down information to the PHY in the RGMII port
     * |        |          |When this bit is reset, no such information is driven to the PHY
     * |[25]    |CST       |CRC Stripping for Type Frames
     * |        |          |When this bit is set, the last 4 bytes (FCS) of all frames of Ether type (Length/Type field greater than or equal to 1,536) are stripped and dropped before forwarding the frame to the application
     * |        |          |This function is not valid when the IP Checksum Engine (Type 1) is enabled in the MAC receiver
     * |        |          |This function is valid when Type 2 Checksum Offload Engine is enabled.
     * |[27]    |TWOKPE    |IEEE 802.3as Support for 2K Packets
     * |        |          |When set, the MAC considers all frames, with up to 2,000 bytes length, as normal packets
     * |        |          |When Bit 20 (JE) is not set, the MAC considers all received frames of size more than 2 Kbytes as Giant frames
     * |        |          |When this bit is reset and Bit 20 (JE) is not set, the MAC considers all received frames of size more than 1,518 bytes (1,522 bytes for tagged) as Giant frames
     * |        |          |When Bit 20 is set, setting this bit has no effect on Giant Frame status.
     * |[30:28] |SARC      |Source Address Insertion or Replacement Control
     * |        |          |This field controls the source address insertion or replacement for all transmitted frames
     * |        |          |Bit 30 specifies which MAC Address register (0 or 1) is used for source address insertion or replacement based on the values of Bits [29:28]:
     * |        |          |* 2'b0x = The input signals mti_sa_ctrl_i and ati_sa_ctrl_i control the SA field generation. * 2'b10 =
     * |        |          |u00B7 If Bit 30 is set to 0, the MAC inserts the content of the MAC Address 0 registers (registers 16 and 17) in the SA field of all transmitted frames.
     * |        |          |u00B7 If Bit 30 is set to 1, the MAC inserts the content of the MAC Address 1 registers (registers 18 and 19) in the SA field of all transmitted frames.
     * |        |          |* 2'b11 =
     * |        |          |u00B7 If Bit 30 is set to 0, the MAC replaces the content of the MAC Address 0 registers (registers 16 and 17) in the SA field of all transmitted frames.
     * |        |          |u00B7 If Bit 30 is set to 1, the MAC replaces the content of the MAC Address 1 registers (registers 18 and 19) in the SA field of all transmitted frames.
     * |        |          |Note:
     * |        |          |u00B7 Changes to this field take effect only on the start of a frame
     * |        |          |If you write this register field when a frame is being transmitted, only the subsequent frame can use the updated value, that is, the current frame does not use the updated value
     * @var GMAC_T::FrameFilter
     * Offset: 0x04  Register 1 (MAC Frame Filter)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PR        |Promiscuous Mode
     * |        |          |When this bit is set, the Address Filter module passes all incoming frames regardless of its destination or source address
     * |        |          |The SA or DA Filter Fails status bits of the Receive Status Word are always cleared when PR is set.
     * |[3]     |DAIF      |DA Inverse Filtering
     * |        |          |When this bit is set, the Address Check block operates in inverse filtering mode for the DA address comparison for both unicast and multicast frames.
     * |        |          |When reset, normal filtering of frames is performed.
     * |[4]     |PM        |Pass All Multicast
     * |        |          |When set, this bit indicates that all received frames with a multicast destination address (first bit in the destination address field is '1') are passed.
     * |        |          |When reset, filtering of multicast frame depends on HMC bit.
     * |[5]     |DBF       |Disable Broadcast Frames
     * |        |          |When this bit is set, the AFM module filters all incoming broadcast frames
     * |        |          |In addition, it overrides all other filter settings.
     * |        |          |When this bit is reset, the AFM module passes all received broadcast frames.
     * |[7:6]   |PCF       |Pass Control Frames
     * |        |          |These bits control the forwarding of all control frames (including unicast and multicast PAUSE frames).
     * |        |          |00 = MAC filters all control frames from reaching the application.
     * |        |          |01 = MAC forwards all control frames except PAUSE control frames to application even if they fail the Address filter.
     * |        |          |10 = MAC forwards all control frames to application even if they fail the Address Filter.
     * |        |          |11 = MAC forwards control frames that pass the Address Filter.
     * |        |          |The following conditions should be true for the PAUSE control frames processing:
     * |        |          |u00B7 Condition 1: The MAC is in the full-duplex mode and flow control is enabled by setting Bit 2 (RFE) of Register 6 (Flow Control Register) to 1.
     * |        |          |u00B7 Condition 2: The destination address (DA) of the received frame matches the special multicast address or the MAC Address 0 when Bit 3 (UP) of the Register 6 (Flow Control Register) is set.
     * |        |          |u00B7 Condition 3: The Type field of the received frame is 0x8808 and the OPCODE field is 0x0001.
     * |        |          |Note:
     * |        |          |This field should be set to 01 only when the Condition 1 is true, that is, the MAC is programmed to operate in the full-duplex mode and the RFE bit is enabled
     * |        |          |Otherwise, the PAUSE frame filtering may be inconsistent
     * |        |          |When Condition 1 is false, the PAUSE frames are considered as generic control frames
     * |        |          |Therefore, to pass all control frames (including PAUSE control frames) when the full-duplex mode and flow control is not enabled, you should set the PCF field to 10 or 11 (as required by the application).
     * |[8]     |SAIF      |SA Inverse Filtering
     * |        |          |When this bit is set, the Address Check block operates in inverse filtering mode for the SA address comparison
     * |        |          |The frames whose SA matches the SA registers are marked as failing the SA Address filter.
     * |        |          |When this bit is reset, frames whose SA does not match the SA registers are marked as failing the SA Address filter.
     * |[9]     |SAF       |Source Address Filter Enable
     * |        |          |When this bit is set, the MAC compares the SA field of the received frames with the values programmed in the enabled SA registers
     * |        |          |If the comparison fails, the MAC drops the frame
     * |        |          |When this bit is reset, the MAC forwards the received frame to the application with updated SAF bit of the Rx Status depending on the SA address comparison.
     * |        |          |Note: According to the IEEE specification, Bit 47 of the SA is reserved and set to 0
     * |        |          |However, in GMAC, the MAC compares all 48 bits
     * |        |          |The software driver should take this into consideration while programming the MAC address registers for SA.
     * |[16]    |VTFE      |VLAN Tag Filter Enable
     * |        |          |When set, this bit enables the MAC to drop VLAN tagged frames that do not match the VLAN Tag comparison.
     * |        |          |When reset, the MAC forwards all frames irrespective of the match status of the VLAN Tag.
     * |[31]    |RA        |Receive All
     * |        |          |When this bit is set, the MAC Receiver module passes all received frames, irrespective of whether they pass the address filter or not, to the Application
     * |        |          |The result of the SA or DA filtering is updated (pass or fail) in the corresponding bits in the Receive Status Word.
     * |        |          |When this bit is reset, the Receiver module passes only those frames to the Application that pass the SA or DA address filter.
     * @var GMAC_T::GmiiAddr
     * Offset: 0x10  Register 4 (GMII Address Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |GB        |GMII Busy
     * |        |          |This bit should read logic 0 before writing to Register 4 and Register 5
     * |        |          |During a PHY register access, the software sets this bit to 1'b1 to indicate that a Read or Write access is in progress.
     * |        |          |Register 5 is invalid until this bit is cleared by the MAC
     * |        |          |Therefore, Register 5 (GMII Data) should be kept valid until the MAC clears this bit during a PHY Write operation
     * |        |          |Similarly for a read operation, the contents of Register 5 are not valid until this bit is cleared.
     * |        |          |The subsequent read or write operation should happen only after the previous operation is complete
     * |        |          |Because there is no acknowledgment from the PHY to MAC after a read or write operation is completed, there is no change in the functionality of this bit even when the PHY is not present.
     * |[1]     |GW        |GMII Write
     * |        |          |When set, this bit indicates to the PHY that this is a Write operation using the GMII Data register
     * |        |          |If this bit is not set, it indicates that this is a Read operation, that is, placing the data in the GMII Data register.
     * |[5:2]   |CR        |CSR Clock Range
     * |        |          |The CSR Clock Range selection determines the frequency of the MDC clock according to the CSR clock frequency used in your design
     * |        |          |The suggested range of CSR clock frequency applicable for each value (when Bit[5] = 0) ensures that the MDC clock is approximately between the frequency range 1.0 MHz - 2.5 MHz.
     * |        |          |0000 = The frequency of the CSR clock is 60-100 MHz and the MDC clock is CSR clock/42.
     * |        |          |0001 = The frequency of the CSR clock is 100-150 MHz and the MDC clock is CSR clock/62.
     * |        |          |0010 = The frequency of the CSR clock is 20-35 MHz and the MDC clock is CSR clock/16.
     * |        |          |0011 = The frequency of the CSR clock is 35-60 MHz and the MDC clock is CSR clock/26.
     * |        |          |0100 = The frequency of the CSR clock is 150-250 MHz and the MDC clock is CSR clock/102.
     * |        |          |0100 = The frequency of the CSR clock is 250-300 MHz and the MDC clock is CSR clock/124.
     * |        |          |0110 and 0111 = Reserved
     * |        |          |When Bit 5 is set, you can achieve MDC clock of frequency higher than the IEEE 802.3 specified frequency limit of 2.5 MHz and program a clock divider of lower value
     * |        |          |For example, when CSR clock is of 100 MHz frequency and you program these bits as 1010, then the resultant MDC clock is of 12.5 MHz which is outside the limit of IEEE 802.3 specified range.
     * |        |          |Program the following values only if the interfacing chips support faster MDC clocks:
     * |        |          |1000 = CSR clock/4
     * |        |          |1001 = CSR clock/6
     * |        |          |1010 = CSR clock/8
     * |        |          |1011 = CSR clock/10
     * |        |          |1100 = CSR clock/12
     * |        |          |1101 = CSR clock/14
     * |        |          |1110 = CSR clock/16
     * |        |          |1111 = CSR clock/18
     * |[10:6]  |GR        |GMII Register
     * |        |          |These bits select the desired GMII register in the selected PHY device.
     * |[15:11] |PA        |Physical Layer Address
     * |        |          |This field indicates which of the 32 possible PHY devices are being accessed.
     * @var GMAC_T::GmiiData
     * Offset: 0x14  Register 5 (GMII Data Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |GD        |GMII Data
     * |        |          |This field contains the 16-bit data value read from the PHY after a Management Read operation or the 16-bit data value to be written to the PHY before a Management Write operation.
     * @var GMAC_T::FlowControl
     * Offset: 0x18  Register 6 (Flow Control Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FCA_BPA   |Flow Control Busy or Backpressure Activate
     * |        |          |This bit initiates a Pause Control frame in the full-duplex mode and activates the backpressure function in the half-duplex mode if the TFE bit is set.
     * |        |          |In the full-duplex mode, this bit should be read as 1'b0 before writing to the Flow Control register
     * |        |          |To initiate a Pause control frame, the Application must set this bit to 1'b1
     * |        |          |During a transfer of the Control Frame, this bit continues to be set to signify that a frame transmission is in progress
     * |        |          |After the completion of Pause control frame transmission, the MAC resets this bit to 1'b0
     * |        |          |The Flow Control register should not be written to until this bit is cleared.
     * |        |          |In the half-duplex mode, when this bit is set (and TFE is set), then backpressure is asserted by the MAC
     * |        |          |During backpressure, when the MAC receives a new frame, the transmitter starts sending a JAM pattern resulting in a collision
     * |        |          |This control register bit is logically ORed with the mti_flowctrl_i input signal for the backpressure function
     * |        |          |When the MAC is configured for the full-duplex mode, the BPA is automatically disabled.
     * |[1]     |TFE       |Transmit Flow Control Enable
     * |        |          |In the full-duplex mode, when this bit is set, the MAC enables the flow control operation to transmit Pause frames
     * |        |          |When this bit is reset, the flow control operation in the MAC is disabled, and the MAC does not transmit any Pause frames.
     * |        |          |In half-duplex mode, when this bit is set, the MAC enables the back-pressure operation
     * |        |          |When this bit is reset, the back-pressure feature is disabled.
     * |[2]     |RFE       |Receive Flow Control Enable
     * |        |          |When this bit is set, the MAC decodes the received Pause frame and disables its transmitter for a specified (Pause) time
     * |        |          |When this bit is reset, the decode function of the Pause frame is disabled.
     * |[3]     |UP        |Unicast Pause Frame Detect
     * |        |          |A pause frame is processed when it has the unique multicast address specified in the IEEE Std 802.3
     * |        |          |When this bit is set, the MAC can also detect Pause frames with unicast address of the station
     * |        |          |This unicast address should be as specified in the MAC Address0 High Register and MAC Address0 Low Register.
     * |        |          |When this bit is reset, the MAC only detects Pause frames with unique multicast address.
     * |        |          |Note: The MAC does not process a Pause frame if the multicast address of received frame is different from the unique multicast address.
     * |[5:4]   |PLT       |Pause Low Threshold
     * |        |          |This field configures the threshold of the PAUSE timer at which the input flow control signal mti_flowctrl_i (or sbd_flowctrl_i) is checked for automatic retransmission of PAUSE Frame.
     * |        |          |The threshold values should be always less than the Pause Time configured in Bits[31:16]
     * |        |          |For example, if PT = 100H (256 slot-times), and PLT = 01, then a second PAUSE frame is automatically transmitted if the mti_flowctrl_i signal is asserted at 228 (256 - 28) slot times after the first PAUSE frame is transmitted.
     * |        |          |The following list provides the threshold values for different values:
     * |        |          |00 = The threshold is Pause time minus 4 slot times (PT - 4 slot times).
     * |        |          |01 = The threshold is Pause time minus 28 slot times (PT - 28 slot times).
     * |        |          |10 = The threshold is Pause time minus 144 slot times (PT - 144 slot times).
     * |        |          |11 = The threshold is Pause time minus 256 slot times (PT - 256 slot times).
     * |        |          |The slot time is defined as the time taken to transmit 512 bits (64 bytes) on the GMII or MII interface.
     * |[7]     |DZPQ      |Disable Zero-Quanta Pause
     * |        |          |When this bit is set, it disables the automatic generation of the Zero-Quanta Pause Control frames on the de-assertion of the flow-control signal from the FIFO layer (MTL or external sideband flow control signal sbd_flowctrl_i/mti_flowctrl_i)
     * |        |          |When this bit is reset, normal operation with automatic Zero-Quanta Pause Control frame generation is enabled.
     * |[31:16] |PT        |Pause Time
     * |        |          |This field holds the value to be used in the Pause Time field in the transmit control frame
     * |        |          |Consecutive writes to this register should be performed only after at least four clock cycles in the destination clock domain.
     * @var GMAC_T::VlanTag
     * Offset: 0x1C  Register 7 (VLAN Tag Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |VL        |VLAN Tag Identifier for Receive Frames
     * |        |          |This field contains the 802.1Q VLAN tag to identify the VLAN frames and is compared to the 15th and 16th bytes of the frames being received for VLAN frames
     * |        |          |The following list describes the bits of this field:
     * |        |          |Bits [15:13] = User Priority
     * |        |          |Bit 12:Canonical Format Indicator (CFI) or Drop Eligible Indicator (DEI)
     * |        |          |Bits[11:0] = VLAN tag's VLAN Identifier (VID) field
     * |        |          |When the ETV bit is set, only the VID (Bits[11:0]) is used for comparison
     * |        |          |If VL (VL[11:0] if ETV is set) is all zeros, the MAC does not check the fifteenth and 16th bytes for VLAN tag comparison, and declares all frames with a Type field value of 0x8100 or 0x88a8 as VLAN frames.
     * |[16]    |ETV       |Enable 12-Bit VLAN Tag Comparison
     * |        |          |When this bit is set, a 12-bit VLAN identifier is used for comparing and filtering instead of the complete 16-bit VLAN tag
     * |        |          |Bits [11:0] of VLAN tag are compared with the corresponding field in the received VLAN-tagged frame
     * |        |          |Similarly, when enabled, only 12 bits of the VLAN tag in the received frame are used for hash-based VLAN filtering.
     * |        |          |When this bit is reset, all 16 bits of the 15th and 16th bytes of the received VLAN frame are used for comparison and VLAN hash filtering.
     * |[17]    |VTIM      |VLAN Tag Inverse Match Enable
     * |        |          |When set, this bit enables the VLAN Tag inverse matching
     * |        |          |The frames that do not have matching VLAN Tag are marked as matched.
     * |        |          |When reset, this bit enables the VLAN Tag perfect matching
     * |        |          |The frames with matched VLAN Tag are marked as matched.
     * |[18]    |ESVL      |Enable S-VLAN
     * |        |          |When this bit is set, the MAC transmitter and receiver also consider the S-VLAN (Type = 0x88A8) frames as valid VLAN tagged frames.
     * @var GMAC_T::Version
     * Offset: 0x20  Register 8 (Version Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * @var GMAC_T::Debug
     * Offset: 0x24  Register 9 (Debug Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RPESTS    |MAC GMII or MII Receive Protocol Engine Status
     * |        |          |When high, this bit indicates that the MAC GMII or MII receive protocol engine is actively receiving data and not in IDLE state.
     * |[2:1]   |RFCFCSTS  |MAC Receive Frame Controller FIFO Status
     * |        |          |When high, this field indicates the active state of the small FIFO Read and Write controllers of the MAC Receive Frame Controller Module.
     * |[4]     |RWCSTS    |MTL Rx FIFO Write Controller Active Status
     * |        |          |When high, this bit indicates that the MTL Rx FIFO Write Controller is active and is transferring a received frame to the FIFO.
     * |[6:5]   |RRCSTS    |MTL Rx FIFO Read Controller State
     * |        |          |This field gives the state of the Rx FIFO read Controller:
     * |        |          |00 = IDLE state
     * |        |          |01 = Reading frame data
     * |        |          |10 = Reading frame status (or timestamp)
     * |        |          |11 = Flushing the frame data and status
     * |[9:8]   |RXFSTS    |MTL Rx FIFO Fill-level Status
     * |        |          |This field gives the status of the fill-level of the Rx FIFO:
     * |        |          |00 = Rx FIFO Empty
     * |        |          |01 = Rx FIFO fill level is below the flow-control deactivate threshold
     * |        |          |10 = Rx FIFO fill level is above the flow-control activate threshold
     * |        |          |11 = Rx FIFO Full
     * |[16]    |TPESTS    |MAC GMII or MII Transmit Protocol Engine Status
     * |        |          |When high, this bit indicates that the MAC GMII or MII transmit protocol engine is actively transmitting data and is not in the IDLE state.
     * |[18:17] |TFCSTS    |MAC Transmit Frame Controller Status
     * |        |          |This field indicates the state of the MAC Transmit Frame Controller module:
     * |        |          |00 = IDLE state
     * |        |          |01 = Waiting for Status of previous frame or IFG or backoff period to be over
     * |        |          |10 = Generating and transmitting a PAUSE control frame (in the full-duplex mode)
     * |        |          |11 = Transferring input frame for transmission
     * |[19]    |TXPAUSED  |MAC transmitter in PAUSE
     * |        |          |When high, this bit indicates that the MAC transmitter is in the PAUSE condition (in the full-duplex only mode) and hence does not schedule any frame for transmission.
     * |[21:20] |TRCSTS    |MTL Tx FIFO Read Controller Status
     * |        |          |This field indicates the state of the Tx FIFO Read Controller:
     * |        |          |00 = IDLE state
     * |        |          |01 = READ state (transferring data to MAC transmitter)
     * |        |          |10 = Waiting for TxStatus from MAC transmitter
     * |        |          |11 = Writing the received TxStatus or flushing the Tx FIFO
     * |[22]    |TWCSTS    |MTL Tx FIFO Write Controller Active Status
     * |        |          |When high, this bit indicates that the MTL Tx FIFO Write Controller is active and transferring data to the Tx FIFO.
     * |[24]    |TXFSTS    |MTL Tx FIFO Not Empty Status
     * |        |          |When high, this bit indicates that the MTL Tx FIFO is not empty and some data is left for transmission.
     * |[25]    |TXSTSFSTS |MTL TxStatus FIFO Full Status
     * |        |          |When high, this bit indicates that the MTL TxStatus FIFO is full
     * |        |          |Therefore, the MTL cannot accept any more frames for transmission.
     * @var GMAC_T::PmtCtrlStatus
     * Offset: 0x2C  Register 11 (PMT Control and Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PWRDWN    |Power Down
     * |        |          |When set, the MAC receiver drops all received frames until it receives the expected magic packet or wake-up frame
     * |        |          |This bit is then self-cleared and the power-down mode is disabled
     * |        |          |The Software can also clear this bit before the expected magic packet or wake-up frame is received
     * |        |          |The frames, received by the MAC after this bit is cleared, are forwarded to the application
     * |        |          |This bit must only be set when the Magic Packet Enable, Global Unicast, or Wake-Up Frame Enable bit is set high.
     * |        |          |Note: You can gate-off the CSR clock during the power-down mode
     * |        |          |However, when the CSR clock is gated-off, you cannot perform any read or write operations on this register
     * |        |          |Therefore, the Software cannot clear this bit.
     * |[1]     |MGKPKTEN  |Magic Packet Enable
     * |        |          |When set, enables generation of a power management event because of magic packet reception.
     * |[5]     |MGKPRCVD  |Magic Packet Received (read only)
     * |        |          |When set, this bit indicates that the power management event is generated because of the reception of a magic packet
     * |        |          |This bit is cleared by a Read into this register.
     * @var GMAC_T::LpiCtrlStatus
     * Offset: 0x30  Register 12 (LPI Control and Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TLPIEN    |Transmit LPI Entry (read only)
     * |        |          |When set, this bit indicates that the MAC Transmitter has entered the LPI state because of the setting of the LPIEN bit
     * |        |          |This bit is cleared by a read into this register.
     * |[1]     |TLPIEX    |Transmit LPI Exit (read only)
     * |        |          |When set, this bit indicates that the MAC transmitter has exited the LPI state after the user has cleared the LPIEN bit and the LPI TW Timer has expired
     * |        |          |This bit is cleared by a read into this register.
     * |[2]     |RLPIEN    |Receive LPI Entry (read only)
     * |        |          |When set, this bit indicates that the MAC Receiver has received an LPI pattern and entered the LPI state
     * |        |          |This bit is cleared by a read into this register.
     * |        |          |Note: This bit may not get set if the MAC stops receiving the LPI pattern for a very short duration, such as, less than 3 clock cycles of CSR clock.
     * |[3]     |RLPIEX    |Receive LPI Exit (read only)
     * |        |          |When set, this bit indicates that the MAC Receiver has stopped receiving the LPI pattern on the GMII or MII interface, exited the LPI state, and resumed the normal reception
     * |        |          |This bit is cleared by a read into this register.
     * |        |          |Note: This bit may not get set if the MAC stops receiving the LPI pattern for a very short duration, such as, less than 3 clock cycles of CSR clock.
     * |[8]     |TLPIST    |Transmit LPI State (read only)
     * |        |          |When set, this bit indicates that the MAC is transmitting the LPI pattern on the GMII or MII interface.
     * |[9]     |RLPIST    |Receive LPI State (read only)
     * |        |          |When set, this bit indicates that the MAC is receiving the LPI pattern on the GMII or MII interface.
     * |[16]    |LPIEN     |LPI Enable
     * |        |          |When set, this bit instructs the MAC Transmitter to enter the LPI state
     * |        |          |When reset, this bit instructs the MAC to exit the LPI state and resume normal transmission.
     * |        |          |This bit is cleared when the LPITXA bit is set and the MAC exits the LPI state because of the arrival of a new packet for transmission.
     * |[17]    |PLS       |PHY Link Status
     * |        |          |This bit indicates the link status of the PHY
     * |        |          |The MAC Transmitter asserts the LPI pattern only when the link status is up (okay) at least for the time indicated by the LPI LS TIMER
     * |        |          |When set, the link is considered to be okay (up) and when reset, the link is considered to be down.
     * |[18]    |PLSEN     |PHY Link Status Enable
     * |        |          |This bit enables the link status received on the RGMII receive paths to be used for activating the LPI LS TIMER.
     * |        |          |When set, the MAC uses the link-status bits of Register 54 (RGMII Status Register) and Bit 17 (PLS) for the LPI LS Timer trigger
     * |        |          |When cleared, the MAC ignores the link-status bits of Register 54 and takes only the PLS bit.
     * |        |          |This bit is RO and reserved if you have not selected the RGMII PHY interface.
     * |[19]    |LPITXA    |LPI TX Automate
     * |        |          |This bit controls the behavior of the MAC when it is entering or coming out of the LPI mode on the transmit side
     * |        |          |If the LPITXA and LPIEN bits are set to 1, the MAC enters the LPI mode only after all outstanding frames (in the core) and pending frames (in the application interface) have been transmitted
     * |        |          |The MAC comes out of the LPI mode when the application sends any frame for transmission or the application issues a TX FIFO Flush command
     * |        |          |In addition, the MAC automatically clears the LPIEN bit when it exits the LPI state
     * |        |          |If TX FIFO Flush is set, in Bit 20 of Register 1006 (Operation Mode Register), when the MAC is in the LPI mode, the MAC exits the LPI mode.
     * |        |          |When this bit is 0, the LPIEN bit directly controls behavior of the MAC when it is entering or coming out of the LPI mode.
     * @var GMAC_T::LpiTimerCtrl
     * Offset: 0x34  Register 13 (LPI Timers Control Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TWT       |LPI TW Timer
     * |        |          |This field specifies the minimum time (in microseconds) for which the MAC waits after it stops transmitting the LPI pattern to the PHY and before it resumes the normal transmission
     * |        |          |The TLPIEX status bit is set after the expiry of this timer.
     * |[25:16] |LST       |LPI LS Timer
     * |        |          |This field specifies the minimum time (in milliseconds) for which the link status from the PHY should be up (OKAY) before the LPI pattern can be transmitted to the PHY
     * |        |          |The MAC does not transmit the LPI pattern even when the LPIEN bit is set unless the LPI LS Timer reaches the programmed terminal count
     * |        |          |The default value of the LPI LS Timer is 1000 (1 sec) as defined in the IEEE standard.
     * @var GMAC_T::IntStatus
     * Offset: 0x38  Register 14 (Interrupt Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RGSMIIIS  |RGMII Interrupt Status
     * |        |          |This bit is set because of any change in value of the Link Status of RGMII interface (Bit 3 in Register 54 (RGMII Status Register))
     * |        |          |This bit is cleared when you perform a read operation on the RGMII Status Register.
     * |[3]     |PMTIS     |PMT Interrupt Status
     * |        |          |This bit is set when a Magic packet or Wake-on-LAN frame is received in the power-down mode (see Bits 5 in the PMT Control and Status Register)
     * |        |          |This bit is cleared when Bit 5 is cleared because of a read operation to the PMT Control and Status register.
     * |[9]     |TSIS      |Timestamp Interrupt Status
     * |        |          |When the Advanced Timestamp feature is enabled, this bit is set when any of the following conditions is true:
     * |        |          |u00B7 The system time value equals or exceeds the value specified in the Target Time High and Low registers.
     * |        |          |u00B7 There is an overflow in the seconds register.
     * |        |          |u00B7 The Auxiliary snapshot trigger is asserted.
     * |        |          |This bit is cleared on reading Bit 0 of the Register 458 (Timestamp Status Register).
     * |        |          |If default Timestamping is enabled, when set, this bit indicates that the system time value is equal to or exceeds the value specified in the Target Time registers
     * |        |          |In this mode, this bit is cleared after the completion of the read of this bit
     * |        |          |In all other modes, this bit is reserved.
     * |[10]    |LPIIS     |LPI Interrupt Status
     * |        |          |When the Energy Efficient Ethernet feature is enabled, this bit is set for any LPI state entry or exit in the MAC Transmitter or Receiver
     * |        |          |This bit is cleared on reading Bit 0 of Register 12 (LPI Control and Status Register)
     * |        |          |In all other modes, this bit is reserved.
     * @var GMAC_T::IntMask
     * Offset: 0x3C  Register 15 (Interrupt Mask Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |RGSMIIIM  |RGMII Interrupt Mask
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of the RGMII Interrupt Status bit in Register 14 (Interrupt Status Register).
     * |[1]     |PCSLCHGIM |PCS Link Status Interrupt Mask (read only)
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of the PCS Link-status changed bit in Register 14 (Interrupt Status Register).
     * |[2]     |PCSANCIM  |PCS AN Completion Interrupt Mask (read only)
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of PCS Auto-negotiation complete bit in Register 14 (Interrupt Status Register)
     * |[3]     |PMTIM     |PMT Interrupt Mask
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of PMT Interrupt Status bit in Register 14 (Interrupt Status Register).
     * |[9]     |TSIM      |Timestamp Interrupt Mask
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of Timestamp Interrupt Status bit in Register 14 (Interrupt Status Register)
     * |        |          |This bit is valid only when IEEE1588 timestamping is enabled
     * |        |          |In all other modes, this bit is reserved.
     * |[10]    |LPIIM     |LPI Interrupt Mask
     * |        |          |When set, this bit disables the assertion of the interrupt signal because of the setting of the LPI Interrupt Status bit in Register 14 (Interrupt Status Register)
     * @var GMAC_T::Addr0High
     * Offset: 0x40  Register 16 (MAC Address0 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address0 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the first 6-byte MAC address
     * |        |          |The MAC uses this field for filtering the received frames and inserting the MAC address in the Transmit Flow Control (PAUSE) Frames.
     * |[31]    |AE        |Address Enable (read only)
     * |        |          |This bit is always set to 1.
     * @var GMAC_T::Addr0Low
     * Offset: 0x44  Register 17 (MAC Address0 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address0 [31:0]
     * |        |          |This field contains the lower 32 bits of the first 6-byte MAC address
     * |        |          |This is used by the MAC for filtering the received frames and inserting the MAC address in the Transmit Flow Control (PAUSE) Frames.
     * @var GMAC_T::Addr1High
     * Offset: 0x48  Register 18 (MAC Address1 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address1 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the second 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address1 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 18[15:8]
     * |        |          |Bit 28 = Register 18[7:0]
     * |        |          |Bit 27 = Register 19[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 19[7:0]
     * |        |          |You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address1[47:0] is used to compare with the SA fields of the received frame.
     * |        |          |When this bit is reset, the MAC Address1[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the second MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr1Low
     * Offset: 0x4C  Register 19 (MAC Address1 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address1 [31:0]
     * |        |          |This field contains the lower 32 bits of the second 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr2High
     * Offset: 0x50  Register 20 (MAC Address2 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address2 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the third 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address2 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 20[15:8]
     * |        |          |Bit 28 = Register 20[7:0]
     * |        |          |Bit 27 = Register 21[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 21[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address2[47:0] is used to compare with the SA fields of the received frame.
     * |        |          |When this bit is reset, the MAC Address2[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the third MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr2Low
     * Offset: 0x54  Register 21 (MAC Address2 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address2 [31:0]
     * |        |          |This field contains the lower 32 bits of the third 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr3High
     * Offset: 0x58  Register 22 (MAC Address3 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address3 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the fourth 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address3 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 22[15:8]
     * |        |          |Bit 28 = Register 22[7:0]
     * |        |          |Bit 27 = Register 23[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 23[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address3[47:0] is used to compare with the SA fields of the received frame.
     * |        |          |When this bit is reset, the MAC Address3[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the fourth MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr3Low
     * Offset: 0x5C  Register 23 (MAC Address3 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address3 [31:0]
     * |        |          |This field contains the lower 32 bits of the fourth 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr4High
     * Offset: 0x60  Register 24 (MAC Address4 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address4 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the fifth 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address4 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 24[15:8]
     * |        |          |Bit 28 = Register 24[7:0]
     * |        |          |Bit 27 = Register 25[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 25[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address4[47:0] is used to compare with the SA fields of the received frame.
     * |        |          |When this bit is reset, the MAC Address4[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the fifth MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr4Low
     * Offset: 0x64  Register 25 (MAC Address4 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address4 [31:0]
     * |        |          |This field contains the lower 32 bits of the fifth 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr5High
     * Offset: 0x68  Register 26 (MAC Address5 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address5 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the sixth 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address5 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 26[15:8]
     * |        |          |Bit 28 = Register 26[7:0]
     * |        |          |Bit 27 = Register 27[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 27[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address5[47:0] is used to compare with the SA fields of the received frame
     * |        |          |When this bit is reset, the MAC Address5[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the sixth MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr5Low
     * Offset: 0x6C  Register 27 (MAC Address5 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address5 [31:0]
     * |        |          |This field contains the lower 32 bits of the sixth 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr6High
     * Offset: 0x70  Register 28 (MAC Address6 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address6 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the seventh 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address6 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 28[15:8]
     * |        |          |Bit 28 = Register 28[7:0]
     * |        |          |Bit 27 = Register 29[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 29[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address6[47:0] is used to compare with the SA fields of the received frame
     * |        |          |When this bit is reset, the MAC Address6[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the seventh MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr6Low
     * Offset: 0x74  Register 29 (MAC Address6 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address6 [31:0]
     * |        |          |This field contains the lower 32 bits of the seventh 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr7High
     * Offset: 0x78  Register 30 (MAC Address7 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address7 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the eighth 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address6 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 30[15:8]
     * |        |          |Bit 28 = Register 30[7:0]
     * |        |          |Bit 27 = Register 31[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 31[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address7[47:0] is used to compare with the SA fields of the received frame
     * |        |          |When this bit is reset, the MAC Address7[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the eighth MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr7Low
     * Offset: 0x7C  Register 31 (MAC Address7 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address7 [31:0]
     * |        |          |This field contains the lower 32 bits of the eighth 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::Addr8High
     * Offset: 0x80  Register 32 (MAC Address8 High Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |ADDRHI    |MAC Address8 [47:32]
     * |        |          |This field contains the upper 16 bits (47:32) of the nineth 6-byte MAC address.
     * |[29:24] |MBC       |Mask Byte Control
     * |        |          |These bits are mask control bits for comparison of each of the MAC Address bytes
     * |        |          |When set high, the MAC does not compare the corresponding byte of received DA or SA with the contents of MAC Address8 registers
     * |        |          |Each bit controls the masking of the bytes as follows:
     * |        |          |Bit 29 = Register 32[15:8]
     * |        |          |Bit 28 = Register 32[7:0]
     * |        |          |Bit 27 = Register 33[31:24]
     * |        |          |...
     * |        |          |Bit 24 = Register 33[7:0]
     * |[30]    |SA        |Source Address
     * |        |          |When this bit is set, the MAC Address8[47:0] is used to compare with the SA fields of the received frame
     * |        |          |When this bit is reset, the MAC Address8[47:0] is used to compare with the DA fields of the received frame.
     * |[31]    |AE        |Address Enable
     * |        |          |When this bit is set, the address filter module uses the nineth MAC address for perfect filtering
     * |        |          |When this bit is reset, the address filter module ignores the address for filtering.
     * @var GMAC_T::Addr8Low
     * Offset: 0x84  Register 33 (MAC Address8 Low Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |ADDRLO    |MAC Address8 [31:0]
     * |        |          |This field contains the lower 32 bits of the nineth 6-byte MAC address
     * |        |          |The content of this field is undefined until loaded by the Application after the initialization process.
     * @var GMAC_T::RgmiiCtrlSts
     * Offset: 0xD8  Register 54 (RGMII Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LNKMOD    |Link Mode
     * |        |          |This bit indicates the current mode of operation of the link:
     * |        |          |1'b0 = Half-duplex mode
     * |        |          |1'b1 = Full-duplex mode
     * |[2:1]   |LNKSPEED  |Link Speed
     * |        |          |This bit indicates the current speed of the link:
     * |        |          |00 = 2.5 MHz
     * |        |          |01 = 25 MHz
     * |        |          |10 = 125 MHz
     * |[3]     |LNKSTS    |Link Status
     * |        |          |When set, this bit indicates that the link is up between the local PHY and the remote PHY
     * |        |          |When cleared, this bit indicates that the link is down between the local PHY and the remote PHY.
     * @var GMAC_T::Timeout
     * Offset: 0xDC  Register 55 (Watchdog Timeout Register) This register controls the watchdog timeout for received frames
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[13:0]  |WTO       |Watchdog Timeout
     * |        |          |When Bit 16 (PWE) is set and Bit 23 (WD) of Register 0 (MAC Configuration Register) is reset, this field is used as watchdog timeout for a received frame
     * |        |          |If the length of a received frame exceeds the value of this field, such frame is terminated and declared as an error frame.
     * |        |          |Note: When Bit 16 (PWE) is set, the value in this field should be more than 1,522 (0x05F2)
     * |        |          |Otherwise, the IEEE Std 802.3-specified valid tagged frames are declared as error frames and are dropped.
     * |[16]    |PWE       |Programmable Watchdog Enable
     * |        |          |When this bit is set and Bit 23 (WD) of Register 0 (MAC Configuration Register) is reset, the WTO field (Bits[13:0]) is used as watchdog timeout for a received frame
     * |        |          |When this bit is cleared, the watchdog timeout for a received frame is controlled by the setting of Bit 23 (WD) and Bit 20 (JE) in Register 0 (MAC Config Register).
     * @var GMAC_T::VlanIncRep
     * Offset: 0x584  Register 353 (VLAN Tag Inclusion or Replacement Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |VLT       |VLAN Tag for Transmit Frames
     * |        |          |This field contains the value of the VLAN tag to be inserted or replaced
     * |        |          |The value must only be changed when the transmit lines are inactive or during the initialization phase
     * |        |          |Bits[15:13] are the User Priority, Bit 12 is the CFI/DEI, and Bits[11:0] are the VLAN tag's VID field.
     * |[17:16] |VLC       |VLAN Tag Control in Transmit Frames
     * |        |          |2'b00 = No VLAN tag deletion, insertion, or replacement
     * |        |          |2'b01 = VLAN tag deletion
     * |        |          |The MAC removes the VLAN type (bytes 13 and 14) and VLAN tag (bytes 15 and 16) of all transmitted frames with VLAN tags.
     * |        |          |2'b10 = VLAN tag insertion
     * |        |          |The MAC inserts VLT in bytes 15 and 16 of the frame after inserting the Type value (0x8100/0x88a8) in bytes 13 and 14
     * |        |          |This operation is performed on all transmitted frames, irrespective of whether they already have a VLAN tag.
     * |        |          |2'b11 = VLAN tag replacement
     * |        |          |The MAC replaces VLT in bytes 15 and 16 of all VLAN-type transmitted frames (Bytes 13 and 14 are 0x8100/0x88a8).
     * |        |          |Note: Changes to this field take effect only on the start of a frame
     * |        |          |If you write this register field when a frame is being transmitted, only the subsequent frame can use the updated value, that is, the current frame does not use the updated value
     * |[18]    |VLP       |VLAN Priority Control
     * |        |          |When this bit is set, the control Bits [17:16] are used for VLAN deletion, insertion, or replacement
     * |        |          |When this bit is reset, the mti_vlan_ctrl_i control input is used, and Bits [17:16] are ignored.
     * |[19]    |CSVL      |C-VLAN or S-VLAN
     * |        |          |When this bit is set, S-VLAN type (0x88A8) is inserted or replaced in the 13th and 14th bytes of transmitted frames
     * |        |          |When this bit is reset, C-VLAN type (0x8100) is inserted or replaced in the transmitted frames.
     * @var GMAC_T::TSControl
     * Offset: 0x700  Register 448 (Timestamp Control Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TSENA     |Timestamp Enable
     * |        |          |When set, the timestamp is added for the transmit and receive frames
     * |        |          |When disabled, timestamp is not added for the transmit and receive frames and the Timestamp Generator is also suspended
     * |        |          |You need to initialize the Timestamp (system time) after enabling this mode
     * |        |          |On the receive side, the MAC processes the 1588 frames only if this bit is set.
     * |[1]     |TSCFUPDT  |Timestamp Fine or Coarse Update
     * |        |          |When set, this bit indicates that the system times update should be done using the fine update method
     * |        |          |When reset, it indicates the system timestamp update should be done using the Coarse method.
     * |[2]     |TSINIT    |Timestamp Initialize
     * |        |          |When set, the system time is initialized (overwritten) with the value specified in the Register 452 (System Time - Seconds Update Register) and Register 453 (System Time - Nanoseconds Update Register).
     * |        |          |This bit should be read zero before updating it
     * |        |          |This bit is reset when the initialization is complete
     * |        |          |The Timestamp Higher Word register can only be initialized.
     * |[3]     |TSUPDT    |Timestamp Update
     * |        |          |When set, the system time is updated (added or subtracted) with the value specified in Register 452 (System Time - Seconds Update Register) and Register 453 (System Time - Nanoseconds Update Register).
     * |        |          |This bit should be read zero before updating it
     * |        |          |This bit is reset when the update is completed in hardware
     * |        |          |The Timestamp Higher Word register is not updated.
     * |[4]     |TSTRIG    |Timestamp Interrupt Trigger Enable
     * |        |          |When set, the timestamp interrupt is generated when the System Time becomes greater than the value written in the Target Time register
     * |        |          |This bit is reset after the generation of the Timestamp Trigger Interrupt.
     * |[5]     |TSADDREG  |Addend Reg Update
     * |        |          |When set, the content of the Timestamp Addend register is updated in the PTP block for fine correction
     * |        |          |This is cleared when the update is completed
     * |        |          |This register bit should be zero before setting it.
     * |[8]     |TSENALL   |Enable Timestamp for All Frames
     * |        |          |When set, the timestamp snapshot is enabled for all frames received by the MAC.
     * |[9]     |TSCTRLSSR |Timestamp Digital or Binary Rollover Control
     * |        |          |When set, the Timestamp Low register rolls over after 0x3B9A_C9FF value (that is, 1 nanosecond accuracy) and increments the timestamp (High) seconds
     * |        |          |When reset, the rollover value of sub-second register is 0x7FFF_FFFF
     * |        |          |The sub-second increment has to be programmed correctly depending on the PTP reference clock frequency and the value of this bit.
     * |[10]    |TSVER2ENA |Enable PTP packet Processing for Version 2 Format
     * |        |          |When set, the PTP packets are processed using the 1588 version 2 format
     * |        |          |Otherwise, the PTP packets are processed using the version 1 format.
     * |[11]    |TSIPENA   |Enable Processing of PTP over Ethernet Frames
     * |        |          |When set, the MAC receiver processes the PTP packets encapsulated directly in the Ethernet frames
     * |        |          |When this bit is clear, the MAC ignores the PTP over Ethernet packets.
     * |[12]    |TSIPV6ENA |Enable Processing of PTP Frames Sent Over IPv6-UDP
     * |        |          |When set, the MAC receiver processes PTP packets encapsulated in UDP over IPv6 packets
     * |        |          |When this bit is clear, the MAC ignores the PTP transported over UDP-IPv6 packets.
     * |[13]    |TSIPV4ENA |Enable Processing of PTP Frames Sent over IPv4-UDP
     * |        |          |When set, the MAC receiver processes the PTP packets encapsulated in UDP over IPv4 packets
     * |        |          |When this bit is clear, the MAC ignores the PTP transported over UDP-IPv4 packets
     * |        |          |This bit is set by default.
     * |[14]    |TSEVNTENA |Enable Timestamp Snapshot for Event Messages
     * |        |          |When set, the timestamp snapshot is taken only for event messages (SYNC, Delay_Req, Pdelay_Req, or Pdelay_Resp)
     * |        |          |When reset, the snapshot is taken for all messages except Announce, Management, and Signaling.
     * |[15]    |TSMSTRENA |Enable Snapshot for Messages Relevant to Master
     * |        |          |When set, the snapshot is taken only for the messages relevant to the master node
     * |        |          |Otherwise, the snapshot is taken for the messages relevant to the slave node.
     * |[17:16] |SNAPTYPSEL|Select PTP packets for Taking Snapshots
     * |        |          |These bits along with Bits 15 and 14 decide the set of PTP packet types for which snapshot needs to be taken.
     * |[18]    |TSENMACADDR|Enable MAC address for PTP Frame Filtering
     * |        |          |When set, the DA MAC address (that matches any MAC Address register) is used to filter the PTP frames when PTP is directly sent over Ethernet.
     * @var GMAC_T::TSSubSecIncr
     * Offset: 0x704  Register 449 (Sub-Second Increment Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |SSINC     |Sub-second Increment Value
     * |        |          |The value programmed in this field is accumulated every clock cycle (of clk_ptp_i) with the contents of the sub-second register
     * |        |          |For example, when PTP clock is 50 MHz (period is 20 ns), you should program 20 (0x14) when the System Time-Nanoseconds register has an accuracy of 1 ns (TSCTRLSSR bit is set)
     * |        |          |When TSCTRLSSR is clear, the Nanoseconds register has a resolution of ~0.465ns
     * |        |          |In this case, you should program a value of 43 (0x2B) that is derived by 20ns/0.465.
     * @var GMAC_T::TSSec
     * Offset: 0x708  Register 450 (System Time - Seconds Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TSS       |Timestamp Second
     * |        |          |The value in this field indicates the current value in seconds of the System Time maintained by the MAC.
     * @var GMAC_T::TSNanosec
     * Offset: 0x70C  Register 451 (System Time - Nanoseconds Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[30:0]  |TSSS      |Timestamp Sub Seconds
     * |        |          |The value in this field has the sub second representation of time, with an accuracy of 0.46 ns
     * |        |          |When bit 9 (TSCTRLSSR) is set in Register 448 (Timestamp Control Register), each bit represents 1 ns and the maximum value is 0x3B9A_C9FF, after which it rolls-over to zero.
     * @var GMAC_T::TSSecUpdate
     * Offset: 0x710  Register 452 (System Time - Seconds Update Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TSS       |Timestamp Second
     * |        |          |The value in this field indicates the time in seconds to be initialized or added to the system time.
     * @var GMAC_T::TSNanosecUpdate
     * Offset: 0x714  Register 453 (System Time - Nanoseconds Update Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[30:0]  |TSSS      |Timestamp Sub Second
     * |        |          |The value in this field has the sub second representation of time, with an accuracy of 0.46 ns
     * |        |          |When bit 9 (TSCTRLSSR) is set in Register 448 (Timestamp Control Register), each bit represents 1 ns and the programmed value should not exceed 0x3B9A_C9FF.
     * |[31]    |ADDSUB    |Add or subtract time
     * |        |          |When this bit is set, the time value is subtracted with the contents of the update register
     * |        |          |When this bit is reset, the time value is added with the contents of the update register.
     * @var GMAC_T::TSAddend
     * Offset: 0x718  Register 454 (Timestamp Addend Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TSAR      |Timestamp Addend Register
     * |        |          |This field indicates the 32-bit time value to be added to the Accumulator register to achieve time synchronization.
     * @var GMAC_T::TSTargetSec
     * Offset: 0x71C  Register 455 (Target Time Seconds Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TSTR      |Target Time Seconds Register
     * |        |          |This register stores the time in seconds
     * |        |          |When the timestamp value matches or exceeds both Target Timestamp registers, then based on Bits [6:5] of Register 459 (PPS Control Register), the MAC starts or stops the PPS signal output and generates an interrupt (if enabled).
     * @var GMAC_T::TSTargetNanosec
     * Offset: 0x720  Register 456 (Target Time Nanoseconds Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[30:0]  |TTSLO     |Target Timestamp Low Register
     * |        |          |This register stores the time in (signed) nanoseconds
     * |        |          |When the value of the timestamp matches the both Target Timestamp registers, then based on the TRGTMODSEL0 field (Bits [6:5]) in Register 459 (PPS Control Register), the MAC starts or stops the PPS signal output and generates an interrupt (if enabled).
     * |        |          |This value should not exceed 0x3B9A_C9FF when TSCTRLSSR is set in the Timestamp control register
     * |        |          |The actual start or stop time of the PPS signal output may have an error margin up to one unit of sub-second increment value.
     * |[31]    |TRGTBUSY  |Target Time Register Busy (read only)
     * |        |          |The MAC sets this bit when the PPSCMD field (Bits[3:0]) in Register 459 (PPS Control Register) is programmed to 010 or 011
     * |        |          |Programming the PPSCMD field to 010 or 011, instructs the MAC to synchronize the Target Time Registers to the PTP clock domain.
     * |        |          |The MAC clears this bit after synchronizing the Target Time Registers to the PTP clock domain The application must not update the Target Time Registers when this bit is read as 1
     * |        |          |Otherwise, the synchronization of the previous programmed time gets corrupted
     * |        |          |This bit is reserved when the Enable Flexible Pulse-Per-Second Output feature is not selected.
     * @var GMAC_T::TSSecHigher
     * Offset: 0x724  Register 457 (System Time - Higher Word Seconds Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |TSHWR     |Timestamp Higher Word Register
     * |        |          |This field contains the most significant 16-bits of the timestamp seconds value
     * |        |          |The register is directly written to initialize the value
     * |        |          |This register is incremented when there is an overflow from the 32-bits of the System Time - Seconds register.
     * @var GMAC_T::TSStatus
     * Offset: 0x728  Register 458 (Timestamp Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TSSOVF    |Timestamp Seconds Overflow
     * |        |          |When set, this bit indicates that the seconds value of the timestamp (when supporting version 2 format) has overflowed beyond 32'hFFFF_FFFF.
     * |[1]     |TSTARGT   |Timestamp Target Time Reached
     * |        |          |When set, this bit indicates that the value of system time is greater or equal to the value specified in the Register 455 (Target Time Seconds Register) and Register 456 (Target Time Nanoseconds Register).
     * |[2]     |AUXTSTRIG |Auxiliary Timestamp Trigger Snapshot
     * |        |          |This bit is set high when the auxiliary snapshot is written to the FIFO
     * |        |          |This bit is valid only if the Enable IEEE 1588 Auxiliary Snapshot feature is selected.
     * |[3]     |TSTRGTERR |Timestamp Target Time Error
     * |        |          |This bit is set when the target time, being programmed in Target Time Registers, is already elapsed
     * |        |          |This bit is cleared when read by the application.
     * |[19:16] |ATSSTN    |Auxiliary Timestamp Snapshot Trigger Identifier
     * |        |          |These bits identify the Auxiliary trigger inputs for which the timestamp available in the Auxiliary Snapshot Register is applicable
     * |        |          |When more than one bit is set at the same time, it means that corresponding auxiliary triggers were sampled at the same clock
     * |        |          |These bits are applicable only if the number of Auxiliary snapshots is more than one
     * |        |          |One bit is assigned for each trigger as shown in the following list:
     * |        |          |Bit 16 = Auxiliary trigger 0
     * |        |          |Bit 17 = Auxiliary trigger 1
     * |        |          |Bit 18 = Auxiliary trigger 2
     * |        |          |Bit 19 = Auxiliary trigger 3
     * |        |          |The software can read this register to find the triggers that are set when the timestamp is taken.
     * @var GMAC_T::PPSCtrl
     * Offset: 0x72C  Register 459 (PPS Control Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |PPSCTRL_PPSCMD|PPSCTRL0 or PPSCMD0
     * |        |          |PPSCTRL0: PPS0 Output Frequency Control
     * |        |          |This field controls the frequency of the PPS0 output (ptp_pps_o[0]) signal
     * |        |          |The default value of PPSCTRL is 0000, and the PPS output is 1 pulse (of width clk_ptp_i) every second
     * |        |          |For other values of PPSCTRL, the PPS output becomes a generated clock of following frequencies:
     * |        |          |0000 = The binary rollover is 1 Hz, and the digital rollover is 0.5 Hz.
     * |        |          |0001 = The binary rollover is 2 Hz, and the digital rollover is 1 Hz.
     * |        |          |0010 = The binary rollover is 4 Hz, and the digital rollover is 2 Hz.
     * |        |          |0011 = The binary rollover is 8 Hz, and the digital rollover is 4 Hz.
     * |        |          |0100 = The binary rollover is 16 Hz, and the digital rollover is 8 Hz.
     * |        |          |...
     * |        |          |1111 = The binary rollover is 32.768 kHz, and the digital rollover is 16.384 kHz.
     * |        |          |Note:
     * |        |          |In the binary rollover mode, the PPS output (ptp_pps_o) has a duty cycle of 50 percent with these frequencies.
     * |        |          |In the digital rollover mode, the PPS output frequency is an average number
     * |        |          |The actual clock is of different frequency that gets synchronized every second
     * |        |          |For example:
     * |        |          |* When PPSCTRL = 0001, the PPS (1 Hz) has a low period of 537 ms and a high period of 463 ms.
     * |        |          |* When PPSCTRL = 0010, the PPS (2 Hz) is a sequence of:.
     * |        |          |One clock of 50 percent duty cycle and 537 ms period
     * |        |          |Second clock of 463 ms period (268 ms low and 195 ms high)
     * |        |          |* When PPSCTRL = 0011, the PPS (4 Hz) is a sequence of:.
     * |        |          |Three clocks of 50 percent duty cycle and 268 ms period
     * |        |          |Fourth clock of 195 ms period (134 ms low and 61 ms high)
     * |        |          |This behavior is because of the non-linear toggling of bits in the digital rollover mode in Register 451 (System Time - Nanoseconds Register).
     * |        |          |Flexible PPS0 Output (ptp_pps_o[0]) Control Programming these bits with a non-zero value instructs the MAC to initiate an event
     * |        |          |Once the command is transferred or synchronized to the PTP clock domain, these bits get cleared automatically
     * |        |          |The Software should ensure that these bits are programmed only when they are all-zero
     * |        |          |The following list describes the values of PPSCMD0:
     * |        |          |0000 = No Command
     * |        |          |0001 = START Single Pulse
     * |        |          |This command generates single pulse rising at the start point defined in Target Time Registers (register 455 and 456) and of a duration defined in the PPS0 Width Register.
     * |        |          |0010 = START Pulse Train
     * |        |          |This command generates the train of pulses rising at the start point defined in the Target Time Registers and of a duration defined in the PPS0 Width Register and repeated at interval defined in the PPS Interval Register
     * |        |          |By default, the PPS pulse train is free-running unless stopped by 'STOP Pulse train at time' or 'STOP Pulse Train immediately' commands.
     * |        |          |0011 = Cancel START
     * |        |          |This command cancels the START Single Pulse and START Pulse Train commands if the system time has not crossed the programmed start time.
     * |        |          |0100 = STOP Pulse Train at time
     * |        |          |This command stops the train of pulses initiated by the START Pulse Train command (PPSCMD = 0010) after the time programmed in the Target Time registers elapses.
     * |        |          |0101 = STOP Pulse Train immediately
     * |        |          |This command immediately stops the train of pulses initiated by the START Pulse Train command (PPSCMD = 0010).
     * |        |          |0110 = Cancel STOP Pulse Train
     * |        |          |This command cancels the STOP pulse train at time command if the programmed stop time has not elapsed
     * |        |          |The PPS pulse train becomes free-running on the successful execution of this command.
     * |        |          |0111-1111 = Reserved
     * |[4]     |PPSEN0    |Flexible PPS Output Mode Enable
     * |        |          |When set low, Bits[3:0] function as PPSCTRL (backward compatible)
     * |        |          |When set high, Bits[3:0] function as PPSCMD.
     * |[6:5]   |TRGTMODSEL0|Target Time Register Mode for PPS0 Output
     * |        |          |This field indicates the Target Time registers (register 455 and 456) mode for PPS0 output signal:
     * |        |          |00 = Indicates that the Target Time registers are programmed only for generating the interrupt event.
     * |        |          |01 = Reserved
     * |        |          |10 = Indicates that the Target Time registers are programmed for generating the interrupt event and starting or stopping the generation of the PPS0 output signal.
     * |        |          |11 = Indicates that the Target Time registers are programmed only for starting or stopping the generation of the PPS0 output signal
     * |        |          |No interrupt is asserted.
     * @var GMAC_T::PPSInterval
     * Offset: 0x760  Register 472 (PPS0 Interval Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |PPSINT    |PPS0 Output Signal Interval
     * |        |          |These bits store the interval between the rising edges of PPS0 signal output in terms of units of sub-second increment value
     * |        |          |You need to program one value less than the required interval
     * |        |          |For example, if the PTP reference clock is 50 MHz (period of 20ns), and desired interval between rising edges of PPS0 signal output is 100ns (that is, five units of sub-second increment value), then you should program value 4 (5 -1) in this register.
     * @var GMAC_T::PPSWidth
     * Offset: 0x764  Register 473 (PPS0 Width Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |PPSWIDTH  |PPS0 Output Signal Width
     * |        |          |These bits store the width between the rising edge and corresponding falling edge of the PPS0 signal output in terms of units of sub-second increment value.
     * |        |          |You need to program one value less than the required interval
     * |        |          |For example, if PTP reference clock is 50 MHz (period of 20ns), and desired width between the rising and corresponding falling edges of PPS0 signal output is 80ns (that is, four units of sub-second increment value), then you should program value 3 (4-1) in this register.
     * |        |          |Note: The value programmed in this register must be lesser than the value programmed in Register 472 (PPS0 Interval Register).
     * @var GMAC_T::DmaBusMode
     * Offset: 0x1000  Register 1000 (Bus Mode Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SWR       |Software Reset
     * |        |          |When this bit is set, the MAC DMA Controller resets the logic and all internal registers of the MAC
     * |        |          |It is cleared automatically after the reset operation has completed in all of the GMAC clock domains
     * |        |          |Before reprogramming any register of the GMAC, you should read a zero (0) value in this bit .
     * |        |          |Note:
     * |        |          |u00B7 The Software reset function is driven only by this bit.
     * |        |          |u00B7 The reset operation is completed only when all resets in all active clock domains are de-asserted
     * |        |          |Therefore, it is essential that all the PHY inputs clocks (applicable for the selected PHY interface) are present for the software reset completion.
     * |[6:2]   |DSL       |Descriptor Skip Length
     * |        |          |This bit specifies the number of Word, Dword, or Lword (depending on the 32-bit, 64-bit, or 128-bit bus) to skip between two unchained descriptors
     * |        |          |The address skipping starts from the end of current descriptor to the start of next descriptor
     * |        |          |When the DSL value is equal to zero, the descriptor table is taken as contiguous by the DMA in Ring mode.
     * |[7]     |ATDS      |Alternate Descriptor Size
     * |        |          |When set, the size of the alternate descriptor increases to 32 bytes (8 DWORDS)
     * |        |          |This is required when the Advanced Timestamp feature or the IPC Full Offload Engine (Type 2) is enabled in the receiver
     * |        |          |The enhanced descriptor is not required if the Advanced Timestamp and IPC Full Checksum Offload (Type 2) features are not enabled
     * |        |          |In such cases, you can use the 16 bytes descriptor to save 4 bytes of memory.
     * |        |          |When reset, the descriptor size reverts back to 4 DWORDs (16 bytes)
     * |        |          |This bit preserves the backward compatibility for the descriptor size
     * |        |          |In versions prior to 3.50a, the descriptor size is 16 bytes for both normal and enhanced descriptor
     * |        |          |In version 3.50a, descriptor size is increased to 32 bytes because of the Advanced Timestamp and IPC Full Checksum Offload Engine (Type 2) features.
     * |[13:8]  |PBL       |Programmable Burst Length
     * |        |          |These bits indicate the maximum number of beats to be transferred in one DMA transaction
     * |        |          |This is the maximum value that is used in a single block Read or Write
     * |        |          |The DMA always attempts to burst as specified in PBL each time it starts a Burst transfer on the host bus
     * |        |          |PBL can be programmed with permissible values of 1, 2, 4, 8, 16, and 32
     * |        |          |Any other value results in undefined behavior
     * |        |          |When USP is set high, this PBL value is applicable only for Tx DMA transactions
     * |        |          |If the number of beats to be transferred is more than 32, then perform the following steps:
     * |        |          |1. Set the PBLx8 mode.
     * |        |          |2. Set the PBL.
     * |        |          |For example, if the maximum number of beats to be transferred is 64, then first set PBLx8 to 1 and then set PBL to 8
     * |        |          |The PBL values have the following limitation: The maximum number of possible beats (PBL) is limited by the size of the Tx FIFO and Rx FIFO in the MTL layer and the data bus width on the DMA
     * |        |          |The FIFO has a constraint that the maximum beat supported is half the depth of the FIFO, except when specified.
     * |        |          |For different data bus widths and FIFO sizes, the valid PBL range (including x8 mode) is provided in the following list
     * |        |          |If the PBL is common for both transmit and receive DMA, the minimum Rx FIFO and Tx FIFO depths must be considered.
     * |        |          |Note: In the half-duplex mode, the valid PBL range specified in the following list is applicable only for Tx FIFO.
     * |        |          |* 32-Bit Data Bus Width
     * |        |          |u00B7 128 Bytes FIFO Depth: In the full-duplex mode, the valid PBL range is 16 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 8 or less for the 10 or 100 Mbps mode.
     * |        |          |u00B7 256 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 32 or less.
     * |        |          |u00B7 512 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 64 or less.
     * |        |          |u00B7 1 Kbytes FIFO Depth: In the full-duplex mode, the valid PBL range is 128 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 128 or less in the 10 or 100 Mbps mode and 64 or less in the 1000 Mbps mode.
     * |        |          |u00B7 2 Kbytes and Higher FIFO Depth: All PBL values are supported in the full-duplex mode and half-duplex modes.
     * |        |          |* 64-Bit Data Bus Width
     * |        |          |u00B7 128 Bytes FIFO Depth: In the full-duplex mode, the valid PBL range is 8 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 4 or less for the 10 or 100 Mbps mode.
     * |        |          |u00B7 256 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 16 or less.
     * |        |          |u00B7 512 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 32 or less.
     * |        |          |u00B7 1 Kbytes FIFO Depth: In the full-duplex mode, the valid PBL range is 64 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 64 or less in the 10 or 100 Mbps mode and 32 or less in the 1000-Mbps mode.
     * |        |          |u00B7 2 Kbytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 128 or less.
     * |        |          |u00B7 4 Kbytes and Higher FIFO Depth: All PBL values are supported in the full-duplex and half-duplex modes.
     * |        |          |* 128-Bit Data Bus Width
     * |        |          |u00B7 128 Bytes FIFO Depth: In the full-duplex mode, the valid PBL range is 4 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 2 or less for the 10 or 100 Mbps mode.
     * |        |          |u00B7 256 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 8 or less.
     * |        |          |u00B7 512 Bytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 16 or less.
     * |        |          |u00B7 1 Kbytes FIFO Depth: In the full-duplex mode, the valid PBL range is 32 or less
     * |        |          |In the half-duplex mode, the valid PBL range is 32 or less in the 10 or 100 Mbps mode and 16 or less in the 1000-Mbps mode.
     * |        |          |u00B7 2 Kbytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 64 or less.
     * |        |          |u00B7 4 Kbytes FIFO Depth: In the full-duplex mode and the half-duplex (10 or 100 Mbps) modes, the valid PBL range is 128 or less.
     * |        |          |u00B7 8 Kbytes and Higher FIFO Depth: All PBL values are supported in the full-duplex and half-duplex modes.
     * |[16]    |FB        |Fixed Burst
     * |        |          |This bit controls whether the AHB or AXI Master interface performs fixed burst transfers or not
     * |        |          |When set, the AHB interface uses only SINGLE, INCR4, INCR8, or INCR16 during start of the normal burst transfers
     * |        |          |When reset, the AHB or AXI interface uses SINGLE and INCR burst transfer operations.
     * |        |          |For more information, see Bit 0 (UNDEF) of the AXI Bus Mode register.
     * |[22:17] |RPBL      |Rx DMA PBL
     * |        |          |This field indicates the maximum number of beats to be transferred in one Rx DMA transaction
     * |        |          |This is the maximum value that is used in a single block Read or Write.
     * |        |          |The Rx DMA always attempts to burst as specified in the RPBL bit each time it starts a Burst transfer on the host bus
     * |        |          |You can program RPBL with values of 1, 2, 4, 8, 16, and 32
     * |        |          |Any other value results in undefined behavior
     * |        |          |This field is valid and applicable only when USP is set high.
     * |[23]    |USP       |Use Separate PBL
     * |        |          |When set high, this bit configures the Rx DMA to use the value configured in Bits[22:17] as PBL
     * |        |          |The PBL value in Bits[13:8] is applicable only to the Tx DMA operations.
     * |        |          |When reset to low, the PBL value in Bits[13:8] is applicable for both DMA engines.
     * |[24]    |PBLx8     |PBLx8 Mode
     * |        |          |When set high, this bit multiplies the programmed PBL value (Bits[22:17] and Bits[13:8]) eight times
     * |        |          |Therefore, the DMA transfers the data in 8, 16, 32, 64, 128, and 256 beats depending on the PBL value
     * |[25]    |AAL       |Address Aligned Beats
     * |        |          |When this bit is set high and the FB bit is equal to 1, the AHB or AXI interface generates all bursts aligned to the start address LS bits
     * |        |          |If the FB bit is equal to 0, the first burst (accessing the data buffer's start address) is not aligned, but subsequent bursts are aligned to the address.
     * @var GMAC_T::DmaTxPollDemand
     * Offset: 0x1004  Register 1001 (Transmit Poll Demand Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |TPD       |Transmit Poll Demand
     * |        |          |When these bits are written with any value, the DMA reads the current descriptor pointed to by Register 1018 (Current Host Transmit Descriptor Register)
     * |        |          |If that descriptor is not available (owned by the Host), the transmission returns to the Suspend state and the Bit 2 (TU) of Register 1005 (Status Register) is asserted
     * |        |          |If the descriptor is available, the transmission resumes.
     * @var GMAC_T::DmaRxPollDemand
     * Offset: 0x1008  Register 1002 (Receive Poll Demand Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |RPD       |Receive Poll Demand
     * |        |          |When these bits are written with any value, the DMA reads the current descriptor pointed to by Register 1019 (Current Host Receive Descriptor Register)
     * |        |          |If that descriptor is not available (owned by the Host), the reception returns to the Suspended state and the Bit 7 (RU) of Register 1005 (Status Register) is not asserted
     * |        |          |If the descriptor is available, the Rx DMA returns to the active state.
     * @var GMAC_T::DmaRxDesc
     * Offset: 0x100C  Register 1003 (Receive Descriptor List Address Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:3]  |RDESLA_64_bit|Start of Receive List
     * |        |          |This field contains the base address of the first descriptor in the Receive Descriptor list
     * |        |          |The LSB bits (2:0) for 64-bit bus width are ignored and are internally taken as all-zero by the DMA
     * |        |          |Therefore, these LSB bits are read-only (RO).
     * @var GMAC_T::DmaTxDesc
     * Offset: 0x1010  Register 1004 (Transmit Descriptor List Address Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:3]  |TDESLA_64_bit|Start of Transmit List
     * |        |          |This field contains the base address of the first descriptor in the Transmit Descriptor list
     * |        |          |The LSB bits (2:0) for 64-bit bus width are ignored and are internally taken as all-zero by the DMA
     * |        |          |Therefore, these LSB bits are read-only (RO).
     * @var GMAC_T::DmaStatus
     * Offset: 0x1014  Register 1005 (Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TI        |Transmit Interrupt
     * |        |          |This bit indicates that the frame transmission is complete
     * |        |          |When transmission is complete, Bit 31 (OWN) of TDES0 is reset, and the specific frame status information is updated in the descriptor.
     * |[1]     |TPS       |Transmit Process Stopped
     * |        |          |This bit is set when the transmission is stopped.
     * |[2]     |TU        |Transmit Buffer Unavailable
     * |        |          |This bit indicates that the host owns the Next Descriptor in the Transmit List and the DMA cannot acquire it
     * |        |          |Transmission is suspended
     * |        |          |Bits[22:20] explain the Transmit Process state transitions.
     * |        |          |To resume processing Transmit descriptors, the host should change the ownership of the descriptor by setting TDES0[31] and then issue a Transmit Poll Demand command.
     * |[3]     |TJT       |Transmit Jabber Timeout
     * |        |          |This bit indicates that the Transmit Jabber Timer expired, which happens when the frame size exceeds 2,048 (10,240 bytes when the Jumbo frame is enabled)
     * |        |          |When the Jabber Timeout occurs, the transmission process is aborted and placed in the Stopped state
     * |        |          |This causes the Transmit Jabber Timeout TDES0[14] flag to assert.
     * |[4]     |OVF       |Receive Overflow
     * |        |          |This bit indicates that the Receive Buffer had an Overflow during frame reception
     * |        |          |If the partial frame is transferred to the application, the overflow status is set in RDES0[11].
     * |[5]     |UNF       |Transmit Underflow
     * |        |          |This bit indicates that the Transmit Buffer had an Underflow during frame transmission
     * |        |          |Transmission is suspended and an Underflow Error TDES0[1] is set.
     * |[6]     |RI        |Receive Interrupt
     * |        |          |This bit indicates that the frame reception is complete
     * |        |          |When reception is complete, the Bit 31 of RDES1 (Disable Interrupt on Completion) is reset in the last Descriptor, and the specific frame status information is updated in the descriptor
     * |        |          |The reception remains in the Running state.
     * |[7]     |RU        |Receive Buffer Unavailable
     * |        |          |This bit indicates that the host owns the Next Descriptor in the Receive List and the DMA cannot acquire it
     * |        |          |The Receive Process is suspended
     * |        |          |To resume processing Receive descriptors, the host should change the ownership of the descriptor and issue a Receive Poll Demand command
     * |        |          |If no Receive Poll Demand is issued, the Receive Process resumes when the next recognized incoming frame is received
     * |        |          |This bit is set only when the previous Receive Descriptor is owned by the DMA.
     * |[8]     |RPS       |Receive Process Stopped
     * |        |          |This bit is asserted when the Receive Process enters the Stopped state.
     * |[9]     |RWT       |Receive Watchdog Timeout
     * |        |          |When set, this bit indicates that the Receive Watchdog Timer expired while receiving the current frame and the current frame is truncated after the watchdog timeout.
     * |[10]    |ETI       |Early Transmit Interrupt
     * |        |          |This bit indicates that the frame to be transmitted is fully transferred to the MTL Transmit FIFO.
     * |[13]    |FBI       |Fatal Bus Error Interrupt
     * |        |          |This bit indicates that a bus error occurred, as described in Bits[25:23]
     * |        |          |When this bit is set, the corresponding DMA engine disables all of its bus accesses.
     * |[14]    |ERI       |Early Receive Interrupt
     * |        |          |This bit indicates that the DMA filled the first data buffer of the packet
     * |        |          |This bit is cleared when the software writes 1 to this bit or Bit 6 (RI) of this register is set (whichever occurs earlier).
     * |[15]    |AIS       |Abnormal Interrupt Summary
     * |        |          |Abnormal Interrupt Summary bit value is the logical OR of the following when the corresponding interrupt bits are enabled in Register 1007 (Interrupt Enable Register):
     * |        |          |Register 1005[1] = Transmit Process Stopped
     * |        |          |Register 1005[3] = Transmit Jabber Timeout
     * |        |          |Register 1005[4] = Receive FIFO Overflow
     * |        |          |Register 1005[5] = Transmit Underflow
     * |        |          |Register 1005[7] = Receive Buffer Unavailable
     * |        |          |Register 1005[8] = Receive Process Stopped
     * |        |          |Register 1005[9] = Receive Watchdog Timeout
     * |        |          |Register 1005[10] = Early Transmit Interrupt
     * |        |          |Register 1005[13] = Fatal Bus Error
     * |        |          |Only unmasked bits affect the Abnormal Interrupt Summary bit.
     * |        |          |This is a sticky bit and must be cleared each time a corresponding bit, which causes AIS to be set, is cleared.
     * |[16]    |NIS       |Normal Interrupt Summary
     * |        |          |Normal Interrupt Summary bit value is the logical OR of the following when the corresponding interrupt bits are enabled in Register 1007 (Interrupt Enable Register):
     * |        |          |Register 1005[0] = Transmit Interrupt
     * |        |          |Register 1005[2] = Transmit Buffer Unavailable
     * |        |          |Register 1005[6] = Receive Interrupt
     * |        |          |Register 1005[14] = Early Receive Interrupt
     * |        |          |Only unmasked bits (interrupts for which interrupt enable is set in Register 1007) affect the Normal Interrupt Summary bit.
     * |        |          |This is a sticky bit and must be cleared (by writing 1 to this bit) each time a corresponding bit, which causes NIS to be set, is cleared.
     * |[19:17] |RS        |Received Process State (read only)
     * |        |          |This field indicates the Receive DMA FSM state. This field does not generate an interrupt.
     * |        |          |3'b000 = Stopped::Reset or Stop Receive Command issued
     * |        |          |3'b001 = Running: Fetching Receive Transfer Descriptor
     * |        |          |3'b010 = Reserved for future use
     * |        |          |3'b011 = Running: Waiting for receive packet
     * |        |          |3'b100 = Suspended: Receive Descriptor Unavailable
     * |        |          |3'b101 = Running: Closing Receive Descriptor
     * |        |          |3'b110 = TIME_STAMP write state
     * |        |          |3'b111 =:Running: Transferring the receive packet data from receive buffer to host memory
     * |[22:20] |TS        |Transmit Process State (read only)
     * |        |          |This field indicates the Transmit DMA FSM state. This field does not generate an interrupt.
     * |        |          |3'b000 = Stopped; Reset or Stop Transmit Command issued
     * |        |          |3'b001 = Running; Fetching Transmit Transfer Descriptor
     * |        |          |3'b010 = Running; Waiting for status
     * |        |          |3'b011 = Running; Reading Data from host memory buffer and queuing it to transmit buffer (Tx FIFO)
     * |        |          |3'b100 = TIME_STAMP write state
     * |        |          |3'b101 = Reserved for future use
     * |        |          |3'b110 = Suspended; Transmit Descriptor Unavailable or Transmit Buffer Underflow
     * |        |          |3'b111 = Running; Closing Transmit Descriptor
     * |[25:23] |EB        |Error Bits (read only)
     * |        |          |This field indicates the type of error that caused a Bus Error, for example, error response on the AHB or AXI interface
     * |        |          |This field is valid only when Bit 13 (FBI) is set
     * |        |          |This field does not generate an interrupt.
     * |        |          |000 = Error during Rx DMA Write Data Transfer
     * |        |          |011 = Error during Tx DMA Read Data Transfer
     * |        |          |100 = Error during Rx DMA Descriptor Write Access
     * |        |          |101 = Error during Tx DMA Descriptor Write Access
     * |        |          |110 = Error during Rx DMA Descriptor Read Access
     * |        |          |111 = Error during Tx DMA Descriptor Read Access
     * |        |          |Note: 001 and 010 are reserved.
     * |[26]    |GLI       |GMAC Line interface Interrupt (read only)
     * |        |          |When set, this bit reflects any of the following interrupt events in the GMAC interfaces:
     * |        |          |u00B7 RGMII: Link change event
     * |        |          |To identify the exact cause of the interrupt, the software must first read Bit 11 and Bits[2:0] of Register 14 (Interrupt Status Register) and then to clear the source of interrupt (which also clears the GLI interrupt), read any of the following corresponding registers:
     * |        |          |u00B7 RGMII: Register 54 (RGMII Status Register)
     * |        |          |The interrupt signal from the GMAC subsystem (sbd_intr_o) is high when this bit is high.
     * |[28]    |GPI       |GMAC PMT Interrupt (read only)
     * |        |          |This bit indicates an interrupt event in the PMT module of the GMAC
     * |        |          |The software must read the PMT Control and Status Register in the MAC to get the exact cause of interrupt and clear its source to reset this bit to 1'b0
     * |        |          |The interrupt signal from the GMAC subsystem (sbd_intr_o) is high when this bit is high.
     * |        |          |This bit is applicable only when the Power Management feature is enabled
     * |        |          |Otherwise, this bit is reserved.
     * |        |          |Note: The GPI and pmt_intr_o interrupts are generated in different clock domains.
     * |[29]    |TTI       |Timestamp Trigger Interrupt (read only)
     * |        |          |This bit indicates an interrupt event in the Timestamp Generator block of GMAC
     * |        |          |The software must read the corresponding registers in the GMAC to get the exact cause of interrupt and clear its source to reset this bit to 1'b0
     * |        |          |When this bit is high, the interrupt signal from the GMAC subsystem (sbd_intr_o) is high.
     * |        |          |This bit is applicable only when the IEEE 1588 Timestamp feature is enabled
     * |        |          |Otherwise, this bit is reserved.
     * |[30]    |GLPII     |GMAC LPI Interrupt (read only)
     * |        |          |This bit indicates an interrupt event in the LPI logic of the GMAC
     * |        |          |To reset this bit to 1'b0, the software must read the corresponding registers in the GMAC to get the exact cause of the interrupt and clear its source.
     * |        |          |Note: GLPII status is given only in Channel 0 DMA register and is applicable only when the Energy Efficient Ethernet feature is enabled
     * |        |          |Otherwise, this bit is reserved
     * |        |          |When this bit is high, the interrupt signal from the MAC (sbd_intr_o) is high.
     * @var GMAC_T::DmaOpMode
     * Offset: 0x1018  Register 1006 (Operation Mode Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1]     |SR        |Start or Stop Receive
     * |        |          |When this bit is set, the Receive process is placed in the Running state
     * |        |          |The DMA attempts to acquire the descriptor from the Receive list and processes the incoming frames
     * |        |          |The descriptor acquisition is attempted from the current position in the list, which is the address set by Register 1003 (Receive Descriptor List Address Register) or the position retained when the Receive process was previously stopped
     * |        |          |If the DMA does not own the descriptor, reception is suspended and Bit 7 (Receive Buffer Unavailable) of Register 1005 (Status Register) is set
     * |        |          |The Start Receive command is effective only when the reception has stopped
     * |        |          |If the command is issued before setting Register 1003 (Receive Descriptor List Address Register), the DMA behavior is unpredictable.
     * |        |          |When this bit is cleared, the Rx DMA operation is stopped after the transfer of the current frame
     * |        |          |The next descriptor position in the Receive list is saved and becomes the current position after the Receive process is restarted
     * |        |          |The Stop Receive command is effective only when the Receive process is in either the Running (waiting for receive packet) or in the Suspended state.
     * |[2]     |OSF       |Operate on Second Frame
     * |        |          |When this bit is set, it instructs the DMA to process the second frame of the Transmit data even before the status for the first frame is obtained.
     * |[4:3]   |RTC       |Receive Threshold Control
     * |        |          |These two bits control the threshold level of the MTL Receive FIFO
     * |        |          |Transfer (request) to DMA starts when the frame size within the MTL Receive FIFO is larger than the threshold
     * |        |          |In addition, full frames with length less than the threshold are transferred automatically.
     * |        |          |These bits are valid only when the RSF bit is zero, and are ignored when the RSF bit is set to 1.
     * |        |          |00 = 64 bytes
     * |        |          |01 = 32 bytes
     * |        |          |10 = 96 bytes
     * |        |          |11 = 128 bytes
     * |[5]     |DGF       |Drop Giant Frames
     * |        |          |When set, the MAC drops the received giant frames in the Rx FIFO, that is, frames that are larger than the computed giant frame limit
     * |        |          |When reset, the MAC does not drop the giant frames in the Rx FIFO.
     * |[6]     |FUF       |Forward Undersized Good Frames
     * |        |          |When set, the Rx FIFO forwards Undersized frames (frames with no Error and length less than 64 bytes) including pad-bytes and CRC.
     * |        |          |When reset, the Rx FIFO drops all frames of less than 64 bytes, unless a frame is already transferred because of the lower value of Receive Threshold, for example, RTC = 01.
     * |[7]     |FEF       |Forward Error Frames
     * |        |          |When this bit is reset, the Rx FIFO drops frames with error status (CRC error, collision error, GMII_ER, giant frame, watchdog timeout, or overflow)
     * |        |          |However, if the start byte (write) pointer of a frame is already transferred to the read controller side (in Threshold mode), then the frame is not dropped
     * |        |          |When the FEF bit is set, all frames except runt error frames are forwarded to the DMA
     * |        |          |If the Bit 25 (RSF) is set and the Rx FIFO overflows when a partial frame is written, then the frame is dropped irrespective of the FEF bit setting
     * |        |          |However, if the Bit 25 (RSF) is reset and the Rx FIFO overflows when a partial frame is written, then a partial frame may be forwarded to the DMA.
     * |[8]     |EFC       |Enable HW Flow Control
     * |        |          |When this bit is set, the flow control signal operation based on the fill-level of Rx FIFO is enabled
     * |        |          |When reset, the flow control operation is disabled
     * |        |          |This bit is not used (reserved and always reset) when the Rx FIFO is less than 4 Kbytes.
     * |[10:9]  |RFA       |Threshold for Activating Flow Control
     * |        |          |These bits control the threshold (Fill level of Rx FIFO) at which the flow control is activated.
     * |        |          |00 = Full minus 1 Kbytes, that is, FULL - 1 Kbytes
     * |        |          |01 = Full minus 2 Kbytes, that is, FULL - 2 Kbytes
     * |        |          |10 = Full minus 3 Kbytes, that is, FULL - 3 Kbytes
     * |        |          |11 = Full minus 4 Kbytes, that is, FULL - 4 Kbytes
     * |        |          |These values are applicable only to Rx FIFOs of 4 Kbytes or more and when Bit 8 (EFC) is set high
     * |        |          |If the Rx FIFO is 8 Kbytes or more, an additional Bit (RFA_2) is used for more threshold levels as described in Bit 23
     * |        |          |These bits are reserved and read-only when the depth of Rx FIFO is less than 4 Kbytes.
     * |        |          |Note: When FIFO size is exactly 4 Kbytes, although the GMAC allows you to program the value of these bits to 11, the software should not program these bits to 2'b11
     * |        |          |The value 2'b11 means flow control on FIFO empty condition.
     * |[12:11] |RFD       |Threshold for Deactivating Flow Control
     * |        |          |These bits control the threshold (Fill-level of Rx FIFO) at which the flow control is de-asserted after activation.
     * |        |          |00 = Full minus 1 Kbytes, that is, FULL - 1 Kbytes
     * |        |          |01 = Full minus 2 Kbytes, that is, FULL - 2 Kbytes
     * |        |          |10 = Full minus 3 Kbytes, that is, FULL - 3 Kbytes
     * |        |          |11 = Full minus 4 Kbytes, that is, FULL - 4 Kbytes
     * |        |          |The de-assertion is effective only after flow control is asserted
     * |        |          |If the Rx FIFO is 8 Kbytes or more, an additional bit (RFD_2) is used for more threshold levels as described in Bit 22
     * |        |          |These bits are reserved and read-only when the Rx FIFO depth is less than 4 Kbytes
     * |        |          |Note = For proper flow control, the value programmed in the "RFD_2, RFD" fields should be equal to or more than the value programmed in the "RFA_2, RFA" fields.
     * |[13]    |ST        |Start or Stop Transmission Command
     * |        |          |When this bit is set, transmission is placed in the Running state, and the DMA checks the Transmit List at the current position for a frame to be transmitted
     * |        |          |Descriptor acquisition is attempted either from the current position in the list, which is the Transmit List Base Address set by Register 1004 (Transmit Descriptor List Address Register), or from the position retained when transmission was stopped previously
     * |        |          |If the DMA does not own the current descriptor, transmission enters the Suspended state and Bit 2 (Transmit Buffer Unavailable) of Register 1005 (Status Register) is set
     * |        |          |The Start Transmission command is effective only when transmission is stopped
     * |        |          |If the command is issued before setting Register 1004 (Transmit Descriptor List Address Register), then the DMA behavior is unpredictable.
     * |        |          |When this bit is reset, the transmission process is placed in the Stopped state after completing the transmission of the current frame
     * |        |          |The Next Descriptor position in the Transmit List is saved, and it becomes the current position when transmission is restarted
     * |        |          |To change the list address, you need to program Register 1004 (Transmit Descriptor List Address Register) with a new value when this bit is reset
     * |        |          |The new value is considered when this bit is set again
     * |        |          |The stop transmission command is effective only when the transmission of the current frame is complete or the transmission is in the Suspended state.
     * |[16:14] |TTC       |Transmit Threshold Control
     * |        |          |These bits control the threshold level of the MTL Transmit FIFO
     * |        |          |Transmission starts when the frame size within the MTL Transmit FIFO is larger than the threshold
     * |        |          |In addition, full frames with a length less than the threshold are also transmitted
     * |        |          |These bits are used only when Bit 21 (TSF) is reset.
     * |        |          |000 = 64 bytes
     * |        |          |001 = 128 bytes
     * |        |          |010 = 192 bytes
     * |        |          |011 = 256 bytes
     * |        |          |100 = 40 bytes
     * |        |          |101 = 32 bytes
     * |        |          |110 = 24 bytes
     * |        |          |111 = 16 bytes
     * |[20]    |FTF       |Flush Transmit FIFO
     * |        |          |When this bit is set, the transmit FIFO controller logic is reset to its default values and thus all data in the Tx FIFO is lost or flushed
     * |        |          |This bit is cleared internally when the flushing operation is completed
     * |        |          |The Operation Mode register should not be written to until this bit is cleared
     * |        |          |The data which is already accepted by the MAC transmitter is not flushed
     * |        |          |It is scheduled for transmission and results in underflow and runt frame transmission.
     * |        |          |Note: The flush operation is complete only when the Tx FIFO is emptied of its contents and all the pending Transmit Status of the transmitted frames are accepted by the host
     * |        |          |To complete this flush operation, the PHY transmit clock (clk_tx_i) is required to be active.
     * |[21]    |TSF       |Transmit Store and Forward
     * |        |          |When this bit is set, transmission starts when a full frame resides in the MTL Transmit FIFO
     * |        |          |When this bit is set, the TTC values specified in Bits[16: 14] are ignored
     * |        |          |This bit should be changed only when the transmission is stopped.
     * |[24]    |DFF       |Disable Flushing of Received Frames
     * |        |          |When this bit is set, the Rx DMA does not flush any frames because of the unavailability of receive descriptors or buffers as it does normally when this bit is reset
     * |[25]    |RSF       |Receive Store and Forward
     * |        |          |When this bit is set, the MTL reads a frame from the Rx FIFO only after the complete frame has been written to it, ignoring the RTC bits
     * |        |          |When this bit is reset, the Rx FIFO operates in the cut-through mode, subject to the threshold specified by the RTC bits
     * |[26]    |DT        |Disable Dropping of TCP/IP Checksum Error Frames
     * |        |          |When this bit is set, the MAC does not drop the frames which only have errors detected by the Receive Checksum Offload engine
     * |        |          |Such frames do not have any errors (including FCS error) in the Ethernet frame received by the MAC but have errors only in the encapsulated payload
     * |        |          |When this bit is reset, all error frames are dropped if the FEF bit is reset.
     * |        |          |If the IPC Full Checksum Offload Engine (Type 2) is disabled, this bit is reserved (RO with value 1'b0).
     * @var GMAC_T::DmaInt
     * Offset: 0x101C  Register 1007 (Interrupt Enable Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TIE       |Transmit Interrupt Enable
     * |        |          |When this bit is set with Normal Interrupt Summary Enable (Bit 16), the Transmit Interrupt is enabled
     * |        |          |When this bit is reset, the Transmit Interrupt is disabled.
     * |[1]     |TSE       |Transmit Stopped Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Transmission Stopped Interrupt is enabled
     * |        |          |When this bit is reset, the Transmission Stopped Interrupt is disabled.
     * |[2]     |TUE       |Transmit Buffer Unavailable Enable
     * |        |          |When this bit is set with Normal Interrupt Summary Enable (Bit 16), the Transmit Buffer Unavailable Interrupt is enabled
     * |        |          |When this bit is reset, the Transmit Buffer Unavailable Interrupt is disabled.
     * |[3]     |TJE       |Transmit Jabber Timeout Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Transmit Jabber Timeout Interrupt is enabled
     * |        |          |When this bit is reset, the Transmit Jabber Timeout Interrupt is disabled.
     * |[4]     |OVE       |Overflow Interrupt Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Receive Overflow Interrupt is enabled
     * |        |          |When this bit is reset, the Overflow Interrupt is disabled.
     * |[5]     |UNE       |Underflow Interrupt Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Transmit Underflow Interrupt is enabled
     * |        |          |When this bit is reset, the Underflow Interrupt is disabled.
     * |[6]     |RIE       |Receive Interrupt Enable
     * |        |          |When this bit is set with Normal Interrupt Summary Enable (Bit 16), the Receive Interrupt is enabled
     * |        |          |When this bit is reset, the Receive Interrupt is disabled.
     * |[7]     |RUE       |Receive Buffer Unavailable Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Receive Buffer Unavailable Interrupt is enabled
     * |        |          |When this bit is reset, the Receive Buffer Unavailable Interrupt is disabled.
     * |[8]     |RSE       |Receive Stopped Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Receive Stopped Interrupt is enabled
     * |        |          |When this bit is reset, the Receive Stopped Interrupt is disabled.
     * |[9]     |RWE       |Receive Watchdog Timeout Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Receive Watchdog Timeout Interrupt is enabled
     * |        |          |When this bit is reset, the Receive Watchdog Timeout Interrupt is disabled.
     * |[10]    |ETE       |Early Transmit Interrupt Enable
     * |        |          |When this bit is set with an Abnormal Interrupt Summary Enable (Bit 15), the Early Transmit Interrupt is enabled
     * |        |          |When this bit is reset, the Early Transmit Interrupt is disabled.
     * |[13]    |FBE       |Fatal Bus Error Enable
     * |        |          |When this bit is set with Abnormal Interrupt Summary Enable (Bit 15), the Fatal Bus Error Interrupt is enabled
     * |        |          |When this bit is reset, the Fatal Bus Error Enable Interrupt is disabled.
     * |[14]    |ERE       |Early Receive Interrupt Enable
     * |        |          |When this bit is set with Normal Interrupt Summary Enable (Bit 16), the Early Receive Interrupt is enabled
     * |        |          |When this bit is reset, the Early Receive Interrupt is disabled.
     * |[15]    |AIE       |Abnormal Interrupt Summary Enable
     * |        |          |When this bit is set, abnormal interrupt summary is enabled
     * |        |          |When this bit is reset, the abnormal interrupt summary is disabled
     * |        |          |This bit enables the following interrupts in Register 1005 (Status Register):
     * |        |          |Register 1005[1] = Transmit Process Stopped
     * |        |          |Register 1005[3] = Transmit Jabber Timeout
     * |        |          |Register 1005[4] = Receive Overflow
     * |        |          |Register 1005[5] = Transmit Underflow
     * |        |          |Register 1005[7] = Receive Buffer Unavailable
     * |        |          |Register 1005[8] = Receive Process Stopped
     * |        |          |Register 1005[9] = Receive Watchdog Timeout
     * |        |          |Register 1005[10] = Early Transmit Interrupt
     * |        |          |Register 1005[13] = Fatal Bus Error
     * |[16]    |NIE       |Normal Interrupt Summary Enable
     * |        |          |When this bit is set, normal interrupt summary is enabled
     * |        |          |When this bit is reset, normal interrupt summary is disabled
     * |        |          |This bit enables the following interrupts in Register 1005 (Status Register):
     * |        |          |Register 1005[0] = Transmit Interrupt
     * |        |          |Register 1005[2] = Transmit Buffer Unavailable
     * |        |          |Register 1005[6] = Receive Interrupt
     * |        |          |Register 1005[14] = Early Receive Interrupt
     * @var GMAC_T::DmaMissedFrame
     * Offset: 0x1020  Register 1008 (Missed Frame and Buffer Overflow Counter Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |MISFRMCNT |Missed Frame Counter
     * |        |          |This field indicates the number of frames missed by the controller because of the Host Receive Buffer being unavailable
     * |        |          |This counter is incremented each time the DMA discards an incoming frame
     * |        |          |The counter is cleared when this register is read with mci_be_i[0] at 1'b1.
     * |[16]    |MISCNTOVF |Overflow Bit for Missed Frame Counter
     * |        |          |This bit is set every time Missed Frame Counter (Bits[15:0]) overflows, that is, the DMA discards an incoming frame because of the Host Receive Buffer being unavailable with the missed frame counter at maximum value
     * |        |          |In such a scenario, the Missed frame counter is reset to all-zeros and this bit indicates that the rollover happened.
     * |[27:17] |OVFFRMCNT |Overflow Frame Counter
     * |        |          |This field indicates the number of frames missed by the application
     * |        |          |This counter is incremented each time the MTL FIFO overflows
     * |        |          |The counter is cleared when this register is read with mci_be_i[2] at 1'b1.
     * |[28]    |OVFCNTOVF |Overflow Bit for FIFO Overflow Counter
     * |        |          |This bit is set every time the Overflow Frame Counter (Bits[27: 17]) overflows, that is, the Rx FIFO overflows with the overflow frame counter at maximum value
     * |        |          |In such a scenario, the overflow frame counter is reset to all-zeros and this bit indicates that the rollover happened.
     * @var GMAC_T::DmaRxIntWdt
     * Offset: 0x1024  Register 1009 (Receive Interrupt Watchdog Timer Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |RIWT      |RI Watchdog Timer Count
     * |        |          |This bit indicates the number of system clock cycles multiplied by 256 for which the watchdog timer is set
     * |        |          |The watchdog timer gets triggered with the programmed value after the Rx DMA completes the transfer of a frame for which the RI status bit is not set because of the setting in the corresponding descriptor RDES1[31]
     * |        |          |When the watchdog timer runs out, the RI bit is set and the timer is stopped
     * |        |          |The watchdog timer is reset when the RI bit is set high because of automatic setting of RI as per RDES1[31] of any received frame.
     * @var GMAC_T::DmaAxiBusMode
     * Offset: 0x1028  Register 1010 (AXI Bus Mode Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |UNDEF     |AXI Undefined Burst Length (read only)
     * |        |          |This bit is read-only bit and indicates the complement (invert) value of Bit 16 (FB) in Register 1000 (Bus Mode Register[16]).
     * |        |          |u00B7 When this bit is set to 1, the GMAC-AXI is allowed to perform any burst length equal to or below the maximum allowed burst length programmed in Bits[7:1].
     * |        |          |u00B7 When this bit is set to 0, the GMAC-AXI is allowed to perform only fixed burst lengths as indicated by BLEN256, BLEN128, BLEN64, BLEN32, BLEN16, BLEN8, or BLEN4, or a burst length of 1
     * |[1]     |BLEN4     |AXI Burst Length 4
     * |        |          |When this bit is set to 1, the GMAC-AXI is allowed to select a burst length of 4 on the AXI Master interface.
     * |        |          |Setting this bit has no effect when UNDEF is set to 1.
     * |[2]     |BLEN8     |AXI Burst Length 8
     * |        |          |When this bit is set to 1, the GMAC-AXI is allowed to select a burst length of 8 on the AXI Master interface.
     * |        |          |Setting this bit has no effect when UNDEF is set to 1.
     * |[3]     |BLEN16    |AXI Burst Length 16
     * |        |          |When this bit is set to 1 or UNDEF is set to 1, the GMAC-AXI is allowed to select a burst length of 16 on the AXI Master interface.
     * |[12]    |AXI_AAL   |Address-Aligned Beats (read only)
     * |        |          |This bit is read-only bit and reflects the Bit 25 (AAL) of Register 1000 (Bus Mode Register).
     * |        |          |When this bit is set to 1, the GMAC-AXI performs address-aligned burst transfers on both read and write channels.
     * |[13]    |ONEKBBE   |1 Kbytes Boundary Crossing Enable for the GMAC-AXI Master When set, the GMAC-AXI Master performs burst transfers that do not cross 1 Kbytes boundary When reset, the GMAC-AXI Master performs burst transfers that do not cross 4 Kbytes boundary.
     * |[17:16] |RD_OSR_LMT|AXI Maximum Read OutStanding Request Limit
     * |        |          |This value limits the maximum outstanding request on the AXI read interface
     * |        |          |Maximum outstanding requests = RD_OSR_LMT+1.
     * |        |          |Note: The Bit 18 is reserved if AXI_GM_MAX_RD_REQUESTS = 4
     * |        |          |The Bit 19 is reserved if AXI_GM_MAX_RD_REQUESTS !.= 16.
     * |[18]    |RD_OSR_LMT_GT4|Reserved.
     * |[19]    |RD_OSR_LMT_GT8|Reserved.
     * |[21:20] |WR_OSR_LMT|AXI Maximum Write OutStanding Request Limit
     * |[22]    |WR_OSR_LMT_GT4|Reserved.
     * |[23]    |WR_OSR_LMT_GT8|Reserved.
     * |[30]    |LPI_XIT_FRM|Unlock on Magic Packet or Remote Wake Up
     * |        |          |When set to 1, this bit enables the GMAC-AXI to come out of the LPI mode only when the Magic Packet or Remote Wake Up Packet is received
     * |        |          |When set to 0, this bit enables the GMAC-AXI to come out of LPI mode when any frame is received.
     * |[31]    |EN_LPI    |Enable Low Power Interface
     * |        |          |When set to 1, this bit enables the Low Power Interface (LPI) mode and accepts the LPI request from the AXI System Clock controller.
     * |        |          |When set to 0, this bit disables the LPI mode and always denies the LPI request from the AXI System Clock controller.
     * @var GMAC_T::DmaAhbAxiStatus
     * Offset: 0x102C  Register 1011 (AHB or AXI Status Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |AXWHSTS   |AXI Master Write Channel or AHB Master Status
     * |        |          |When high, it indicates that AXI Master's write channel is active and transferring data.
     * |[1]     |AXIRDSTS  |AXI Master Read Channel Status
     * |        |          |When high, it indicates that AXI Master's read channel is active and transferring data.
     * @var GMAC_T::DmaCurrTxDesc
     * Offset: 0x1048  Register 1018 (Current Host Transmit Descriptor Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURTDESAPTR|Host Transmit Descriptor Address Pointer
     * |        |          |Cleared on Reset. Pointer updated by the DMA during operation.
     * @var GMAC_T::DmaCurrRxDesc
     * Offset: 0x104C  Register 1019 (Current Host Receive Descriptor Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURRDESAPTR|Host Receive Descriptor Address Pointer
     * |        |          |Cleared on Reset. Pointer updated by the DMA during operation.
     * @var GMAC_T::DmaCurrTxAddr
     * Offset: 0x1050  Register 1020 (Current Host Transmit Buffer Address Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURTBUFAPTR|Host Transmit Buffer Address Pointer
     * |        |          |Cleared on Reset. Pointer updated by the DMA during operation.
     * @var GMAC_T::DmaCurrRxAddr
     * Offset: 0x1054  Register 1021 (Current Host Receive Buffer Address Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURRBUFAPTR|Host Receive Buffer Address Pointer
     * |        |          |Cleared on Reset. Pointer updated by the DMA during operation.
     * @var GMAC_T::HwFeature
     * Offset: 0x1058  Register 1022 (HW Feature Register)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MIISEL    |10 or 100 Mbps support
     * |[1]     |GMIISEL   |1000 Mbps support
     * |[2]     |HDSEL     |Half-Duplex support
     * |[3]     |EXTHASHEN |Expanded DA Hash Filter
     * |[4]     |HASHSEL   |HASH Filter
     * |[5]     |ADDMACADRSEL|Multiple MAC Address Registers
     * |[6]     |PCSSEL    |PCS registers (TBI, SGMII, or RTBI PHY interface)
     * |[7]     |L3L4FLTREN|Layer 3 and Layer 4 Filter Feature
     * |[8]     |SMASEL    |SMA (MDIO) Interface
     * |[9]     |RWKSEL    |PMT Remote Wakeup
     * |[10]    |MGKSEL    |PMT Magic Packet
     * |[11]    |MMCSEL    |RMON Module
     * |[12]    |TSVER1SEL |Only IEEE 1588-2002 Timestamp
     * |[13]    |TSVER2SEL |IEEE 1588-2008 Advanced Timestamp
     * |[14]    |EEESEL    |Energy Efficient Ethernet
     * |[15]    |AVSEL     |AV Feature
     * |[16]    |TXCOESEL  |Checksum Offload in Tx
     * |[17]    |RXTYP1COE |IP Checksum Offload (Type 1) in Rx
     * |[18]    |RXTYP2COE |IP Checksum Offload (Type 2) in Rx
     * |[19]    |RXFIFOSIZE|Rx FIFO > 2,048 Bytes
     * |[21:20] |RXCHCNT   |Number of additional Rx channels
     * |[23:22] |TXCHCNT   |Number of additional Tx channels
     * |[24]    |ENHDESSEL |Alternate (Enhanced Descriptor)
     * |[25]    |INTTSEN   |Timestamping with Internal System Time
     * |[26]    |FLEXIPPSEN|Flexible Pulse-Per-Second Output
     * |[27]    |SAVLANINS |Source Address or VLAN Insertion
     * |[30:28] |ACTPHYIF  |Active or Selected PHY interface
     * |        |          |When you have multiple PHY interfaces in your configuration, this field indicates the sampled value of phy_intf_sel_i during reset de-assertion
     * |        |          |0001 = RGMII
     * |        |          |0100 = RMII
     * |        |          |All Others = Reserved
     */
    __IO uint32_t Config;                /*!< [0x0000] Register 0 (MAC Configuration Register)                          */
    __IO uint32_t FrameFilter;           /*!< [0x0004] Register 1 (MAC Frame Filter)                                    */
    __I  uint32_t RESERVE0[2];
    __IO uint32_t GmiiAddr;              /*!< [0x0010] Register 4 (GMII Address Register)                               */
    __IO uint32_t GmiiData;              /*!< [0x0014] Register 5 (GMII Data Register)                                  */
    __IO uint32_t FlowControl;           /*!< [0x0018] Register 6 (Flow Control Register)                               */
    __IO uint32_t VlanTag;               /*!< [0x001c] Register 7 (VLAN Tag Register)                                   */
    __IO uint32_t Version;               /*!< [0x0020] Register 8 (Version Register)                                    */
    __I  uint32_t Debug;                 /*!< [0x0024] Register 9 (Debug Register)                                      */
    __I  uint32_t RESERVE1[1];
    __IO uint32_t PmtCtrlStatus;         /*!< [0x002c] Register 11 (PMT Control and Status Register)                    */
    __IO uint32_t LpiCtrlStatus;         /*!< [0x0030] Register 12 (LPI Control and Status Register)                    */
    __IO uint32_t LpiTimerCtrl;          /*!< [0x0034] Register 13 (LPI Timers Control Register)                        */
    __I  uint32_t IntStatus;             /*!< [0x0038] Register 14 (Interrupt Register)                                 */
    __IO uint32_t IntMask;               /*!< [0x003c] Register 15 (Interrupt Mask Register)                            */
    __IO uint32_t Addr0High;             /*!< [0x0040] Register 16 (MAC Address0 High Register)                         */
    __IO uint32_t Addr0Low;              /*!< [0x0044] Register 17 (MAC Address0 Low Register)                          */
    __IO uint32_t Addr1High;             /*!< [0x0048] Register 18 (MAC Address1 High Register)                         */
    __IO uint32_t Addr1Low;              /*!< [0x004c] Register 19 (MAC Address1 Low Register)                          */
    __IO uint32_t Addr2High;             /*!< [0x0050] Register 20 (MAC Address2 High Register)                         */
    __IO uint32_t Addr2Low;              /*!< [0x0054] Register 21 (MAC Address2 Low Register)                          */
    __IO uint32_t Addr3High;             /*!< [0x0058] Register 22 (MAC Address3 High Register)                         */
    __IO uint32_t Addr3Low;              /*!< [0x005c] Register 23 (MAC Address3 Low Register)                          */
    __IO uint32_t Addr4High;             /*!< [0x0060] Register 24 (MAC Address4 High Register)                         */
    __IO uint32_t Addr4Low;              /*!< [0x0064] Register 25 (MAC Address4 Low Register)                          */
    __IO uint32_t Addr5High;             /*!< [0x0068] Register 26 (MAC Address5 High Register)                         */
    __IO uint32_t Addr5Low;              /*!< [0x006c] Register 27 (MAC Address5 Low Register)                          */
    __IO uint32_t Addr6High;             /*!< [0x0070] Register 28 (MAC Address6 High Register)                         */
    __IO uint32_t Addr6Low;              /*!< [0x0074] Register 29 (MAC Address6 Low Register)                          */
    __IO uint32_t Addr7High;             /*!< [0x0078] Register 30 (MAC Address7 High Register)                         */
    __IO uint32_t Addr7Low;              /*!< [0x007c] Register 31 (MAC Address7 Low Register)                          */
    __IO uint32_t Addr8High;             /*!< [0x0080] Register 32 (MAC Address8 High Register)                         */
    __IO uint32_t Addr8Low;              /*!< [0x0084] Register 33 (MAC Address8 Low Register)                          */
    __I  uint32_t RESERVE2[20];
    __I  uint32_t RgmiiCtrlSts;          /*!< [0x00d8] Register 54 (RGMII Status Register)                              */
    __IO uint32_t Timeout;               /*!< [0x00dc] Register 55 (Watchdog Timeout Register) This register controls the watchdog timeout for received frames */
    __I  uint32_t RESERVE3[297];
    __IO uint32_t VlanIncRep;            /*!< [0x0584] Register 353 (VLAN Tag Inclusion or Replacement Register)        */
    __I  uint32_t RESERVE4[94];
    __IO uint32_t TSControl;             /*!< [0x0700] Register 448 (Timestamp Control Register)                        */
    __IO uint32_t TSSubSecIncr;          /*!< [0x0704] Register 449 (Sub-Second Increment Register)                     */
    __I  uint32_t TSSec;                 /*!< [0x0708] Register 450 (System Time - Seconds Register)                    */
    __I  uint32_t TSNanosec;             /*!< [0x070c] Register 451 (System Time - Nanoseconds Register)                */
    __IO uint32_t TSSecUpdate;           /*!< [0x0710] Register 452 (System Time - Seconds Update Register)             */
    __IO uint32_t TSNanosecUpdate;       /*!< [0x0714] Register 453 (System Time - Nanoseconds Update Register)         */
    __IO uint32_t TSAddend;              /*!< [0x0718] Register 454 (Timestamp Addend Register)                         */
    __IO uint32_t TSTargetSec;           /*!< [0x071c] Register 455 (Target Time Seconds Register)                      */
    __IO uint32_t TSTargetNanosec;       /*!< [0x0720] Register 456 (Target Time Nanoseconds Register)                  */
    __IO uint32_t TSSecHigher;           /*!< [0x0724] Register 457 (System Time - Higher Word Seconds Register)        */
    __I  uint32_t TSStatus;              /*!< [0x0728] Register 458 (Timestamp Status Register)                         */
    __IO uint32_t PPSCtrl;               /*!< [0x072c] Register 459 (PPS Control Register)                              */
    __I  uint32_t RESERVE5[12];
    __IO uint32_t PPSInterval;           /*!< [0x0760] Register 472 (PPS0 Interval Register)                            */
    __IO uint32_t PPSWidth;              /*!< [0x0764] Register 473 (PPS0 Width Register)                               */
    __I  uint32_t RESERVE6[550];
    __IO uint32_t DmaBusMode;            /*!< [0x1000] Register 1000 (Bus Mode Register)                                */
    __IO uint32_t DmaTxPollDemand;       /*!< [0x1004] Register 1001 (Transmit Poll Demand Register)                    */
    __IO uint32_t DmaRxPollDemand;       /*!< [0x1008] Register 1002 (Receive Poll Demand Register)                     */
    __IO uint32_t DmaRxDesc;             /*!< [0x100c] Register 1003 (Receive Descriptor List Address Register)         */
    __IO uint32_t DmaTxDesc;             /*!< [0x1010] Register 1004 (Transmit Descriptor List Address Register)        */
    __IO uint32_t DmaStatus;             /*!< [0x1014] Register 1005 (Status Register)                                  */
    __IO uint32_t DmaOpMode;             /*!< [0x1018] Register 1006 (Operation Mode Register)                          */
    __IO uint32_t DmaInt;                /*!< [0x101c] Register 1007 (Interrupt Enable Register)                        */
    __I  uint32_t DmaMissedFrame;        /*!< [0x1020] Register 1008 (Missed Frame and Buffer Overflow Counter Register) */
    __IO uint32_t DmaRxIntWdt;           /*!< [0x1024] Register 1009 (Receive Interrupt Watchdog Timer Register)        */
    __IO uint32_t DmaAxiBusMode;         /*!< [0x1028] Register 1010 (AXI Bus Mode Register)                            */
    __I  uint32_t DmaAhbAxiStatus;       /*!< [0x102c] Register 1011 (AHB or AXI Status Register)                       */
    __I  uint32_t RESERVE7[6];
    __I  uint32_t DmaCurrTxDesc;         /*!< [0x1048] Register 1018 (Current Host Transmit Descriptor Register)        */
    __I  uint32_t DmaCurrRxDesc;         /*!< [0x104c] Register 1019 (Current Host Receive Descriptor Register)         */
    __I  uint32_t DmaCurrTxAddr;         /*!< [0x1050] Register 1020 (Current Host Transmit Buffer Address Register)    */
    __I  uint32_t DmaCurrRxAddr;         /*!< [0x1054] Register 1021 (Current Host Receive Buffer Address Register)     */
    __I  uint32_t HwFeature;             /*!< [0x1058] Register 1022 (HW Feature Register)                              */

} GMAC_T;

/**  @addtogroup GMAC Bit Field Definition
    Constant Definitions for GMAC
@{ */

#define GMAC_Config_PRELEN_Pos              (0)                                             /*!< GMAC_T::Config: PRELEN Position       */
#define GMAC_Config_PRELEN_Msk              (0x3ul << GMAC_Config_PRELEN_Pos)               /*!< GMAC_T::Config: PRELEN Mask           */

#define GMAC_Config_RE_Pos                  (2)                                             /*!< GMAC_T::Config: RE Position           */
#define GMAC_Config_RE_Msk                  (0x1ul << GMAC_Config_RE_Pos)                   /*!< GMAC_T::Config: RE Mask               */

#define GMAC_Config_TE_Pos                  (3)                                             /*!< GMAC_T::Config: TE Position           */
#define GMAC_Config_TE_Msk                  (0x1ul << GMAC_Config_TE_Pos)                   /*!< GMAC_T::Config: TE Mask               */

#define GMAC_Config_DC_Pos                  (4)                                             /*!< GMAC_T::Config: DC Position           */
#define GMAC_Config_DC_Msk                  (0x1ul << GMAC_Config_DC_Pos)                   /*!< GMAC_T::Config: DC Mask               */

#define GMAC_Config_BL_Pos                  (5)                                             /*!< GMAC_T::Config: BL Position           */
#define GMAC_Config_BL_Msk                  (0x3ul << GMAC_Config_BL_Pos)                   /*!< GMAC_T::Config: BL Mask               */

#define GMAC_Config_ACS_Pos                 (7)                                             /*!< GMAC_T::Config: ACS Position          */
#define GMAC_Config_ACS_Msk                 (0x1ul << GMAC_Config_ACS_Pos)                  /*!< GMAC_T::Config: ACS Mask              */

#define GMAC_Config_LUD_Pos                 (8)                                             /*!< GMAC_T::Config: LUD Position          */
#define GMAC_Config_LUD_Msk                 (0x1ul << GMAC_Config_LUD_Pos)                  /*!< GMAC_T::Config: LUD Mask              */

#define GMAC_Config_DR_Pos                  (9)                                             /*!< GMAC_T::Config: DR Position           */
#define GMAC_Config_DR_Msk                  (0x1ul << GMAC_Config_DR_Pos)                   /*!< GMAC_T::Config: DR Mask               */

#define GMAC_Config_IPC_Pos                 (10)                                            /*!< GMAC_T::Config: IPC Position          */
#define GMAC_Config_IPC_Msk                 (0x1ul << GMAC_Config_IPC_Pos)                  /*!< GMAC_T::Config: IPC Mask              */

#define GMAC_Config_DM_Pos                  (11)                                            /*!< GMAC_T::Config: DM Position           */
#define GMAC_Config_DM_Msk                  (0x1ul << GMAC_Config_DM_Pos)                   /*!< GMAC_T::Config: DM Mask               */

#define GMAC_Config_LM_Pos                  (12)                                            /*!< GMAC_T::Config: LM Position           */
#define GMAC_Config_LM_Msk                  (0x1ul << GMAC_Config_LM_Pos)                   /*!< GMAC_T::Config: LM Mask               */

#define GMAC_Config_DO_Pos                  (13)                                            /*!< GMAC_T::Config: DO Position           */
#define GMAC_Config_DO_Msk                  (0x1ul << GMAC_Config_DO_Pos)                   /*!< GMAC_T::Config: DO Mask               */

#define GMAC_Config_FES_Pos                 (14)                                            /*!< GMAC_T::Config: FES Position          */
#define GMAC_Config_FES_Msk                 (0x1ul << GMAC_Config_FES_Pos)                  /*!< GMAC_T::Config: FES Mask              */

#define GMAC_Config_PS_Pos                  (15)                                            /*!< GMAC_T::Config: PS Position           */
#define GMAC_Config_PS_Msk                  (0x1ul << GMAC_Config_PS_Pos)                   /*!< GMAC_T::Config: PS Mask               */

#define GMAC_Config_DCRS_Pos                (16)                                            /*!< GMAC_T::Config: DCRS Position         */
#define GMAC_Config_DCRS_Msk                (0x1ul << GMAC_Config_DCRS_Pos)                 /*!< GMAC_T::Config: DCRS Mask             */

#define GMAC_Config_IFG_Pos                 (17)                                            /*!< GMAC_T::Config: IFG Position          */
#define GMAC_Config_IFG_Msk                 (0x7ul << GMAC_Config_IFG_Pos)                  /*!< GMAC_T::Config: IFG Mask              */

#define GMAC_Config_JE_Pos                  (20)                                            /*!< GMAC_T::Config: JE Position           */
#define GMAC_Config_JE_Msk                  (0x1ul << GMAC_Config_JE_Pos)                   /*!< GMAC_T::Config: JE Mask               */

#define GMAC_Config_BE_Pos                  (21)                                            /*!< GMAC_T::Config: BE Position           */
#define GMAC_Config_BE_Msk                  (0x1ul << GMAC_Config_BE_Pos)                   /*!< GMAC_T::Config: BE Mask               */

#define GMAC_Config_JD_Pos                  (22)                                            /*!< GMAC_T::Config: JD Position           */
#define GMAC_Config_JD_Msk                  (0x1ul << GMAC_Config_JD_Pos)                   /*!< GMAC_T::Config: JD Mask               */

#define GMAC_Config_WD_Pos                  (23)                                            /*!< GMAC_T::Config: WD Position           */
#define GMAC_Config_WD_Msk                  (0x1ul << GMAC_Config_WD_Pos)                   /*!< GMAC_T::Config: WD Mask               */

#define GMAC_Config_TC_Pos                  (24)                                            /*!< GMAC_T::Config: TC Position           */
#define GMAC_Config_TC_Msk                  (0x1ul << GMAC_Config_TC_Pos)                   /*!< GMAC_T::Config: TC Mask               */

#define GMAC_Config_CST_Pos                 (25)                                            /*!< GMAC_T::Config: CST Position          */
#define GMAC_Config_CST_Msk                 (0x1ul << GMAC_Config_CST_Pos)                  /*!< GMAC_T::Config: CST Mask              */

#define GMAC_Config_TWOKPE_Pos              (27)                                            /*!< GMAC_T::Config: TWOKPE Position       */
#define GMAC_Config_TWOKPE_Msk              (0x1ul << GMAC_Config_TWOKPE_Pos)               /*!< GMAC_T::Config: TWOKPE Mask           */

#define GMAC_Config_SARC_Pos                (28)                                            /*!< GMAC_T::Config: SARC Position         */
#define GMAC_Config_SARC_Msk                (0x7ul << GMAC_Config_SARC_Pos)                 /*!< GMAC_T::Config: SARC Mask             */

#define GMAC_FrameFilter_PR_Pos             (0)                                             /*!< GMAC_T::FrameFilter: PR Position      */
#define GMAC_FrameFilter_PR_Msk             (0x1ul << GMAC_FrameFilter_PR_Pos)              /*!< GMAC_T::FrameFilter: PR Mask          */

#define GMAC_FrameFilter_DAIF_Pos           (3)                                             /*!< GMAC_T::FrameFilter: DAIF Position    */
#define GMAC_FrameFilter_DAIF_Msk           (0x1ul << GMAC_FrameFilter_DAIF_Pos)            /*!< GMAC_T::FrameFilter: DAIF Mask        */

#define GMAC_FrameFilter_PM_Pos             (4)                                             /*!< GMAC_T::FrameFilter: PM Position      */
#define GMAC_FrameFilter_PM_Msk             (0x1ul << GMAC_FrameFilter_PM_Pos)              /*!< GMAC_T::FrameFilter: PM Mask          */

#define GMAC_FrameFilter_DBF_Pos            (5)                                             /*!< GMAC_T::FrameFilter: DBF Position     */
#define GMAC_FrameFilter_DBF_Msk            (0x1ul << GMAC_FrameFilter_DBF_Pos)             /*!< GMAC_T::FrameFilter: DBF Mask         */

#define GMAC_FrameFilter_PCF_Pos            (6)                                             /*!< GMAC_T::FrameFilter: PCF Position     */
#define GMAC_FrameFilter_PCF_Msk            (0x3ul << GMAC_FrameFilter_PCF_Pos)             /*!< GMAC_T::FrameFilter: PCF Mask         */

#define GMAC_FrameFilter_SAIF_Pos           (8)                                             /*!< GMAC_T::FrameFilter: SAIF Position    */
#define GMAC_FrameFilter_SAIF_Msk           (0x1ul << GMAC_FrameFilter_SAIF_Pos)            /*!< GMAC_T::FrameFilter: SAIF Mask        */

#define GMAC_FrameFilter_SAF_Pos            (9)                                             /*!< GMAC_T::FrameFilter: SAF Position     */
#define GMAC_FrameFilter_SAF_Msk            (0x1ul << GMAC_FrameFilter_SAF_Pos)             /*!< GMAC_T::FrameFilter: SAF Mask         */

#define GMAC_FrameFilter_VTFE_Pos           (16)                                            /*!< GMAC_T::FrameFilter: VTFE Position    */
#define GMAC_FrameFilter_VTFE_Msk           (0x1ul << GMAC_FrameFilter_VTFE_Pos)            /*!< GMAC_T::FrameFilter: VTFE Mask        */

#define GMAC_FrameFilter_RA_Pos             (31)                                            /*!< GMAC_T::FrameFilter: RA Position      */
#define GMAC_FrameFilter_RA_Msk             (0x1ul << GMAC_FrameFilter_RA_Pos)              /*!< GMAC_T::FrameFilter: RA Mask          */

#define GMAC_GmiiAddr_GB_Pos                (0)                                             /*!< GMAC_T::GmiiAddr: GB Position           */
#define GMAC_GmiiAddr_GB_Msk                (0x1ul << GMAC_GmiiAddr_GB_Pos)                 /*!< GMAC_T::GmiiAddr: GB Mask               */

#define GMAC_GmiiAddr_GW_Pos                (1)                                             /*!< GMAC_T::GmiiAddr: GW Position           */
#define GMAC_GmiiAddr_GW_Msk                (0x1ul << GMAC_GmiiAddr_GW_Pos)                 /*!< GMAC_T::GmiiAddr: GW Mask               */

#define GMAC_GmiiAddr_CR_Pos                (2)                                             /*!< GMAC_T::GmiiAddr: CR Position           */
#define GMAC_GmiiAddr_CR_Msk                (0xful << GMAC_GmiiAddr_CR_Pos)                 /*!< GMAC_T::GmiiAddr: CR Mask               */

#define GMAC_GmiiAddr_GR_Pos                (6)                                             /*!< GMAC_T::GmiiAddr: GR Position           */
#define GMAC_GmiiAddr_GR_Msk                (0x1ful << GMAC_GmiiAddr_GR_Pos)                /*!< GMAC_T::GmiiAddr: GR Mask               */

#define GMAC_GmiiAddr_PA_Pos                (11)                                            /*!< GMAC_T::GmiiAddr: PA Position           */
#define GMAC_GmiiAddr_PA_Msk                (0x1ful << GMAC_GmiiAddr_PA_Pos)                /*!< GMAC_T::GmiiAddr: PA Mask               */

#define GMAC_GmiiData_GD_Pos                (0)                                             /*!< GMAC_T::GmiiData: GD Position              */
#define GMAC_GmiiData_GD_Msk                (0xfffful << GMAC_GmiiData_GD_Pos)              /*!< GMAC_T::GmiiData: GD Mask                  */

#define GMAC_FlowControl_FCA_BPA_Pos        (0)                                             /*!< GMAC_T::FlowControl: FCA_BPA Position      */
#define GMAC_FlowControl_FCA_BPA_Msk        (0x1ul << GMAC_FlowControl_FCA_BPA_Pos)         /*!< GMAC_T::FlowControl: FCA_BPA Mask          */

#define GMAC_FlowControl_TFE_Pos            (1)                                             /*!< GMAC_T::FlowControl: TFE Position          */
#define GMAC_FlowControl_TFE_Msk            (0x1ul << GMAC_FlowControl_TFE_Pos)             /*!< GMAC_T::FlowControl: TFE Mask              */

#define GMAC_FlowControl_RFE_Pos            (2)                                             /*!< GMAC_T::FlowControl: RFE Position          */
#define GMAC_FlowControl_RFE_Msk            (0x1ul << GMAC_FlowControl_RFE_Pos)             /*!< GMAC_T::FlowControl: RFE Mask              */

#define GMAC_FlowControl_UP_Pos             (3)                                             /*!< GMAC_T::FlowControl: UP Position           */
#define GMAC_FlowControl_UP_Msk             (0x1ul << GMAC_FlowControl_UP_Pos)              /*!< GMAC_T::FlowControl: UP Mask               */

#define GMAC_FlowControl_PLT_Pos            (4)                                             /*!< GMAC_T::FlowControl: PLT Position          */
#define GMAC_FlowControl_PLT_Msk            (0x3ul << GMAC_FlowControl_PLT_Pos)             /*!< GMAC_T::FlowControl: PLT Mask              */

#define GMAC_FlowControl_DZPQ_Pos           (7)                                             /*!< GMAC_T::FlowControl: DZPQ Position         */
#define GMAC_FlowControl_DZPQ_Msk           (0x1ul << GMAC_FlowControl_DZPQ_Pos)            /*!< GMAC_T::FlowControl: DZPQ Mask             */

#define GMAC_FlowControl_PT_Pos             (16)                                            /*!< GMAC_T::FlowControl: PT Position           */
#define GMAC_FlowControl_PT_Msk             (0xfffful << GMAC_FlowControl_PT_Pos)           /*!< GMAC_T::FlowControl: PT Mask               */

#define GMAC_VlanTag_VL_Pos                 (0)                                             /*!< GMAC_T::VlanTag: VL Position               */
#define GMAC_VlanTag_VL_Msk                 (0xfffful << GMAC_VlanTag_VL_Pos)               /*!< GMAC_T::VlanTag: VL Mask                   */

#define GMAC_VlanTag_ETV_Pos                (16)                                            /*!< GMAC_T::VlanTag: ETV Position              */
#define GMAC_VlanTag_ETV_Msk                (0x1ul << GMAC_VlanTag_ETV_Pos)                 /*!< GMAC_T::VlanTag: ETV Mask                  */

#define GMAC_VlanTag_VTIM_Pos               (17)                                            /*!< GMAC_T::VlanTag: VTIM Position             */
#define GMAC_VlanTag_VTIM_Msk               (0x1ul << GMAC_VlanTag_VTIM_Pos)                /*!< GMAC_T::VlanTag: VTIM Mask                 */

#define GMAC_VlanTag_ESVL_Pos               (18)                                            /*!< GMAC_T::VlanTag: ESVL Position             */
#define GMAC_VlanTag_ESVL_Msk               (0x1ul << GMAC_VlanTag_ESVL_Pos)                /*!< GMAC_T::VlanTag: ESVL Mask                 */

#define GMAC_Debug_RPESTS_Pos               (0)                                             /*!< GMAC_T::Debug: RPESTS Position         */
#define GMAC_Debug_RPESTS_Msk               (0x1ul << GMAC_Debug_RPESTS_Pos)                /*!< GMAC_T::Debug: RPESTS Mask             */

#define GMAC_Debug_RFCFCSTS_Pos             (1)                                             /*!< GMAC_T::Debug: RFCFCSTS Position       */
#define GMAC_Debug_RFCFCSTS_Msk             (0x3ul << GMAC_Debug_RFCFCSTS_Pos)              /*!< GMAC_T::Debug: RFCFCSTS Mask           */

#define GMAC_Debug_RWCSTS_Pos               (4)                                             /*!< GMAC_T::Debug: RWCSTS Position         */
#define GMAC_Debug_RWCSTS_Msk               (0x1ul << GMAC_Debug_RWCSTS_Pos)                /*!< GMAC_T::Debug: RWCSTS Mask             */

#define GMAC_Debug_RRCSTS_Pos               (5)                                             /*!< GMAC_T::Debug: RRCSTS Position         */
#define GMAC_Debug_RRCSTS_Msk               (0x3ul << GMAC_Debug_RRCSTS_Pos)                /*!< GMAC_T::Debug: RRCSTS Mask             */

#define GMAC_Debug_RXFSTS_Pos               (8)                                             /*!< GMAC_T::Debug: RXFSTS Position         */
#define GMAC_Debug_RXFSTS_Msk               (0x3ul << GMAC_Debug_RXFSTS_Pos)                /*!< GMAC_T::Debug: RXFSTS Mask             */

#define GMAC_Debug_TPESTS_Pos               (16)                                            /*!< GMAC_T::Debug: TPESTS Position         */
#define GMAC_Debug_TPESTS_Msk               (0x1ul << GMAC_Debug_TPESTS_Pos)                /*!< GMAC_T::Debug: TPESTS Mask             */

#define GMAC_Debug_TFCSTS_Pos               (17)                                            /*!< GMAC_T::Debug: TFCSTS Position         */
#define GMAC_Debug_TFCSTS_Msk               (0x3ul << GMAC_Debug_TFCSTS_Pos)                /*!< GMAC_T::Debug: TFCSTS Mask             */

#define GMAC_Debug_TXPAUSED_Pos             (19)                                            /*!< GMAC_T::Debug: TXPAUSED Position       */
#define GMAC_Debug_TXPAUSED_Msk             (0x1ul << GMAC_Debug_TXPAUSED_Pos)              /*!< GMAC_T::Debug: TXPAUSED Mask           */

#define GMAC_Debug_TRCSTS_Pos               (20)                                            /*!< GMAC_T::Debug: TRCSTS Position         */
#define GMAC_Debug_TRCSTS_Msk               (0x3ul << GMAC_Debug_TRCSTS_Pos)                /*!< GMAC_T::Debug: TRCSTS Mask             */

#define GMAC_Debug_TWCSTS_Pos               (22)                                            /*!< GMAC_T::Debug: TWCSTS Position         */
#define GMAC_Debug_TWCSTS_Msk               (0x1ul << GMAC_Debug_TWCSTS_Pos)                /*!< GMAC_T::Debug: TWCSTS Mask             */

#define GMAC_Debug_TXFSTS_Pos               (24)                                            /*!< GMAC_T::Debug: TXFSTS Position         */
#define GMAC_Debug_TXFSTS_Msk               (0x1ul << GMAC_Debug_TXFSTS_Pos)                /*!< GMAC_T::Debug: TXFSTS Mask             */

#define GMAC_Debug_TXSTSFSTS_Pos            (25)                                            /*!< GMAC_T::Debug: TXSTSFSTS Position      */
#define GMAC_Debug_TXSTSFSTS_Msk            (0x1ul << GMAC_Debug_TXSTSFSTS_Pos)             /*!< GMAC_T::Debug: TXSTSFSTS Mask          */

#define GMAC_PmtCtrlStatus_PWRDWN_Pos       (0)                                             /*!< GMAC_T::PmtCtrlStatus: PWRDWN Position*/
#define GMAC_PmtCtrlStatus_PWRDWN_Msk       (0x1ul << GMAC_PmtCtrlStatus_PWRDWN_Pos)        /*!< GMAC_T::PmtCtrlStatus: PWRDWN Mask    */

#define GMAC_PmtCtrlStatus_MGKPKTEN_Pos     (1)                                             /*!< GMAC_T::PmtCtrlStatus: MGKPKTEN Position*/
#define GMAC_PmtCtrlStatus_MGKPKTEN_Msk     (0x1ul << GMAC_PmtCtrlStatus_MGKPKTEN_Pos)      /*!< GMAC_T::PmtCtrlStatus: MGKPKTEN Mask  */

#define GMAC_PmtCtrlStatus_MGKPRCVD_Pos     (5)                                             /*!< GMAC_T::PmtCtrlStatus: MGKPRCVD Position*/
#define GMAC_PmtCtrlStatus_MGKPRCVD_Msk     (0x1ul << GMAC_PmtCtrlStatus_MGKPRCVD_Pos)      /*!< GMAC_T::PmtCtrlStatus: MGKPRCVD Mask  */

#define GMAC_LpiCtrlStatus_TLPIEN_Pos       (0)                                             /*!< GMAC_T::LpiCtrlStatus: TLPIEN Position*/
#define GMAC_LpiCtrlStatus_TLPIEN_Msk       (0x1ul << GMAC_LpiCtrlStatus_TLPIEN_Pos)        /*!< GMAC_T::LpiCtrlStatus: TLPIEN Mask    */

#define GMAC_LpiCtrlStatus_TLPIEX_Pos       (1)                                             /*!< GMAC_T::LpiCtrlStatus: TLPIEX Position*/
#define GMAC_LpiCtrlStatus_TLPIEX_Msk       (0x1ul << GMAC_LpiCtrlStatus_TLPIEX_Pos)        /*!< GMAC_T::LpiCtrlStatus: TLPIEX Mask    */

#define GMAC_LpiCtrlStatus_RLPIEN_Pos       (2)                                             /*!< GMAC_T::LpiCtrlStatus: RLPIEN Position*/
#define GMAC_LpiCtrlStatus_RLPIEN_Msk       (0x1ul << GMAC_LpiCtrlStatus_RLPIEN_Pos)        /*!< GMAC_T::LpiCtrlStatus: RLPIEN Mask    */

#define GMAC_LpiCtrlStatus_RLPIEX_Pos       (3)                                             /*!< GMAC_T::LpiCtrlStatus: RLPIEX Position*/
#define GMAC_LpiCtrlStatus_RLPIEX_Msk       (0x1ul << GMAC_LpiCtrlStatus_RLPIEX_Pos)        /*!< GMAC_T::LpiCtrlStatus: RLPIEX Mask    */

#define GMAC_LpiCtrlStatus_TLPIST_Pos       (8)                                             /*!< GMAC_T::LpiCtrlStatus: TLPIST Position*/
#define GMAC_LpiCtrlStatus_TLPIST_Msk       (0x1ul << GMAC_LpiCtrlStatus_TLPIST_Pos)        /*!< GMAC_T::LpiCtrlStatus: TLPIST Mask    */

#define GMAC_LpiCtrlStatus_RLPIST_Pos       (9)                                             /*!< GMAC_T::LpiCtrlStatus: RLPIST Position*/
#define GMAC_LpiCtrlStatus_RLPIST_Msk       (0x1ul << GMAC_LpiCtrlStatus_RLPIST_Pos)        /*!< GMAC_T::LpiCtrlStatus: RLPIST Mask    */

#define GMAC_LpiCtrlStatus_LPIEN_Pos        (16)                                            /*!< GMAC_T::LpiCtrlStatus: LPIEN Position */
#define GMAC_LpiCtrlStatus_LPIEN_Msk        (0x1ul << GMAC_LpiCtrlStatus_LPIEN_Pos)         /*!< GMAC_T::LpiCtrlStatus: LPIEN Mask     */

#define GMAC_LpiCtrlStatus_PLS_Pos          (17)                                            /*!< GMAC_T::LpiCtrlStatus: PLS Position   */
#define GMAC_LpiCtrlStatus_PLS_Msk          (0x1ul << GMAC_LpiCtrlStatus_PLS_Pos)           /*!< GMAC_T::LpiCtrlStatus: PLS Mask       */

#define GMAC_LpiCtrlStatus_PLSEN_Pos        (18)                                            /*!< GMAC_T::LpiCtrlStatus: PLSEN Position */
#define GMAC_LpiCtrlStatus_PLSEN_Msk        (0x1ul << GMAC_LpiCtrlStatus_PLSEN_Pos)         /*!< GMAC_T::LpiCtrlStatus: PLSEN Mask     */

#define GMAC_LpiCtrlStatus_LPITXA_Pos       (19)                                            /*!< GMAC_T::LpiCtrlStatus: LPITXA Position*/
#define GMAC_LpiCtrlStatus_LPITXA_Msk       (0x1ul << GMAC_LpiCtrlStatus_LPITXA_Pos)        /*!< GMAC_T::LpiCtrlStatus: LPITXA Mask    */

#define GMAC_LpiTimerCtrl_TWT_Pos           (0)                                             /*!< GMAC_T::LpiTimerCtrl: TWT Position   */
#define GMAC_LpiTimerCtrl_TWT_Msk           (0xfffful << GMAC_LpiTimerCtrl_TWT_Pos)         /*!< GMAC_T::LpiTimerCtrl: TWT Mask       */

#define GMAC_LpiTimerCtrl_LST_Pos           (16)                                            /*!< GMAC_T::LpiTimerCtrl: LST Position   */
#define GMAC_LpiTimerCtrl_LST_Msk           (0x3fful << GMAC_LpiTimerCtrl_LST_Pos)          /*!< GMAC_T::LpiTimerCtrl: LST Mask       */

#define GMAC_IntStatus_RGSMIIIS_Pos         (0)                                             /*!< GMAC_T::IntStatus: RGSMIIIS Position      */
#define GMAC_IntStatus_RGSMIIIS_Msk         (0x1ul << GMAC_IntStatus_RGSMIIIS_Pos)          /*!< GMAC_T::IntStatus: RGSMIIIS Mask          */

#define GMAC_IntStatus_PMTIS_Pos            (3)                                             /*!< GMAC_T::IntStatus: PMTIS Position         */
#define GMAC_IntStatus_PMTIS_Msk            (0x1ul << GMAC_IntStatus_PMTIS_Pos)             /*!< GMAC_T::IntStatus: PMTIS Mask             */

#define GMAC_IntStatus_TSIS_Pos             (9)                                             /*!< GMAC_T::IntStatus: TSIS Position          */
#define GMAC_IntStatus_TSIS_Msk             (0x1ul << GMAC_IntStatus_TSIS_Pos)              /*!< GMAC_T::IntStatus: TSIS Mask              */

#define GMAC_IntStatus_LPIIS_Pos            (10)                                            /*!< GMAC_T::IntStatus: LPIIS Position         */
#define GMAC_IntStatus_LPIIS_Msk            (0x1ul << GMAC_IntStatus_LPIIS_Pos)             /*!< GMAC_T::IntStatus: LPIIS Mask             */

#define GMAC_IntMask_RGSMIIIM_Pos           (0)                                             /*!< GMAC_T::IntMask: RGSMIIIM Position        */
#define GMAC_IntMask_RGSMIIIM_Msk           (0x1ul << GMAC_IntMask_RGSMIIIM_Pos)            /*!< GMAC_T::IntMask: RGSMIIIM Mask            */

#define GMAC_IntMask_PCSLCHGIM_Pos          (1)                                             /*!< GMAC_T::IntMask: PCSLCHGIM Position       */
#define GMAC_IntMask_PCSLCHGIM_Msk          (0x1ul << GMAC_IntMask_PCSLCHGIM_Pos)           /*!< GMAC_T::IntMask: PCSLCHGIM Mask           */

#define GMAC_IntMask_PCSANCIM_Pos           (2)                                             /*!< GMAC_T::IntMask: PCSANCIM Position        */
#define GMAC_IntMask_PCSANCIM_Msk           (0x1ul << GMAC_IntMask_PCSANCIM_Pos)            /*!< GMAC_T::IntMask: PCSANCIM Mask            */

#define GMAC_IntMask_PMTIM_Pos              (3)                                             /*!< GMAC_T::IntMask: PMTIM Position           */
#define GMAC_IntMask_PMTIM_Msk              (0x1ul << GMAC_IntMask_PMTIM_Pos)               /*!< GMAC_T::IntMask: PMTIM Mask               */

#define GMAC_IntMask_TSIM_Pos               (9)                                             /*!< GMAC_T::IntMask: TSIM Position            */
#define GMAC_IntMask_TSIM_Msk               (0x1ul << GMAC_IntMask_TSIM_Pos)                /*!< GMAC_T::IntMask: TSIM Mask                */

#define GMAC_IntMask_LPIIM_Pos              (10)                                            /*!< GMAC_T::IntMask: LPIIM Position           */
#define GMAC_IntMask_LPIIM_Msk              (0x1ul << GMAC_IntMask_LPIIM_Pos)               /*!< GMAC_T::IntMask: LPIIM Mask               */

#define GMAC_Addr0High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr0High: ADDRHI Position */
#define GMAC_Addr0High_ADDRHI_Msk           (0xfffful << GMAC_Addr0High_ADDRHI_Pos)         /*!< GMAC_T::Addr0High: ADDRHI Mask     */

#define GMAC_Addr0High_AE_Pos               (31)                                            /*!< GMAC_T::Addr0High: AE Position     */
#define GMAC_Addr0High_AE_Msk               (0x1ul << GMAC_Addr0High_AE_Pos)                /*!< GMAC_T::Addr0High: AE Mask         */

#define GMAC_Addr0Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr0Low: ADDRLO Position  */
#define GMAC_Addr0Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr0Low_ADDRLO_Pos)      /*!< GMAC_T::Addr0Low: ADDRLO Mask      */

#define GMAC_Addr1High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr1High: ADDRHI Position */
#define GMAC_Addr1High_ADDRHI_Msk           (0xfffful << GMAC_Addr1High_ADDRHI_Pos)         /*!< GMAC_T::Addr1High: ADDRHI Mask     */

#define GMAC_Addr1High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr1High: MBC Position    */
#define GMAC_Addr1High_MBC_Msk              (0x3ful << GMAC_Addr1High_MBC_Pos)              /*!< GMAC_T::Addr1High: MBC Mask        */

#define GMAC_Addr1High_SA_Pos               (30)                                            /*!< GMAC_T::Addr1High: SA Position     */
#define GMAC_Addr1High_SA_Msk               (0x1ul << GMAC_Addr1High_SA_Pos)                /*!< GMAC_T::Addr1High: SA Mask         */

#define GMAC_Addr1High_AE_Pos               (31)                                            /*!< GMAC_T::Addr1High: AE Position     */
#define GMAC_Addr1High_AE_Msk               (0x1ul << GMAC_Addr1High_AE_Pos)                /*!< GMAC_T::Addr1High: AE Mask         */

#define GMAC_Addr1Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr1Low: ADDRLO Position  */
#define GMAC_Addr1Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr1Low_ADDRLO_Pos)      /*!< GMAC_T::Addr1Low: ADDRLO Mask      */

#define GMAC_Addr2High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr2High: ADDRHI Position */
#define GMAC_Addr2High_ADDRHI_Msk           (0xfffful << GMAC_Addr2High_ADDRHI_Pos)         /*!< GMAC_T::Addr2High: ADDRHI Mask     */

#define GMAC_Addr2High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr2High: MBC Position    */
#define GMAC_Addr2High_MBC_Msk              (0x3ful << GMAC_Addr2High_MBC_Pos)              /*!< GMAC_T::Addr2High: MBC Mask        */

#define GMAC_Addr2High_SA_Pos               (30)                                            /*!< GMAC_T::Addr2High: SA Position     */
#define GMAC_Addr2High_SA_Msk               (0x1ul << GMAC_Addr2High_SA_Pos)                /*!< GMAC_T::Addr2High: SA Mask         */

#define GMAC_Addr2High_AE_Pos               (31)                                            /*!< GMAC_T::Addr2High: AE Position     */
#define GMAC_Addr2High_AE_Msk               (0x1ul << GMAC_Addr2High_AE_Pos)                /*!< GMAC_T::Addr2High: AE Mask         */

#define GMAC_Addr2Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr2Low: ADDRLO Position  */
#define GMAC_Addr2Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr2Low_ADDRLO_Pos)      /*!< GMAC_T::Addr2Low: ADDRLO Mask      */

#define GMAC_Addr3High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr3High: ADDRHI Position */
#define GMAC_Addr3High_ADDRHI_Msk           (0xfffful << GMAC_Addr3High_ADDRHI_Pos)         /*!< GMAC_T::Addr3High: ADDRHI Mask     */

#define GMAC_Addr3High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr3High: MBC Position    */
#define GMAC_Addr3High_MBC_Msk              (0x3ful << GMAC_Addr3High_MBC_Pos)              /*!< GMAC_T::Addr3High: MBC Mask        */

#define GMAC_Addr3High_SA_Pos               (30)                                            /*!< GMAC_T::Addr3High: SA Position     */
#define GMAC_Addr3High_SA_Msk               (0x1ul << GMAC_Addr3High_SA_Pos)                /*!< GMAC_T::Addr3High: SA Mask         */

#define GMAC_Addr3High_AE_Pos               (31)                                            /*!< GMAC_T::Addr3High: AE Position     */
#define GMAC_Addr3High_AE_Msk               (0x1ul << GMAC_Addr3High_AE_Pos)                /*!< GMAC_T::Addr3High: AE Mask         */

#define GMAC_Addr3Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr3Low: ADDRLO Position  */
#define GMAC_Addr3Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr3Low_ADDRLO_Pos)      /*!< GMAC_T::Addr3Low: ADDRLO Mask      */

#define GMAC_Addr4High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr4High: ADDRHI Position */
#define GMAC_Addr4High_ADDRHI_Msk           (0xfffful << GMAC_Addr4High_ADDRHI_Pos)         /*!< GMAC_T::Addr4High: ADDRHI Mask     */

#define GMAC_Addr4High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr4High: MBC Position    */
#define GMAC_Addr4High_MBC_Msk              (0x3ful << GMAC_Addr4High_MBC_Pos)              /*!< GMAC_T::Addr4High: MBC Mask        */

#define GMAC_Addr4High_SA_Pos               (30)                                            /*!< GMAC_T::Addr4High: SA Position     */
#define GMAC_Addr4High_SA_Msk               (0x1ul << GMAC_Addr4High_SA_Pos)                /*!< GMAC_T::Addr4High: SA Mask         */

#define GMAC_Addr4High_AE_Pos               (31)                                            /*!< GMAC_T::Addr4High: AE Position     */
#define GMAC_Addr4High_AE_Msk               (0x1ul << GMAC_Addr4High_AE_Pos)                /*!< GMAC_T::Addr4High: AE Mask         */

#define GMAC_Addr4Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr4Low: ADDRLO Position  */
#define GMAC_Addr4Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr4Low_ADDRLO_Pos)      /*!< GMAC_T::Addr4Low: ADDRLO Mask      */

#define GMAC_Addr5High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr5High: ADDRHI Position */
#define GMAC_Addr5High_ADDRHI_Msk           (0xfffful << GMAC_Addr5High_ADDRHI_Pos)         /*!< GMAC_T::Addr5High: ADDRHI Mask     */

#define GMAC_Addr5High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr5High: MBC Position    */
#define GMAC_Addr5High_MBC_Msk              (0x3ful << GMAC_Addr5High_MBC_Pos)              /*!< GMAC_T::Addr5High: MBC Mask        */

#define GMAC_Addr5High_SA_Pos               (30)                                            /*!< GMAC_T::Addr5High: SA Position     */
#define GMAC_Addr5High_SA_Msk               (0x1ul << GMAC_Addr5High_SA_Pos)                /*!< GMAC_T::Addr5High: SA Mask         */

#define GMAC_Addr5High_AE_Pos               (31)                                            /*!< GMAC_T::Addr5High: AE Position     */
#define GMAC_Addr5High_AE_Msk               (0x1ul << GMAC_Addr5High_AE_Pos)                /*!< GMAC_T::Addr5High: AE Mask         */

#define GMAC_Addr5Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr5Low: ADDRLO Position  */
#define GMAC_Addr5Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr5Low_ADDRLO_Pos)      /*!< GMAC_T::Addr5Low: ADDRLO Mask      */

#define GMAC_Addr6High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr6High: ADDRHI Position */
#define GMAC_Addr6High_ADDRHI_Msk           (0xfffful << GMAC_Addr6High_ADDRHI_Pos)         /*!< GMAC_T::Addr6High: ADDRHI Mask     */

#define GMAC_Addr6High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr6High: MBC Position    */
#define GMAC_Addr6High_MBC_Msk              (0x3ful << GMAC_Addr6High_MBC_Pos)              /*!< GMAC_T::Addr6High: MBC Mask        */

#define GMAC_Addr6High_SA_Pos               (30)                                            /*!< GMAC_T::Addr6High: SA Position     */
#define GMAC_Addr6High_SA_Msk               (0x1ul << GMAC_Addr6High_SA_Pos)                /*!< GMAC_T::Addr6High: SA Mask         */

#define GMAC_Addr6High_AE_Pos               (31)                                            /*!< GMAC_T::Addr6High: AE Position     */
#define GMAC_Addr6High_AE_Msk               (0x1ul << GMAC_Addr6High_AE_Pos)                /*!< GMAC_T::Addr6High: AE Mask         */

#define GMAC_Addr6Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr6Low: ADDRLO Position  */
#define GMAC_Addr6Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr6Low_ADDRLO_Pos)      /*!< GMAC_T::Addr6Low: ADDRLO Mask      */

#define GMAC_Addr7High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr7High: ADDRHI Position */
#define GMAC_Addr7High_ADDRHI_Msk           (0xfffful << GMAC_Addr7High_ADDRHI_Pos)         /*!< GMAC_T::Addr7High: ADDRHI Mask     */

#define GMAC_Addr7High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr7High: MBC Position    */
#define GMAC_Addr7High_MBC_Msk              (0x3ful << GMAC_Addr7High_MBC_Pos)              /*!< GMAC_T::Addr7High: MBC Mask        */

#define GMAC_Addr7High_SA_Pos               (30)                                            /*!< GMAC_T::Addr7High: SA Position     */
#define GMAC_Addr7High_SA_Msk               (0x1ul << GMAC_Addr7High_SA_Pos)                /*!< GMAC_T::Addr7High: SA Mask         */

#define GMAC_Addr7High_AE_Pos               (31)                                            /*!< GMAC_T::Addr7High: AE Position     */
#define GMAC_Addr7High_AE_Msk               (0x1ul << GMAC_Addr7High_AE_Pos)                /*!< GMAC_T::Addr7High: AE Mask         */

#define GMAC_Addr7Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr7Low: ADDRLO Position  */
#define GMAC_Addr7Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr7Low_ADDRLO_Pos)      /*!< GMAC_T::Addr7Low: ADDRLO Mask      */

#define GMAC_Addr8High_ADDRHI_Pos           (0)                                             /*!< GMAC_T::Addr8High: ADDRHI Position */
#define GMAC_Addr8High_ADDRHI_Msk           (0xfffful << GMAC_Addr8High_ADDRHI_Pos)         /*!< GMAC_T::Addr8High: ADDRHI Mask     */

#define GMAC_Addr8High_MBC_Pos              (24)                                            /*!< GMAC_T::Addr8High: MBC Position    */
#define GMAC_Addr8High_MBC_Msk              (0x3ful << GMAC_Addr8High_MBC_Pos)              /*!< GMAC_T::Addr8High: MBC Mask        */

#define GMAC_Addr8High_SA_Pos               (30)                                            /*!< GMAC_T::Addr8High: SA Position     */
#define GMAC_Addr8High_SA_Msk               (0x1ul << GMAC_Addr8High_SA_Pos)                /*!< GMAC_T::Addr8High: SA Mask         */

#define GMAC_Addr8High_AE_Pos               (31)                                            /*!< GMAC_T::Addr8High: AE Position     */
#define GMAC_Addr8High_AE_Msk               (0x1ul << GMAC_Addr8High_AE_Pos)                /*!< GMAC_T::Addr8High: AE Mask         */

#define GMAC_Addr8Low_ADDRLO_Pos            (0)                                             /*!< GMAC_T::Addr8Low: ADDRLO Position  */
#define GMAC_Addr8Low_ADDRLO_Msk            (0xfffffffful << GMAC_Addr8Low_ADDRLO_Pos)      /*!< GMAC_T::Addr8Low: ADDRLO Mask      */

#define GMAC_RgmiiCtrlSts_LNKMOD_Pos        (0)                                             /*!< GMAC_T::RgmiiCtrlSts: LNKMOD Position*/
#define GMAC_RgmiiCtrlSts_LNKMOD_Msk        (0x1ul << GMAC_RgmiiCtrlSts_LNKMOD_Pos)         /*!< GMAC_T::RgmiiCtrlSts: LNKMOD Mask    */

#define GMAC_RgmiiCtrlSts_LNKSPEED_Pos      (1)                                             /*!< GMAC_T::RgmiiCtrlSts: LNKSPEED Position*/
#define GMAC_RgmiiCtrlSts_LNKSPEED_Msk      (0x3ul << GMAC_RgmiiCtrlSts_LNKSPEED_Pos)       /*!< GMAC_T::RgmiiCtrlSts: LNKSPEED Mask  */

#define GMAC_RgmiiCtrlSts_LNKSTS_Pos        (3)                                             /*!< GMAC_T::RgmiiCtrlSts: LNKSTS Position*/
#define GMAC_RgmiiCtrlSts_LNKSTS_Msk        (0x1ul << GMAC_RgmiiCtrlSts_LNKSTS_Pos)         /*!< GMAC_T::RgmiiCtrlSts: LNKSTS Mask    */

#define GMAC_Timeout_WTO_Pos                (0)                                             /*!< GMAC_T::Timeout: WTO Position          */
#define GMAC_Timeout_WTO_Msk                (0x3ffful << GMAC_Timeout_WTO_Pos)              /*!< GMAC_T::Timeout: WTO Mask              */

#define GMAC_Timeout_PWE_Pos                (16)                                            /*!< GMAC_T::Timeout: PWE Position          */
#define GMAC_Timeout_PWE_Msk                (0x1ul << GMAC_Timeout_PWE_Pos)                 /*!< GMAC_T::Timeout: PWE Mask              */

#define GMAC_VlanIncRep_VLT_Pos             (0)                                             /*!< GMAC_T::VlanIncRep: VLT Position         */
#define GMAC_VlanIncRep_VLT_Msk             (0xfffful << GMAC_VlanIncRep_VLT_Pos)           /*!< GMAC_T::VlanIncRep: VLT Mask             */

#define GMAC_VlanIncRep_VLC_Pos             (16)                                            /*!< GMAC_T::VlanIncRep: VLC Position         */
#define GMAC_VlanIncRep_VLC_Msk             (0x3ul << GMAC_VlanIncRep_VLC_Pos)              /*!< GMAC_T::VlanIncRep: VLC Mask             */

#define GMAC_VlanIncRep_VLP_Pos             (18)                                            /*!< GMAC_T::VlanIncRep: VLP Position         */
#define GMAC_VlanIncRep_VLP_Msk             (0x1ul << GMAC_VlanIncRep_VLP_Pos)              /*!< GMAC_T::VlanIncRep: VLP Mask             */

#define GMAC_VlanIncRep_CSVL_Pos            (19)                                            /*!< GMAC_T::VlanIncRep: CSVL Position        */
#define GMAC_VlanIncRep_CSVL_Msk            (0x1ul << GMAC_VlanIncRep_CSVL_Pos)             /*!< GMAC_T::VlanIncRep: CSVL Mask            */

#define GMAC_TSControl_TSENA_Pos            (0)                                             /*!< GMAC_T::TSControl: TSENA Position        */
#define GMAC_TSControl_TSENA_Msk            (0x1ul << GMAC_TSControl_TSENA_Pos)             /*!< GMAC_T::TSControl: TSENA Mask            */

#define GMAC_TSControl_TSCFUPDT_Pos         (1)                                             /*!< GMAC_T::TSControl: TSCFUPDT Position     */
#define GMAC_TSControl_TSCFUPDT_Msk         (0x1ul << GMAC_TSControl_TSCFUPDT_Pos)          /*!< GMAC_T::TSControl: TSCFUPDT Mask         */

#define GMAC_TSControl_TSINIT_Pos           (2)                                             /*!< GMAC_T::TSControl: TSINIT Position       */
#define GMAC_TSControl_TSINIT_Msk           (0x1ul << GMAC_TSControl_TSINIT_Pos)            /*!< GMAC_T::TSControl: TSINIT Mask           */

#define GMAC_TSControl_TSUPDT_Pos           (3)                                             /*!< GMAC_T::TSControl: TSUPDT Position       */
#define GMAC_TSControl_TSUPDT_Msk           (0x1ul << GMAC_TSControl_TSUPDT_Pos)            /*!< GMAC_T::TSControl: TSUPDT Mask           */

#define GMAC_TSControl_TSTRIG_Pos           (4)                                             /*!< GMAC_T::TSControl: TSTRIG Position       */
#define GMAC_TSControl_TSTRIG_Msk           (0x1ul << GMAC_TSControl_TSTRIG_Pos)            /*!< GMAC_T::TSControl: TSTRIG Mask           */

#define GMAC_TSControl_TSADDREG_Pos         (5)                                             /*!< GMAC_T::TSControl: TSADDREG Position     */
#define GMAC_TSControl_TSADDREG_Msk         (0x1ul << GMAC_TSControl_TSADDREG_Pos)          /*!< GMAC_T::TSControl: TSADDREG Mask         */

#define GMAC_TSControl_TSENALL_Pos          (8)                                             /*!< GMAC_T::TSControl: TSENALL Position      */
#define GMAC_TSControl_TSENALL_Msk          (0x1ul << GMAC_TSControl_TSENALL_Pos)           /*!< GMAC_T::TSControl: TSENALL Mask          */

#define GMAC_TSControl_TSCTRLSSR_Pos        (9)                                             /*!< GMAC_T::TSControl: TSCTRLSSR Position    */
#define GMAC_TSControl_TSCTRLSSR_Msk        (0x1ul << GMAC_TSControl_TSCTRLSSR_Pos)         /*!< GMAC_T::TSControl: TSCTRLSSR Mask        */

#define GMAC_TSControl_TSVER2ENA_Pos        (10)                                            /*!< GMAC_T::TSControl: TSVER2ENA Position    */
#define GMAC_TSControl_TSVER2ENA_Msk        (0x1ul << GMAC_TSControl_TSVER2ENA_Pos)         /*!< GMAC_T::TSControl: TSVER2ENA Mask        */

#define GMAC_TSControl_TSIPENA_Pos          (11)                                            /*!< GMAC_T::TSControl: TSIPENA Position      */
#define GMAC_TSControl_TSIPENA_Msk          (0x1ul << GMAC_TSControl_TSIPENA_Pos)           /*!< GMAC_T::TSControl: TSIPENA Mask          */

#define GMAC_TSControl_TSIPV6ENA_Pos        (12)                                            /*!< GMAC_T::TSControl: TSIPV6ENA Position    */
#define GMAC_TSControl_TSIPV6ENA_Msk        (0x1ul << GMAC_TSControl_TSIPV6ENA_Pos)         /*!< GMAC_T::TSControl: TSIPV6ENA Mask        */

#define GMAC_TSControl_TSIPV4ENA_Pos        (13)                                            /*!< GMAC_T::TSControl: TSIPV4ENA Position    */
#define GMAC_TSControl_TSIPV4ENA_Msk        (0x1ul << GMAC_TSControl_TSIPV4ENA_Pos)         /*!< GMAC_T::TSControl: TSIPV4ENA Mask        */

#define GMAC_TSControl_TSEVNTENA_Pos        (14)                                            /*!< GMAC_T::TSControl: TSEVNTENA Position    */
#define GMAC_TSControl_TSEVNTENA_Msk        (0x1ul << GMAC_TSControl_TSEVNTENA_Pos)         /*!< GMAC_T::TSControl: TSEVNTENA Mask        */

#define GMAC_TSControl_TSMSTRENA_Pos        (15)                                            /*!< GMAC_T::TSControl: TSMSTRENA Position    */
#define GMAC_TSControl_TSMSTRENA_Msk        (0x1ul << GMAC_TSControl_TSMSTRENA_Pos)         /*!< GMAC_T::TSControl: TSMSTRENA Mask        */

#define GMAC_TSControl_SNAPTYPSEL_Pos       (16)                                            /*!< GMAC_T::TSControl: SNAPTYPSEL Position   */
#define GMAC_TSControl_SNAPTYPSEL_Msk       (0x3ul << GMAC_TSControl_SNAPTYPSEL_Pos)        /*!< GMAC_T::TSControl: SNAPTYPSEL Mask       */

#define GMAC_TSControl_TSENMACADDR_Pos      (18)                                            /*!< GMAC_T::TSControl: TSENMACADDR Position  */
#define GMAC_TSControl_TSENMACADDR_Msk      (0x1ul << GMAC_TSControl_TSENMACADDR_Pos)       /*!< GMAC_T::TSControl: TSENMACADDR Mask      */

#define GMAC_TSSubSecIncr_SSINC_Pos         (0)                                             /*!< GMAC_T::TSSubSecIncr: SSINC Position*/
#define GMAC_TSSubSecIncr_SSINC_Msk         (0xfful << GMAC_TSSubSecIncr_SSINC_Pos)         /*!< GMAC_T::TSSubSecIncr: SSINC Mask   */

#define GMAC_TSSec_TSS_Pos                  (0)                                             /*!< GMAC_T::TSSec: TSS Position     */
#define GMAC_TSSec_TSS_Msk                  (0xfffffffful << GMAC_TSSec_TSS_Pos)            /*!< GMAC_T::TSSec: TSS Mask         */

#define GMAC_TSNanosec_TSSS_Pos             (0)                                             /*!< GMAC_T::TSNanosec: TSSS Position*/
#define GMAC_TSNanosec_TSSS_Msk             (0x7ffffffful << GMAC_TSNanosec_TSSS_Pos)       /*!< GMAC_T::TSNanosec: TSSS Mask    */

#define GMAC_TSSecUpdate_TSS_Pos            (0)                                             /*!< GMAC_T::TSSecUpdate: TSS Position*/
#define GMAC_TSSecUpdate_TSS_Msk            (0xfffffffful << GMAC_TSSecUpdate_TSS_Pos)      /*!< GMAC_T::TSSecUpdate: TSS Mask  */

#define GMAC_TSNanosecUpdate_TSSS_Pos       (0)                                             /*!< GMAC_T::TSNanosecUpdate: TSSS Position*/
#define GMAC_TSNanosecUpdate_TSSS_Msk       (0x7ffffffful << GMAC_TSNanosecUpdate_TSSS_Pos) /*!< GMAC_T::TSNanosecUpdate: TSSS Mask*/

#define GMAC_TSNanosecUpdate_ADDSUB_Pos     (31)                                            /*!< GMAC_T::TSNanosecUpdate: ADDSUB Position*/
#define GMAC_TSNanosecUpdate_ADDSUB_Msk     (0x1ul << GMAC_TSNanosecUpdate_ADDSUB_Pos)      /*!< GMAC_T::TSNanosecUpdate: ADDSUB Mask*/

#define GMAC_TSAddend_TSAR_Pos              (0)                                             /*!< GMAC_T::TSAddend: TSAR Position          */
#define GMAC_TSAddend_TSAR_Msk              (0xfffffffful << GMAC_TSAddend_TSAR_Pos)        /*!< GMAC_T::TSAddend: TSAR Mask              */

#define GMAC_TSTargetSec_TSTR_Pos           (0)                                             /*!< GMAC_T::TSTargetSec: TSTR Position    */
#define GMAC_TSTargetSec_TSTR_Msk           (0xfffffffful << GMAC_TSTargetSec_TSTR_Pos)     /*!< GMAC_T::TSTargetSec: TSTR Mask        */

#define GMAC_TSTargetNanosec_TTSLO_Pos      (0)                                             /*!< GMAC_T::TSTargetNanosec: TTSLO Position*/
#define GMAC_TSTargetNanosec_TTSLO_Msk      (0x7ffffffful << GMAC_TSTargetNanosec_TTSLO_Pos)/*!< GMAC_T::TSTargetNanosec: TTSLO Mask   */

#define GMAC_TSTargetNanosec_TRGTBUSY_Pos   (31)                                            /*!< GMAC_T::TSTargetNanosec: TRGTBUSY Position*/
#define GMAC_TSTargetNanosec_TRGTBUSY_Msk   (0x1ul << GMAC_TSTargetNanosec_TRGTBUSY_Pos)    /*!< GMAC_T::TSTargetNanosec: TRGTBUSY Mask*/

#define GMAC_TSSecHigher_TSHWR_Pos          (0)                                             /*!< GMAC_T::TSSecHigher: TSHWR Position*/
#define GMAC_TSSecHigher_TSHWR_Msk          (0xfffful << GMAC_TSSecHigher_TSHWR_Pos)        /*!< GMAC_T::TSSecHigher: TSHWR Mask*/

#define GMAC_TSStatus_TSSOVF_Pos            (0)                                             /*!< GMAC_T::TSStatus: TSSOVF Position        */
#define GMAC_TSStatus_TSSOVF_Msk            (0x1ul << GMAC_TSStatus_TSSOVF_Pos)             /*!< GMAC_T::TSStatus: TSSOVF Mask            */

#define GMAC_TSStatus_TSTARGT_Pos           (1)                                             /*!< GMAC_T::TSStatus: TSTARGT Position       */
#define GMAC_TSStatus_TSTARGT_Msk           (0x1ul << GMAC_TSStatus_TSTARGT_Pos)            /*!< GMAC_T::TSStatus: TSTARGT Mask           */

#define GMAC_TSStatus_AUXTSTRIG_Pos         (2)                                             /*!< GMAC_T::TSStatus: AUXTSTRIG Position     */
#define GMAC_TSStatus_AUXTSTRIG_Msk         (0x1ul << GMAC_TSStatus_AUXTSTRIG_Pos)          /*!< GMAC_T::TSStatus: AUXTSTRIG Mask         */

#define GMAC_TSStatus_TSTRGTERR_Pos         (3)                                             /*!< GMAC_T::TSStatus: TSTRGTERR Position     */
#define GMAC_TSStatus_TSTRGTERR_Msk         (0x1ul << GMAC_TSStatus_TSTRGTERR_Pos)          /*!< GMAC_T::TSStatus: TSTRGTERR Mask         */

#define GMAC_TSStatus_ATSSTN_Pos            (16)                                            /*!< GMAC_T::TSStatus: ATSSTN Position        */
#define GMAC_TSStatus_ATSSTN_Msk            (0xful << GMAC_TSStatus_ATSSTN_Pos)             /*!< GMAC_T::TSStatus: ATSSTN Mask            */

#define GMAC_PPSCtrl_PPSCTRL_PPSCMD_Pos     (0)                                             /*!< GMAC_T::PPSCtrl: PPSCTRL_PPSCMD Position*/
#define GMAC_PPSCtrl_PPSCTRL_PPSCMD_Msk     (0xful << GMAC_PPSCtrl_PPSCTRL_PPSCMD_Pos)      /*!< GMAC_T::PPSCtrl: PPSCTRL_PPSCMD Mask   */

#define GMAC_PPSCtrl_PPSEN0_Pos             (4)                                             /*!< GMAC_T::PPSCtrl: PPSEN0 Position       */
#define GMAC_PPSCtrl_PPSEN0_Msk             (0x1ul << GMAC_PPSCtrl_PPSEN0_Pos)              /*!< GMAC_T::PPSCtrl: PPSEN0 Mask           */

#define GMAC_PPSCtrl_TRGTMODSEL0_Pos        (5)                                             /*!< GMAC_T::PPSCtrl: TRGTMODSEL0 Position  */
#define GMAC_PPSCtrl_TRGTMODSEL0_Msk        (0x3ul << GMAC_PPSCtrl_TRGTMODSEL0_Pos)         /*!< GMAC_T::PPSCtrl: TRGTMODSEL0 Mask      */

#define GMAC_PPSInterval_PPSINT_Pos         (0)                                             /*!< GMAC_T::PPSInterval: PPSINT Position      */
#define GMAC_PPSInterval_PPSINT_Msk         (0xfffffffful << GMAC_PPSInterval_PPSINT_Pos)   /*!< GMAC_T::PPSInterval: PPSINT Mask          */

#define GMAC_PPSWidth_PPSWIDTH_Pos          (0)                                             /*!< GMAC_T::PPSWidth: PPSWIDTH Position       */
#define GMAC_PPSWidth_PPSWIDTH_Msk          (0xfffffffful << GMAC_PPSWidth_PPSWIDTH_Pos)    /*!< GMAC_T::PPSWidth: PPSWIDTH Mask           */

#define GMAC_DmaBusMode_SWR_Pos             (0)                                             /*!< GMAC_T::DmaBusMode: SWR Position             */
#define GMAC_DmaBusMode_SWR_Msk             (0x1ul << GMAC_DmaBusMode_SWR_Pos)              /*!< GMAC_T::DmaBusMode: SWR Mask                 */

#define GMAC_DmaBusMode_DSL_Pos             (2)                                             /*!< GMAC_T::DmaBusMode: DSL Position             */
#define GMAC_DmaBusMode_DSL_Msk             (0x1ful << GMAC_DmaBusMode_DSL_Pos)             /*!< GMAC_T::DmaBusMode: DSL Mask                 */

#define GMAC_DmaBusMode_ATDS_Pos            (7)                                             /*!< GMAC_T::DmaBusMode: ATDS Position            */
#define GMAC_DmaBusMode_ATDS_Msk            (0x1ul << GMAC_DmaBusMode_ATDS_Pos)             /*!< GMAC_T::DmaBusMode: ATDS Mask                */

#define GMAC_DmaBusMode_PBL_Pos             (8)                                             /*!< GMAC_T::DmaBusMode: PBL Position             */
#define GMAC_DmaBusMode_PBL_Msk             (0x3ful << GMAC_DmaBusMode_PBL_Pos)             /*!< GMAC_T::DmaBusMode: PBL Mask                 */

#define GMAC_DmaBusMode_FB_Pos              (16)                                            /*!< GMAC_T::DmaBusMode: FB Position              */
#define GMAC_DmaBusMode_FB_Msk              (0x1ul << GMAC_DmaBusMode_FB_Pos)               /*!< GMAC_T::DmaBusMode: FB Mask                  */

#define GMAC_DmaBusMode_RPBL_Pos            (17)                                            /*!< GMAC_T::DmaBusMode: RPBL Position            */
#define GMAC_DmaBusMode_RPBL_Msk            (0x3ful << GMAC_DmaBusMode_RPBL_Pos)            /*!< GMAC_T::DmaBusMode: RPBL Mask                */

#define GMAC_DmaBusMode_USP_Pos             (23)                                            /*!< GMAC_T::DmaBusMode: USP Position             */
#define GMAC_DmaBusMode_USP_Msk             (0x1ul << GMAC_DmaBusMode_USP_Pos)              /*!< GMAC_T::DmaBusMode: USP Mask                 */

#define GMAC_DmaBusMode_PBLx8_Pos           (24)                                            /*!< GMAC_T::DmaBusMode: PBLx8 Position           */
#define GMAC_DmaBusMode_PBLx8_Msk           (0x1ul << GMAC_DmaBusMode_PBLx8_Pos)            /*!< GMAC_T::DmaBusMode: PBLx8 Mask               */

#define GMAC_DmaBusMode_AAL_Pos             (25)                                            /*!< GMAC_T::DmaBusMode: AAL Position             */
#define GMAC_DmaBusMode_AAL_Msk             (0x1ul << GMAC_DmaBusMode_AAL_Pos)              /*!< GMAC_T::DmaBusMode: AAL Mask                 */

#define GMAC_DmaTxPollDemand_TPD_Pos        (0)                                             /*!< GMAC_T::DmaTxPollDemand: TPD Position      */
#define GMAC_DmaTxPollDemand_TPD_Msk        (0xfffffffful << GMAC_DmaTxPollDemand_TPD_Pos)  /*!< GMAC_T::DmaTxPollDemand: TPD Mask          */

#define GMAC_DmaRxPollDemand_RPD_Pos        (0)                                             /*!< GMAC_T::DmaRxPollDemand: RPD Position      */
#define GMAC_DmaRxPollDemand_RPD_Msk        (0xfffffffful << GMAC_DmaRxPollDemand_RPD_Pos)  /*!< GMAC_T::DmaRxPollDemand: RPD Mask          */

#define GMAC_DmaRxDesc_RDESLA_64_bit_Pos    (3)                                             /*!< GMAC_T::DmaRxDesc: RDESLA_64_bit Position*/
#define GMAC_DmaRxDesc_RDESLA_64_bit_Msk    (0x1ffffffful << GMAC_DmaRxDesc_RDESLA_64_bit_Pos) /*!< GMAC_T::DmaRxDesc: RDESLA_64_bit Mask*/

#define GMAC_DmaTxDesc_TDESLA_64_bit_Pos    (3)                                             /*!< GMAC_T::DmaTxDesc: TDESLA_64_bit Position*/
#define GMAC_DmaTxDesc_TDESLA_64_bit_Msk    (0x1ffffffful << GMAC_DmaTxDesc_TDESLA_64_bit_Pos) /*!< GMAC_T::DmaTxDesc: TDESLA_64_bit Mask*/

#define GMAC_DmaStatus_TI_Pos               (0)                                             /*!< GMAC_T::DmaStatus: TI Position            */
#define GMAC_DmaStatus_TI_Msk               (0x1ul << GMAC_DmaStatus_TI_Pos)                /*!< GMAC_T::DmaStatus: TI Mask                */

#define GMAC_DmaStatus_TPS_Pos              (1)                                             /*!< GMAC_T::DmaStatus: TPS Position           */
#define GMAC_DmaStatus_TPS_Msk              (0x1ul << GMAC_DmaStatus_TPS_Pos)               /*!< GMAC_T::DmaStatus: TPS Mask               */

#define GMAC_DmaStatus_TU_Pos               (2)                                             /*!< GMAC_T::DmaStatus: TU Position            */
#define GMAC_DmaStatus_TU_Msk               (0x1ul << GMAC_DmaStatus_TU_Pos)                /*!< GMAC_T::DmaStatus: TU Mask                */

#define GMAC_DmaStatus_TJT_Pos              (3)                                             /*!< GMAC_T::DmaStatus: TJT Position           */
#define GMAC_DmaStatus_TJT_Msk              (0x1ul << GMAC_DmaStatus_TJT_Pos)               /*!< GMAC_T::DmaStatus: TJT Mask               */

#define GMAC_DmaStatus_OVF_Pos              (4)                                             /*!< GMAC_T::DmaStatus: OVF Position           */
#define GMAC_DmaStatus_OVF_Msk              (0x1ul << GMAC_DmaStatus_OVF_Pos)               /*!< GMAC_T::DmaStatus: OVF Mask               */

#define GMAC_DmaStatus_UNF_Pos              (5)                                             /*!< GMAC_T::DmaStatus: UNF Position           */
#define GMAC_DmaStatus_UNF_Msk              (0x1ul << GMAC_DmaStatus_UNF_Pos)               /*!< GMAC_T::DmaStatus: UNF Mask               */

#define GMAC_DmaStatus_RI_Pos               (6)                                             /*!< GMAC_T::DmaStatus: RI Position            */
#define GMAC_DmaStatus_RI_Msk               (0x1ul << GMAC_DmaStatus_RI_Pos)                /*!< GMAC_T::DmaStatus: RI Mask                */

#define GMAC_DmaStatus_RU_Pos               (7)                                             /*!< GMAC_T::DmaStatus: RU Position            */
#define GMAC_DmaStatus_RU_Msk               (0x1ul << GMAC_DmaStatus_RU_Pos)                /*!< GMAC_T::DmaStatus: RU Mask                */

#define GMAC_DmaStatus_RPS_Pos              (8)                                             /*!< GMAC_T::DmaStatus: RPS Position           */
#define GMAC_DmaStatus_RPS_Msk              (0x1ul << GMAC_DmaStatus_RPS_Pos)               /*!< GMAC_T::DmaStatus: RPS Mask               */

#define GMAC_DmaStatus_RWT_Pos              (9)                                             /*!< GMAC_T::DmaStatus: RWT Position           */
#define GMAC_DmaStatus_RWT_Msk              (0x1ul << GMAC_DmaStatus_RWT_Pos)               /*!< GMAC_T::DmaStatus: RWT Mask               */

#define GMAC_DmaStatus_ETI_Pos              (10)                                            /*!< GMAC_T::DmaStatus: ETI Position           */
#define GMAC_DmaStatus_ETI_Msk              (0x1ul << GMAC_DmaStatus_ETI_Pos)               /*!< GMAC_T::DmaStatus: ETI Mask               */

#define GMAC_DmaStatus_FBI_Pos              (13)                                            /*!< GMAC_T::DmaStatus: FBI Position           */
#define GMAC_DmaStatus_FBI_Msk              (0x1ul << GMAC_DmaStatus_FBI_Pos)               /*!< GMAC_T::DmaStatus: FBI Mask               */

#define GMAC_DmaStatus_ERI_Pos              (14)                                            /*!< GMAC_T::DmaStatus: ERI Position           */
#define GMAC_DmaStatus_ERI_Msk              (0x1ul << GMAC_DmaStatus_ERI_Pos)               /*!< GMAC_T::DmaStatus: ERI Mask               */

#define GMAC_DmaStatus_AIS_Pos              (15)                                            /*!< GMAC_T::DmaStatus: AIS Position           */
#define GMAC_DmaStatus_AIS_Msk              (0x1ul << GMAC_DmaStatus_AIS_Pos)               /*!< GMAC_T::DmaStatus: AIS Mask               */

#define GMAC_DmaStatus_NIS_Pos              (16)                                            /*!< GMAC_T::DmaStatus: NIS Position           */
#define GMAC_DmaStatus_NIS_Msk              (0x1ul << GMAC_DmaStatus_NIS_Pos)               /*!< GMAC_T::DmaStatus: NIS Mask               */

#define GMAC_DmaStatus_RS_Pos               (17)                                            /*!< GMAC_T::DmaStatus: RS Position            */
#define GMAC_DmaStatus_RS_Msk               (0x7ul << GMAC_DmaStatus_RS_Pos)                /*!< GMAC_T::DmaStatus: RS Mask                */

#define GMAC_DmaStatus_TS_Pos               (20)                                            /*!< GMAC_T::DmaStatus: TS Position            */
#define GMAC_DmaStatus_TS_Msk               (0x7ul << GMAC_DmaStatus_TS_Pos)                /*!< GMAC_T::DmaStatus: TS Mask                */

#define GMAC_DmaStatus_EB_Pos               (23)                                            /*!< GMAC_T::DmaStatus: EB Position            */
#define GMAC_DmaStatus_EB_Msk               (0x7ul << GMAC_DmaStatus_EB_Pos)                /*!< GMAC_T::DmaStatus: EB Mask                */

#define GMAC_DmaStatus_GLI_Pos              (26)                                            /*!< GMAC_T::DmaStatus: GLI Position           */
#define GMAC_DmaStatus_GLI_Msk              (0x1ul << GMAC_DmaStatus_GLI_Pos)               /*!< GMAC_T::DmaStatus: GLI Mask               */

#define GMAC_DmaStatus_GPI_Pos              (28)                                            /*!< GMAC_T::DmaStatus: GPI Position           */
#define GMAC_DmaStatus_GPI_Msk              (0x1ul << GMAC_DmaStatus_GPI_Pos)               /*!< GMAC_T::DmaStatus: GPI Mask               */

#define GMAC_DmaStatus_TTI_Pos              (29)                                            /*!< GMAC_T::DmaStatus: TTI Position           */
#define GMAC_DmaStatus_TTI_Msk              (0x1ul << GMAC_DmaStatus_TTI_Pos)               /*!< GMAC_T::DmaStatus: TTI Mask               */

#define GMAC_DmaStatus_GLPII_Pos            (30)                                            /*!< GMAC_T::DmaStatus: GLPII Position         */
#define GMAC_DmaStatus_GLPII_Msk            (0x1ul << GMAC_DmaStatus_GLPII_Pos)             /*!< GMAC_T::DmaStatus: GLPII Mask             */

#define GMAC_DmaOpMode_SR_Pos               (1)                                             /*!< GMAC_T::DmaOpMode: SR Position              */
#define GMAC_DmaOpMode_SR_Msk               (0x1ul << GMAC_DmaOpMode_SR_Pos)                /*!< GMAC_T::DmaOpMode: SR Mask                  */

#define GMAC_DmaOpMode_OSF_Pos              (2)                                             /*!< GMAC_T::DmaOpMode: OSF Position             */
#define GMAC_DmaOpMode_OSF_Msk              (0x1ul << GMAC_DmaOpMode_OSF_Pos)               /*!< GMAC_T::DmaOpMode: OSF Mask                 */

#define GMAC_DmaOpMode_RTC_Pos              (3)                                             /*!< GMAC_T::DmaOpMode: RTC Position             */
#define GMAC_DmaOpMode_RTC_Msk              (0x3ul << GMAC_DmaOpMode_RTC_Pos)               /*!< GMAC_T::DmaOpMode: RTC Mask                 */

#define GMAC_DmaOpMode_DGF_Pos              (5)                                             /*!< GMAC_T::DmaOpMode: DGF Position             */
#define GMAC_DmaOpMode_DGF_Msk              (0x1ul << GMAC_DmaOpMode_DGF_Pos)               /*!< GMAC_T::DmaOpMode: DGF Mask                 */

#define GMAC_DmaOpMode_FUF_Pos              (6)                                             /*!< GMAC_T::DmaOpMode: FUF Position             */
#define GMAC_DmaOpMode_FUF_Msk              (0x1ul << GMAC_DmaOpMode_FUF_Pos)               /*!< GMAC_T::DmaOpMode: FUF Mask                 */

#define GMAC_DmaOpMode_FEF_Pos              (7)                                             /*!< GMAC_T::DmaOpMode: FEF Position             */
#define GMAC_DmaOpMode_FEF_Msk              (0x1ul << GMAC_DmaOpMode_FEF_Pos)               /*!< GMAC_T::DmaOpMode: FEF Mask                 */

#define GMAC_DmaOpMode_EFC_Pos              (8)                                             /*!< GMAC_T::DmaOpMode: EFC Position             */
#define GMAC_DmaOpMode_EFC_Msk              (0x1ul << GMAC_DmaOpMode_EFC_Pos)               /*!< GMAC_T::DmaOpMode: EFC Mask                 */

#define GMAC_DmaOpMode_RFA_Pos              (9)                                             /*!< GMAC_T::DmaOpMode: RFA Position             */
#define GMAC_DmaOpMode_RFA_Msk              (0x3ul << GMAC_DmaOpMode_RFA_Pos)               /*!< GMAC_T::DmaOpMode: RFA Mask                 */

#define GMAC_DmaOpMode_RFD_Pos              (11)                                            /*!< GMAC_T::DmaOpMode: RFD Position             */
#define GMAC_DmaOpMode_RFD_Msk              (0x3ul << GMAC_DmaOpMode_RFD_Pos)               /*!< GMAC_T::DmaOpMode: RFD Mask                 */

#define GMAC_DmaOpMode_ST_Pos               (13)                                            /*!< GMAC_T::DmaOpMode: ST Position              */
#define GMAC_DmaOpMode_ST_Msk               (0x1ul << GMAC_DmaOpMode_ST_Pos)                /*!< GMAC_T::DmaOpMode: ST Mask                  */

#define GMAC_DmaOpMode_TTC_Pos              (14)                                            /*!< GMAC_T::DmaOpMode: TTC Position             */
#define GMAC_DmaOpMode_TTC_Msk              (0x7ul << GMAC_DmaOpMode_TTC_Pos)               /*!< GMAC_T::DmaOpMode: TTC Mask                 */

#define GMAC_DmaOpMode_FTF_Pos              (20)                                            /*!< GMAC_T::DmaOpMode: FTF Position             */
#define GMAC_DmaOpMode_FTF_Msk              (0x1ul << GMAC_DmaOpMode_FTF_Pos)               /*!< GMAC_T::DmaOpMode: FTF Mask                 */

#define GMAC_DmaOpMode_TSF_Pos              (21)                                            /*!< GMAC_T::DmaOpMode: TSF Position             */
#define GMAC_DmaOpMode_TSF_Msk              (0x1ul << GMAC_DmaOpMode_TSF_Pos)               /*!< GMAC_T::DmaOpMode: TSF Mask                 */

#define GMAC_DmaOpMode_DFF_Pos              (24)                                            /*!< GMAC_T::DmaOpMode: DFF Position             */
#define GMAC_DmaOpMode_DFF_Msk              (0x1ul << GMAC_DmaOpMode_DFF_Pos)               /*!< GMAC_T::DmaOpMode: DFF Mask                 */

#define GMAC_DmaOpMode_RSF_Pos              (25)                                            /*!< GMAC_T::DmaOpMode: RSF Position             */
#define GMAC_DmaOpMode_RSF_Msk              (0x1ul << GMAC_DmaOpMode_RSF_Pos)               /*!< GMAC_T::DmaOpMode: RSF Mask                 */

#define GMAC_DmaOpMode_DT_Pos               (26)                                            /*!< GMAC_T::DmaOpMode: DT Position              */
#define GMAC_DmaOpMode_DT_Msk               (0x1ul << GMAC_DmaOpMode_DT_Pos)                /*!< GMAC_T::DmaOpMode: DT Mask                  */

#define GMAC_DmaInt_TIE_Pos                 (0)                                             /*!< GMAC_T::DmaInt: TIE Position           */
#define GMAC_DmaInt_TIE_Msk                 (0x1ul << GMAC_DmaInt_TIE_Pos)                  /*!< GMAC_T::DmaInt: TIE Mask               */

#define GMAC_DmaInt_TSE_Pos                 (1)                                             /*!< GMAC_T::DmaInt: TSE Position           */
#define GMAC_DmaInt_TSE_Msk                 (0x1ul << GMAC_DmaInt_TSE_Pos)                  /*!< GMAC_T::DmaInt: TSE Mask               */

#define GMAC_DmaInt_TUE_Pos                 (2)                                             /*!< GMAC_T::DmaInt: TUE Position           */
#define GMAC_DmaInt_TUE_Msk                 (0x1ul << GMAC_DmaInt_TUE_Pos)                  /*!< GMAC_T::DmaInt: TUE Mask               */

#define GMAC_DmaInt_TJE_Pos                 (3)                                             /*!< GMAC_T::DmaInt: TJE Position           */
#define GMAC_DmaInt_TJE_Msk                 (0x1ul << GMAC_DmaInt_TJE_Pos)                  /*!< GMAC_T::DmaInt: TJE Mask               */

#define GMAC_DmaInt_OVE_Pos                 (4)                                             /*!< GMAC_T::DmaInt: OVE Position           */
#define GMAC_DmaInt_OVE_Msk                 (0x1ul << GMAC_DmaInt_OVE_Pos)                  /*!< GMAC_T::DmaInt: OVE Mask               */

#define GMAC_DmaInt_UNE_Pos                 (5)                                             /*!< GMAC_T::DmaInt: UNE Position           */
#define GMAC_DmaInt_UNE_Msk                 (0x1ul << GMAC_DmaInt_UNE_Pos)                  /*!< GMAC_T::DmaInt: UNE Mask               */

#define GMAC_DmaInt_RIE_Pos                 (6)                                             /*!< GMAC_T::DmaInt: RIE Position           */
#define GMAC_DmaInt_RIE_Msk                 (0x1ul << GMAC_DmaInt_RIE_Pos)                  /*!< GMAC_T::DmaInt: RIE Mask               */

#define GMAC_DmaInt_RUE_Pos                 (7)                                             /*!< GMAC_T::DmaInt: RUE Position           */
#define GMAC_DmaInt_RUE_Msk                 (0x1ul << GMAC_DmaInt_RUE_Pos)                  /*!< GMAC_T::DmaInt: RUE Mask               */

#define GMAC_DmaInt_RSE_Pos                 (8)                                             /*!< GMAC_T::DmaInt: RSE Position           */
#define GMAC_DmaInt_RSE_Msk                 (0x1ul << GMAC_DmaInt_RSE_Pos)                  /*!< GMAC_T::DmaInt: RSE Mask               */

#define GMAC_DmaInt_RWE_Pos                 (9)                                             /*!< GMAC_T::DmaInt: RWE Position           */
#define GMAC_DmaInt_RWE_Msk                 (0x1ul << GMAC_DmaInt_RWE_Pos)                  /*!< GMAC_T::DmaInt: RWE Mask               */

#define GMAC_DmaInt_ETE_Pos                 (10)                                            /*!< GMAC_T::DmaInt: ETE Position           */
#define GMAC_DmaInt_ETE_Msk                 (0x1ul << GMAC_DmaInt_ETE_Pos)                  /*!< GMAC_T::DmaInt: ETE Mask               */

#define GMAC_DmaInt_FBE_Pos                 (13)                                            /*!< GMAC_T::DmaInt: FBE Position           */
#define GMAC_DmaInt_FBE_Msk                 (0x1ul << GMAC_DmaInt_FBE_Pos)                  /*!< GMAC_T::DmaInt: FBE Mask               */

#define GMAC_DmaInt_ERE_Pos                 (14)                                            /*!< GMAC_T::DmaInt: ERE Position           */
#define GMAC_DmaInt_ERE_Msk                 (0x1ul << GMAC_DmaInt_ERE_Pos)                  /*!< GMAC_T::DmaInt: ERE Mask               */

#define GMAC_DmaInt_AIE_Pos                 (15)                                            /*!< GMAC_T::DmaInt: AIE Position           */
#define GMAC_DmaInt_AIE_Msk                 (0x1ul << GMAC_DmaInt_AIE_Pos)                  /*!< GMAC_T::DmaInt: AIE Mask               */

#define GMAC_DmaInt_NIE_Pos                 (16)                                            /*!< GMAC_T::DmaInt: NIE Position           */
#define GMAC_DmaInt_NIE_Msk                 (0x1ul << GMAC_DmaInt_NIE_Pos)                  /*!< GMAC_T::DmaInt: NIE Mask               */

#define GMAC_DmaMissedFrame_MISFRMCNT_Pos   (0)                                             /*!< GMAC_T::DmaMissedFrame: MISFRMCNT Position*/
#define GMAC_DmaMissedFrame_MISFRMCNT_Msk   (0xfffful << GMAC_DmaMissedFrame_MISFRMCNT_Pos) /*!< GMAC_T::DmaMissedFrame: MISFRMCNT Mask*/

#define GMAC_DmaMissedFrame_MISCNTOVF_Pos   (16)                                            /*!< GMAC_T::DmaMissedFrame: MISCNTOVF Position*/
#define GMAC_DmaMissedFrame_MISCNTOVF_Msk   (0x1ul << GMAC_DmaMissedFrame_MISCNTOVF_Pos)    /*!< GMAC_T::DmaMissedFrame: MISCNTOVF Mask*/

#define GMAC_DmaMissedFrame_OVFFRMCNT_Pos   (17)                                            /*!< GMAC_T::DmaMissedFrame: OVFFRMCNT Position*/
#define GMAC_DmaMissedFrame_OVFFRMCNT_Msk   (0x7fful << GMAC_DmaMissedFrame_OVFFRMCNT_Pos)  /*!< GMAC_T::DmaMissedFrame: OVFFRMCNT Mask*/

#define GMAC_DmaMissedFrame_OVFCNTOVF_Pos   (28)                                            /*!< GMAC_T::DmaMissedFrame: OVFCNTOVF Position*/
#define GMAC_DmaMissedFrame_OVFCNTOVF_Msk   (0x1ul << GMAC_DmaMissedFrame_OVFCNTOVF_Pos)    /*!< GMAC_T::DmaMissedFrame: OVFCNTOVF Mask*/

#define GMAC_DmaRxIntWdt_RIWT_Pos           (0)                                             /*!< GMAC_T::DmaRxIntWdt: RIWT Position*/
#define GMAC_DmaRxIntWdt_RIWT_Msk           (0xfful << GMAC_DmaRxIntWdt_RIWT_Pos)           /*!< GMAC_T::DmaRxIntWdt: RIWT Mask*/

#define GMAC_DmaAxiBusMode_UNDEF_Pos        (0)                                             /*!< GMAC_T::DmaAxiBusMode: UNDEF Position       */
#define GMAC_DmaAxiBusMode_UNDEF_Msk        (0x1ul << GMAC_DmaAxiBusMode_UNDEF_Pos)         /*!< GMAC_T::DmaAxiBusMode: UNDEF Mask           */

#define GMAC_DmaAxiBusMode_BLEN4_Pos        (1)                                             /*!< GMAC_T::DmaAxiBusMode: BLEN4 Position       */
#define GMAC_DmaAxiBusMode_BLEN4_Msk        (0x1ul << GMAC_DmaAxiBusMode_BLEN4_Pos)         /*!< GMAC_T::DmaAxiBusMode: BLEN4 Mask           */

#define GMAC_DmaAxiBusMode_BLEN8_Pos        (2)                                             /*!< GMAC_T::DmaAxiBusMode: BLEN8 Position       */
#define GMAC_DmaAxiBusMode_BLEN8_Msk        (0x1ul << GMAC_DmaAxiBusMode_BLEN8_Pos)         /*!< GMAC_T::DmaAxiBusMode: BLEN8 Mask           */

#define GMAC_DmaAxiBusMode_BLEN16_Pos       (3)                                             /*!< GMAC_T::DmaAxiBusMode: BLEN16 Position      */
#define GMAC_DmaAxiBusMode_BLEN16_Msk       (0x1ul << GMAC_DmaAxiBusMode_BLEN16_Pos)        /*!< GMAC_T::DmaAxiBusMode: BLEN16 Mask          */

#define GMAC_DmaAxiBusMode_AXI_AAL_Pos      (12)                                            /*!< GMAC_T::DmaAxiBusMode: AXI_AAL Position     */
#define GMAC_DmaAxiBusMode_AXI_AAL_Msk      (0x1ul << GMAC_DmaAxiBusMode_AXI_AAL_Pos)       /*!< GMAC_T::DmaAxiBusMode: AXI_AAL Mask         */

#define GMAC_DmaAxiBusMode_ONEKBBE_Pos      (13)                                            /*!< GMAC_T::DmaAxiBusMode: ONEKBBE Position     */
#define GMAC_DmaAxiBusMode_ONEKBBE_Msk      (0x1ul << GMAC_DmaAxiBusMode_ONEKBBE_Pos)       /*!< GMAC_T::DmaAxiBusMode: ONEKBBE Mask         */

#define GMAC_DmaAxiBusMode_RD_OSR_LMT_Pos   (16)                                            /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT Position  */
#define GMAC_DmaAxiBusMode_RD_OSR_LMT_Msk   (0x3ul << GMAC_DmaAxiBusMode_RD_OSR_LMT_Pos)    /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT Mask      */

#define GMAC_DmaAxiBusMode_RD_OSR_LMT_GT4_Pos (18)                                              /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT_GT4 Position*/
#define GMAC_DmaAxiBusMode_RD_OSR_LMT_GT4_Msk (0x1ul << GMAC_DmaAxiBusMode_RD_OSR_LMT_GT4_Pos)  /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT_GT4 Mask  */

#define GMAC_DmaAxiBusMode_RD_OSR_LMT_GT8_Pos (19)                                              /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT_GT8 Position*/
#define GMAC_DmaAxiBusMode_RD_OSR_LMT_GT8_Msk (0x1ul << GMAC_DmaAxiBusMode_RD_OSR_LMT_GT8_Pos)  /*!< GMAC_T::DmaAxiBusMode: RD_OSR_LMT_GT8 Mask  */

#define GMAC_DmaAxiBusMode_WR_OSR_LMT_Pos     (20)                                              /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT Position  */
#define GMAC_DmaAxiBusMode_WR_OSR_LMT_Msk     (0x3ul << GMAC_DmaAxiBusMode_WR_OSR_LMT_Pos)      /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT Mask      */

#define GMAC_DmaAxiBusMode_WR_OSR_LMT_GT4_Pos (22)                                              /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT_GT4 Position*/
#define GMAC_DmaAxiBusMode_WR_OSR_LMT_GT4_Msk (0x1ul << GMAC_DmaAxiBusMode_WR_OSR_LMT_GT4_Pos)  /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT_GT4 Mask  */

#define GMAC_DmaAxiBusMode_WR_OSR_LMT_GT8_Pos (23)                                              /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT_GT8 Position*/
#define GMAC_DmaAxiBusMode_WR_OSR_LMT_GT8_Msk (0x1ul << GMAC_DmaAxiBusMode_WR_OSR_LMT_GT8_Pos)  /*!< GMAC_T::DmaAxiBusMode: WR_OSR_LMT_GT8 Mask  */

#define GMAC_DmaAxiBusMode_LPI_XIT_FRM_Pos    (30)                                              /*!< GMAC_T::DmaAxiBusMode: LPI_XIT_FRM Position */
#define GMAC_DmaAxiBusMode_LPI_XIT_FRM_Msk    (0x1ul << GMAC_DmaAxiBusMode_LPI_XIT_FRM_Pos)     /*!< GMAC_T::DmaAxiBusMode: LPI_XIT_FRM Mask     */

#define GMAC_DmaAxiBusMode_EN_LPI_Pos         (31)                                              /*!< GMAC_T::DmaAxiBusMode: EN_LPI Position      */
#define GMAC_DmaAxiBusMode_EN_LPI_Msk         (0x1ul << GMAC_DmaAxiBusMode_EN_LPI_Pos)          /*!< GMAC_T::DmaAxiBusMode: EN_LPI Mask          */

#define GMAC_DmaAhbAxiStatus_AXWHSTS_Pos      (0)                                               /*!< GMAC_T::DmaAhbAxiStatus: AXWHSTS Position*/
#define GMAC_DmaAhbAxiStatus_AXWHSTS_Msk      (0x1ul << GMAC_DmaAhbAxiStatus_AXWHSTS_Pos)       /*!< GMAC_T::DmaAhbAxiStatus: AXWHSTS Mask    */

#define GMAC_DmaAhbAxiStatus_AXIRDSTS_Pos     (1)                                               /*!< GMAC_T::DmaAhbAxiStatus: AXIRDSTS Position*/
#define GMAC_DmaAhbAxiStatus_AXIRDSTS_Msk     (0x1ul << GMAC_DmaAhbAxiStatus_AXIRDSTS_Pos)      /*!< GMAC_T::DmaAhbAxiStatus: AXIRDSTS Mask   */

#define GMAC_DmaCurrTxDesc_CURTDESAPTR_Pos    (0)                                               /*!< GMAC_T::DmaCurrTxDesc: CURTDESAPTR Position*/
#define GMAC_DmaCurrTxDesc_CURTDESAPTR_Msk    (0xfffffffful << GMAC_DmaCurrTxDesc_CURTDESAPTR_Pos) /*!< GMAC_T::DmaCurrTxDesc: CURTDESAPTR Mask*/

#define GMAC_DmaCurrRxDesc_CURRDESAPTR_Pos    (0)                                               /*!< GMAC_T::DmaCurrRxDesc: CURRDESAPTR Position*/
#define GMAC_DmaCurrRxDesc_CURRDESAPTR_Msk    (0xfffffffful << GMAC_DmaCurrRxDesc_CURRDESAPTR_Pos) /*!< GMAC_T::DmaCurrRxDesc: CURRDESAPTR Mask*/

#define GMAC_DmaCurrTxAddr_CURTBUFAPTR_Pos    (0)                                               /*!< GMAC_T::DmaCurrTxAddr: CURTBUFAPTR Position*/
#define GMAC_DmaCurrTxAddr_CURTBUFAPTR_Msk    (0xfffffffful << GMAC_DmaCurrTxAddr_CURTBUFAPTR_Pos) /*!< GMAC_T::DmaCurrTxAddr: CURTBUFAPTR Mask*/

#define GMAC_DmaCurrRxAddr_CURRBUFAPTR_Pos    (0)                                               /*!< GMAC_T::DmaCurrRxAddr: CURRBUFAPTR Position*/
#define GMAC_DmaCurrRxAddr_CURRBUFAPTR_Msk    (0xfffffffful << GMAC_DmaCurrRxAddr_CURRBUFAPTR_Pos) /*!< GMAC_T::DmaCurrRxAddr: CURRBUFAPTR Mask*/

#define GMAC_HwFeature_MIISEL_Pos             (0)                                               /*!< GMAC_T::HwFeature: MIISEL Position       */
#define GMAC_HwFeature_MIISEL_Msk             (0x1ul << GMAC_HwFeature_MIISEL_Pos)              /*!< GMAC_T::HwFeature: MIISEL Mask           */

#define GMAC_HwFeature_GMIISEL_Pos            (1)                                               /*!< GMAC_T::HwFeature: GMIISEL Position      */
#define GMAC_HwFeature_GMIISEL_Msk            (0x1ul << GMAC_HwFeature_GMIISEL_Pos)             /*!< GMAC_T::HwFeature: GMIISEL Mask          */

#define GMAC_HwFeature_HDSEL_Pos              (2)                                               /*!< GMAC_T::HwFeature: HDSEL Position        */
#define GMAC_HwFeature_HDSEL_Msk              (0x1ul << GMAC_HwFeature_HDSEL_Pos)               /*!< GMAC_T::HwFeature: HDSEL Mask            */

#define GMAC_HwFeature_EXTHASHEN_Pos          (3)                                               /*!< GMAC_T::HwFeature: EXTHASHEN Position    */
#define GMAC_HwFeature_EXTHASHEN_Msk          (0x1ul << GMAC_HwFeature_EXTHASHEN_Pos)           /*!< GMAC_T::HwFeature: EXTHASHEN Mask        */

#define GMAC_HwFeature_HASHSEL_Pos            (4)                                               /*!< GMAC_T::HwFeature: HASHSEL Position      */
#define GMAC_HwFeature_HASHSEL_Msk            (0x1ul << GMAC_HwFeature_HASHSEL_Pos)             /*!< GMAC_T::HwFeature: HASHSEL Mask          */

#define GMAC_HwFeature_ADDMACADRSEL_Pos       (5)                                               /*!< GMAC_T::HwFeature: ADDMACADRSEL Position */
#define GMAC_HwFeature_ADDMACADRSEL_Msk       (0x1ul << GMAC_HwFeature_ADDMACADRSEL_Pos)        /*!< GMAC_T::HwFeature: ADDMACADRSEL Mask     */

#define GMAC_HwFeature_PCSSEL_Pos             (6)                                               /*!< GMAC_T::HwFeature: PCSSEL Position       */
#define GMAC_HwFeature_PCSSEL_Msk             (0x1ul << GMAC_HwFeature_PCSSEL_Pos)              /*!< GMAC_T::HwFeature: PCSSEL Mask           */

#define GMAC_HwFeature_L3L4FLTREN_Pos         (7)                                               /*!< GMAC_T::HwFeature: L3L4FLTREN Position   */
#define GMAC_HwFeature_L3L4FLTREN_Msk         (0x1ul << GMAC_HwFeature_L3L4FLTREN_Pos)          /*!< GMAC_T::HwFeature: L3L4FLTREN Mask       */

#define GMAC_HwFeature_SMASEL_Pos             (8)                                               /*!< GMAC_T::HwFeature: SMASEL Position       */
#define GMAC_HwFeature_SMASEL_Msk             (0x1ul << GMAC_HwFeature_SMASEL_Pos)              /*!< GMAC_T::HwFeature: SMASEL Mask           */

#define GMAC_HwFeature_RWKSEL_Pos             (9)                                               /*!< GMAC_T::HwFeature: RWKSEL Position       */
#define GMAC_HwFeature_RWKSEL_Msk             (0x1ul << GMAC_HwFeature_RWKSEL_Pos)              /*!< GMAC_T::HwFeature: RWKSEL Mask           */

#define GMAC_HwFeature_MGKSEL_Pos             (10)                                              /*!< GMAC_T::HwFeature: MGKSEL Position       */
#define GMAC_HwFeature_MGKSEL_Msk             (0x1ul << GMAC_HwFeature_MGKSEL_Pos)              /*!< GMAC_T::HwFeature: MGKSEL Mask           */

#define GMAC_HwFeature_MMCSEL_Pos             (11)                                              /*!< GMAC_T::HwFeature: MMCSEL Position       */
#define GMAC_HwFeature_MMCSEL_Msk             (0x1ul << GMAC_HwFeature_MMCSEL_Pos)              /*!< GMAC_T::HwFeature: MMCSEL Mask           */

#define GMAC_HwFeature_TSVER1SEL_Pos          (12)                                              /*!< GMAC_T::HwFeature: TSVER1SEL Position    */
#define GMAC_HwFeature_TSVER1SEL_Msk          (0x1ul << GMAC_HwFeature_TSVER1SEL_Pos)           /*!< GMAC_T::HwFeature: TSVER1SEL Mask        */

#define GMAC_HwFeature_TSVER2SEL_Pos          (13)                                              /*!< GMAC_T::HwFeature: TSVER2SEL Position    */
#define GMAC_HwFeature_TSVER2SEL_Msk          (0x1ul << GMAC_HwFeature_TSVER2SEL_Pos)           /*!< GMAC_T::HwFeature: TSVER2SEL Mask        */

#define GMAC_HwFeature_EEESEL_Pos             (14)                                              /*!< GMAC_T::HwFeature: EEESEL Position       */
#define GMAC_HwFeature_EEESEL_Msk             (0x1ul << GMAC_HwFeature_EEESEL_Pos)              /*!< GMAC_T::HwFeature: EEESEL Mask           */

#define GMAC_HwFeature_AVSEL_Pos              (15)                                              /*!< GMAC_T::HwFeature: AVSEL Position        */
#define GMAC_HwFeature_AVSEL_Msk              (0x1ul << GMAC_HwFeature_AVSEL_Pos)               /*!< GMAC_T::HwFeature: AVSEL Mask            */

#define GMAC_HwFeature_TXCOESEL_Pos           (16)                                              /*!< GMAC_T::HwFeature: TXCOESEL Position     */
#define GMAC_HwFeature_TXCOESEL_Msk           (0x1ul << GMAC_HwFeature_TXCOESEL_Pos)            /*!< GMAC_T::HwFeature: TXCOESEL Mask         */

#define GMAC_HwFeature_RXTYP1COE_Pos          (17)                                              /*!< GMAC_T::HwFeature: RXTYP1COE Position    */
#define GMAC_HwFeature_RXTYP1COE_Msk          (0x1ul << GMAC_HwFeature_RXTYP1COE_Pos)           /*!< GMAC_T::HwFeature: RXTYP1COE Mask        */

#define GMAC_HwFeature_RXTYP2COE_Pos          (18)                                              /*!< GMAC_T::HwFeature: RXTYP2COE Position    */
#define GMAC_HwFeature_RXTYP2COE_Msk          (0x1ul << GMAC_HwFeature_RXTYP2COE_Pos)           /*!< GMAC_T::HwFeature: RXTYP2COE Mask        */

#define GMAC_HwFeature_RXFIFOSIZE_Pos         (19)                                              /*!< GMAC_T::HwFeature: RXFIFOSIZE Position   */
#define GMAC_HwFeature_RXFIFOSIZE_Msk         (0x1ul << GMAC_HwFeature_RXFIFOSIZE_Pos)          /*!< GMAC_T::HwFeature: RXFIFOSIZE Mask       */

#define GMAC_HwFeature_RXCHCNT_Pos            (20)                                              /*!< GMAC_T::HwFeature: RXCHCNT Position      */
#define GMAC_HwFeature_RXCHCNT_Msk            (0x3ul << GMAC_HwFeature_RXCHCNT_Pos)             /*!< GMAC_T::HwFeature: RXCHCNT Mask          */

#define GMAC_HwFeature_TXCHCNT_Pos            (22)                                              /*!< GMAC_T::HwFeature: TXCHCNT Position      */
#define GMAC_HwFeature_TXCHCNT_Msk            (0x3ul << GMAC_HwFeature_TXCHCNT_Pos)             /*!< GMAC_T::HwFeature: TXCHCNT Mask          */

#define GMAC_HwFeature_ENHDESSEL_Pos          (24)                                              /*!< GMAC_T::HwFeature: ENHDESSEL Position    */
#define GMAC_HwFeature_ENHDESSEL_Msk          (0x1ul << GMAC_HwFeature_ENHDESSEL_Pos)           /*!< GMAC_T::HwFeature: ENHDESSEL Mask        */

#define GMAC_HwFeature_INTTSEN_Pos            (25)                                              /*!< GMAC_T::HwFeature: INTTSEN Position      */
#define GMAC_HwFeature_INTTSEN_Msk            (0x1ul << GMAC_HwFeature_INTTSEN_Pos)             /*!< GMAC_T::HwFeature: INTTSEN Mask          */

#define GMAC_HwFeature_FLEXIPPSEN_Pos         (26)                                              /*!< GMAC_T::HwFeature: FLEXIPPSEN Position   */
#define GMAC_HwFeature_FLEXIPPSEN_Msk         (0x1ul << GMAC_HwFeature_FLEXIPPSEN_Pos)          /*!< GMAC_T::HwFeature: FLEXIPPSEN Mask       */

#define GMAC_HwFeature_SAVLANINS_Pos          (27)                                              /*!< GMAC_T::HwFeature: SAVLANINS Position    */
#define GMAC_HwFeature_SAVLANINS_Msk          (0x1ul << GMAC_HwFeature_SAVLANINS_Pos)           /*!< GMAC_T::HwFeature: SAVLANINS Mask        */

#define GMAC_HwFeature_ACTPHYIF_Pos           (28)                                              /*!< GMAC_T::HwFeature: ACTPHYIF Position     */
#define GMAC_HwFeature_ACTPHYIF_Msk           (0x7ul << GMAC_HwFeature_ACTPHYIF_Pos)            /*!< GMAC_T::HwFeature: ACTPHYIF Mask         */

#ifdef __cplusplus
}
#endif

#endif /* __GMAC_REG_H__ */
