/*
    COPYRIGHT 2024 Winbond Electronics Corp.
    SPI NAND Reference Code
    Version  V1.00

    Definitions of SPI NAND Flash devices and commands parameters.
*/

#if 1 /* page size : 2K */
#define PAGE_SHIFT     11 /* Page size = 2048 */
#define PAGE_SIZE      2048
#else /* page size : 4K */
#define PAGE_SHIFT     12 /* Page size = 4096 */
#define PAGE_SIZE      4096
#endif

#define BLOCK_SIZE     (PAGE_SIZE * 64)

#define WB_Flash_PARAM_Timeout 10000 // Set the value carefully, default disable timeout in WB_Wait_Ready()

#define WB_Flash_Dummy 0x0

// SPI NAND Command Set (Table 1)
#define WB_Flash_CMD_Device_Reset 0xFF
#define WB_Flash_CMD_Read_JEDEC_ID 0x9F
#define WB_Flash_CMD_Write_Enable 0x06
#define WB_Flash_CMD_Write_Disable 0x04
#define WB_Flash_CMD_Read_Status_Register_05 0x05
#define WB_Flash_CMD_Read_Status_Register_0F 0x0F
#define WB_Flash_CMD_Write_Status_Register_01 0x01
#define WB_Flash_CMD_Write_Status_Register_1F 0x1F
#define WB_Flash_CMD_BBM 0xA1
#define WB_Flash_CMD_Read_BBM_LUT 0xA5
#define WB_Flash_CMD_Last_ECC_Fail_Page_Addr 0xA9
#define WB_Flash_CMD_Page_Program 0x02
#define WB_Flash_CMD_Block_Erase 0xD8
#define WB_Flash_CMD_Program_Data_Load 0x02
#define WB_Flash_CMD_Random_Program_Data_Load 0x84
#define WB_Flash_CMD_Quad_Program_Data_Load 0x32
#define WB_Flash_CMD_Random_Quad_Program_Data_Load 0x34
#define WB_Flash_CMD_Program_Execute 0x10
#define WB_Flash_CMD_Page_Data_Read 0x13
#define WB_Flash_CMD_Read 0x03
#define WB_Flash_CMD_Fast_Read 0x0B
#define WB_Flash_CMD_Fast_Read_Dual_Output 0x3B
#define WB_Flash_CMD_Fast_Read_Quad_Output 0x6B
#define WB_Flash_CMD_Fast_Read_Dual_IO 0xBB
#define WB_Flash_CMD_Fast_Read_Quad_IO 0xEB
#define WB_Flash_CMD_Deep_Power_Down 0xB9
#define WB_Flash_CMD_Release_Power_Down 0xAB
#define WB_Flash_CMD_Enable_Reset 0x66
#define WB_Flash_CMD_Reset_Device 0x99


// SPI NAND Mask
#define WB_Flash_MASK_WIP 0x01

// Set Read Parameters (C0h)
#define WB_Flash_Read_Wrap_Length_Pos (0) // only applicable in QPI mode
#define WB_Flash_Read_Wrap_Length_Msk (0x3ul << WB_Flash_Read_Wrap_Length_Pos)

#define WB_Flash_Read_Dummy_Clock_Pos (4)
#define WB_Flash_Read_Dummy_Cloc_Msk (0x7ul << WB_Flash_Read_Dummy_Clock_Pos)

#define WB_Flash_Wrap_8_Byte (0x0ul << WB_Flash_Read_Wrap_Length_Pos) // default from power up
#define WB_Flash_Wrap_16_Byte (0x1ul << WB_Flash_Read_Wrap_Length_Pos)
#define WB_Flash_Wrap_32_Byte (0x2ul << WB_Flash_Read_Wrap_Length_Pos)
#define WB_Flash_Wrap_64_Byte (0x3ul << WB_Flash_Read_Wrap_Length_Pos)

// Status Register-1
#define WB_Flash_Status1_BUSY_Pos (0)
#define WB_Flash_Status1_BUSY_Msk (0x1ul << WB_Flash_Status1_BUSY_Pos) // S0

// Status Register-2
#define WB_Flash_Status2_QE_Pos (1)
#define WB_Flash_Status2_QE_Msk (0x1ul << WB_Flash_Status2_QE_Pos) // S9

// Serial Flash Discoverable Parameter Register
#define WB_Flash_SFDP_Register_Address 0x000000 // A23-A8 = 0, A7-A0 = starting byte address

#define WB_Flash_SFDP_Register_Address_Pos (8)
#define WB_Flash_SFDP_Register_Address_Msk (0xFFFFul << WB_Flash_SFDP_Register_Address_Pos)

// Security Register
#define WB_Flash_Security_Register_Address_1                                                                           \
    0x001000 // A23-A16 = 00h, A15-A12 = 1h, A11-A8 = 0h, A7-A0 = starting byte address
#define WB_Flash_Security_Register_Address_2                                                                           \
    0x002000 // A23-A16 = 00h, A15-A12 = 2h, A11-A8 = 0h, A7-A0 = starting byte address
#define WB_Flash_Security_Register_Address_3                                                                           \
    0x003000 // A23-A16 = 00h, A15-A12 = 3h, A11-A8 = 0h, A7-A0 = starting byte address

#define WB_Flash_Security_Register_Address_Pos (8)
#define WB_Flash_Security_Register_Address_Msk (0xFFFFul << WB_Flash_Security_Register_Address_Pos)

// Advanced ECC Register
#define ECC_Register_0 0x0
#define ECC_Register_1 0x1
#define ECC_Register_2 0x2
#define ECC_Register_3 0x3
#define ECC_Register_4 0x4
#define ECC_Register_5 0x5
#define ECC_Register_6 0x6
#define ECC_Register_7 0x7

#define WB_Flash_Advanced_ECC_Register0_ECCOF_Pos (6)
#define WB_Flash_Advanced_ECC_Register0_ECCOF_Msk (0x1ul << WB_Flash_Advanced_ECC_Register0_ECCOF_Pos)


#define WB_Flash_Advanced_ECC_Status_Register_Pos (0)
#define WB_Flash_Advanced_ECC_Status_Register_Msk (0xFul << WB_Flash_Advanced_ECC_Status_Register_Pos)  // ERA[3:0]

#define WB_Flash_Advanced_ECC_Status_Memory_Pos (4)
#define WB_Flash_Advanced_ECC_Status_Memory_Msk (0xFFFFFFFul << WB_Flash_Advanced_ECC_Status_Memory_Pos) // ERA[31:4]


/*
    SPI NOR Parameter (3 Bytes)
    - Operation Mode Byte:
      [2:0]    0 = SPI
               1 = Dual SPI
               2 = Quad SPI
               3 = Octal SPI
               4 = SPI       & DTR
               5 = Dual SPI  & DTR
               6 = Quad SPI  & DTR
               7 = Octal SPI & DTR
      [5:3]    Reserved
      [7:6]    0x0 = N/A
               0x1 = set SPI Dual/Quad IO direction to input
               0x2 = set SPI Dual/Quad IO direction to output

    - Extend Parameter Byte:
      [15:8]   0x1  = check SPI BUSY
               0x2  = clear RX FIFO
               0x4  = return TX result
               0x8  = disable SPI function
               0x10 = read RX FIFO
               All others are reserved.

    - Dummy Model Byte for DTR instruction:
      [23:16]  0x1 = 6 dummy clock
               0x2 = 4 dummy clock
               0x4 = 7 dummy clock
               All others are reserved.
*/
#define WB_Flash_SPI_Mode_Msk 0x00000007
#define WB_Flash_SPI_DIR_Msk 0x000000C0
#define WB_Flash_SPI_PARAM_Msk 0x0000FF00
#define WB_Flash_SPI_DUMMY_Model_Msk 0x00FF0000

#define SIO 0
#define DIO 1
#define QIO 2
#define OIO 3
#define DTSIO 4
#define DTDIO 5
#define DTQIO 6
#define DTOIO 7

#define SPI_DIR_INPUT 0x40
#define SPI_DIR_OUTPUT 0x80

#define SPI_PARAM_CHECK_BUSY_Pos (8)
#define SPI_PARAM_CHECK_BUSY_Msk (0x1ul << SPI_PARAM_CHECK_BUSY_Pos)

#define SPI_PARAM_CLEAR_RXFIFO_Pos (9)
#define SPI_PARAM_CLEAR_RXFIFO_Msk (0x1ul << SPI_PARAM_CLEAR_RXFIFO_Pos)

#define SPI_PARAM_RETURN_TXRESULT_Pos (10)
#define SPI_PARAM_RETURN_TXRESULT_Msk (0x1ul << SPI_PARAM_RETURN_TXRESULT_Pos)

#define SPI_PARAM_DISABLE_FUNCTION_Pos (11)
#define SPI_PARAM_DISABLE_FUNCTION_Msk (0x1ul << SPI_PARAM_DISABLE_FUNCTION_Pos)

#define SPI_PARAM_READ_RXFIFO_Pos (12)
#define SPI_PARAM_READ_RXFIFO_Msk (0x1ul << SPI_PARAM_READ_RXFIFO_Pos)

#define SPI_DUMMY_MODEL_1_Pos (16)
#define SPI_DUMMY_MODEL_1_Msk (0x1ul << SPI_DUMMY_MODEL_1_Pos)

#define SPI_DUMMY_MODEL_2_Pos (17)
#define SPI_DUMMY_MODEL_2_Msk (0x1ul << SPI_DUMMY_MODEL_2_Pos)

#define SPI_DUMMY_MODEL_3_Pos (18)
#define SPI_DUMMY_MODEL_3_Msk (0x1ul << SPI_DUMMY_MODEL_3_Pos)


void WB_Device_Reset(void);
void WB_Wait_Ready(void);
unsigned int WB_Read_JEDEC_ID(void);
void WB_Write_Enable(void);
void WB_Write_Disable(void);
unsigned char WB_Read_Status_Register_05(unsigned char SR_Addr);
unsigned char WB_Read_Status_Register_0F(unsigned char SR_Addr);
void WB_Write_Status_Register_01(unsigned char SR_Addr, unsigned char sr_data_write);
void WB_Write_Status_Register_1F(unsigned char SR_Addr, unsigned char sr_data_write);
void WB_Block_Erase(unsigned int addr);
void WB_Program_Data_Load(unsigned int column_address, unsigned int load_count, unsigned char *load_buf);
void WB_Random_Program_Data_Load(unsigned int column_address, unsigned int load_count, unsigned char *load_buf);
void WB_Quad_Program_Data_Load(unsigned int column_address, unsigned int program_count, unsigned char *program_buf);
void WB_Random_Quad_Program_Data_Load(unsigned int column_address, unsigned int program_count, unsigned char *program_buf);
void WB_Program_Execute(unsigned int addr);
void WB_Page_Data_Read(unsigned int addr);
void WB_Read(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Fast_Read(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Fast_Read_Dual_Output(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Fast_Read_Quad_Output(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Fast_Read_Dual_IO(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Fast_Read_Quad_IO(unsigned int column_address, unsigned int read_count, unsigned char *read_buf);
void WB_Deep_PowerDown(void);
void WB_Release_PowerDown(void);
void WB_Enable_Reset(void);
void WB_Reset_Device(void);
void WB_BBM(unsigned int LBA, unsigned int PBA);
void WB_Read_BBM_LUT(unsigned int read_count, unsigned char *read_buf);
void WB_Last_ECC_Fail_Page_Addr(unsigned char *read_buf);

