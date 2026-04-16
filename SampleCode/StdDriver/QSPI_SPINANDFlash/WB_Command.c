/*
    COPYRIGHT 2024 Winbond Electronics Corp.
    SPI NAND FLASH Reference Code
    Version  V1.00

    The functions in this file describe the SPI input/output combinations for each command in the W25N04KW.
    Each function is associated with the command as shown in Chapters 8.1 and 8.2 of the W25N04KW-DTR datasheet.
    For example, the comment Command# "7" refers to Chapter 8.2."7" of the datasheet.

    The 'N/A' command is associated with the function that handles SPI control signals
     or signal combinations for command testing.

*/

#include "WB_Command.h"
#include "main.h"
#include "NuMicro.h"

#define TXRXRST         (0x01 << 23)
#define FIFOCLR         (0x01 << 22)
#define TXTHIF          0x40000
#define TXFULL          0x20000
#define RXEMPTY         0x100
#define SPI_BUSY        0x00000001
#define SPI_QUAD_EN     0x400000
#define SPI_DIR_2QM     0x100000
#define SPI_REORDER     0x80000
#define DWIDTH_MASK     0x1F00
#define RXPDMAEN        2

static unsigned char WB_SPINAND_SPIin(unsigned int data_input, unsigned int spi_mode);
static void WB_CS_Low();
static void WB_CS_High();

/*
Command#:           N/A
Function:           WB_SPINAND_SPIin(data_input, spi_mode)
Description:        MOSI input to Flash, MISO output from Flash.
Arguments:          data_input: MOSI input to Flash
                    spi_mode: single, dual, quad, octal,
                              single dtr, dual dtr, quad dtr,
                              octal dtr
Return:             MISO output from Flash or execution result (TRUE/FALSE)
Comment:            Please implement the low level driver
                    for SPI bus follow the controller
*/
static unsigned char WB_SPINAND_SPIin(unsigned int data_input, unsigned int spi_mode)
{
    unsigned char u8RxData = 0;
    unsigned int u32SpiParam = 0, u32SpiModel = 0;

    // Implement the SPI TX/RX function follow the hardware

    switch (spi_mode & WB_Flash_SPI_Mode_Msk)
    {
    case SIO:
        break;

    case DIO:
        if (!QSPI_IS_DUAL_ENABLED(QSPI_FLASH_PORT))
        {
            // Enable dual IO mode for first time
            if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
            {
                QSPI_ENABLE_DUAL_INPUT_MODE(QSPI_FLASH_PORT); // Enable SPI dual IO mode and set direction to input
            }
            else
            {
                QSPI_ENABLE_DUAL_OUTPUT_MODE(QSPI_FLASH_PORT); // Enable SPI dual IO mode and set direction to output
            }
        }
        else
        {
            // Dual IO mode has been enabled, determine whether to switch direction
            if (QSPI_IS_DIR_INPUT_MODE(QSPI_FLASH_PORT))
            {
                if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_OUTPUT)
                {
                    QSPI_ENABLE_DUAL_OUTPUT_MODE(QSPI_FLASH_PORT); // input -> output
                }
            }
            else
            {
                if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
                {
                    QSPI_ENABLE_DUAL_INPUT_MODE(QSPI_FLASH_PORT); // output -> input
                }
            }
        }
        break;

    case QIO:
        if (!QSPI_IS_QUAD_ENABLED(QSPI_FLASH_PORT))
        {
            // Enable quad IO mode for first time
            if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
            {
                QSPI_ENABLE_QUAD_INPUT_MODE(QSPI_FLASH_PORT); // Enable SPI quad IO mode and set direction to input
            }
            else
            {
                QSPI_ENABLE_QUAD_OUTPUT_MODE(QSPI_FLASH_PORT); // Enable SPI quad IO mode and set direction to output
            }
        }
        else
        {
            // Quad IO mode has been enabled, determine whether to switch direction
            if (QSPI_IS_DIR_INPUT_MODE(QSPI_FLASH_PORT))
            {
                if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_OUTPUT)
                {
                    QSPI_ENABLE_QUAD_OUTPUT_MODE(QSPI_FLASH_PORT); // input -> output
                }
            }
            else
            {
                if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
                {
                    QSPI_ENABLE_QUAD_INPUT_MODE(QSPI_FLASH_PORT); // output -> input
                }
            }
        }
        break;

    case OIO:
        break;

    case DTSIO:
        if (!QSPI_IS_DTR_ENABLED(QSPI_FLASH_PORT))
        {
            QSPI_ENABLE_DTR_MODE(QSPI_FLASH_PORT); // Enable DTR mode
        }
        break;

    case DTDIO:
        if (!QSPI_IS_DTR_ENABLED(QSPI_FLASH_PORT))
        {
            QSPI_ENABLE_DTR_MODE(QSPI_FLASH_PORT); // Enable DTR mode
        }

        if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
        {
            if (!QSPI_IS_DIR_INPUT_MODE(QSPI_FLASH_PORT))
            {
                QSPI_ENABLE_DUAL_INPUT_MODE(QSPI_FLASH_PORT); // Enable SPI dual IO mode and set direction to input
            }
        }
        else
        {
            if (!QSPI_IS_DIR_OUTPUT_MODE(QSPI_FLASH_PORT))
            {
                QSPI_ENABLE_DUAL_OUTPUT_MODE(QSPI_FLASH_PORT); // Enable SPI dual IO mode and set direction to output
            }
        }
        break;

    case DTQIO:
        if (!QSPI_IS_DTR_ENABLED(QSPI_FLASH_PORT))
        {
            QSPI_ENABLE_DTR_MODE(QSPI_FLASH_PORT); // Enable DTR mode
        }

        if ((spi_mode & WB_Flash_SPI_DIR_Msk) == SPI_DIR_INPUT)
        {
            if (!QSPI_IS_DIR_INPUT_MODE(QSPI_FLASH_PORT))
            {
                QSPI_ENABLE_QUAD_INPUT_MODE(QSPI_FLASH_PORT); // Enable SPI quad IO mode and set direction to input
            }
        }
        else
        {
            if (!QSPI_IS_DIR_OUTPUT_MODE(QSPI_FLASH_PORT))
            {
                QSPI_ENABLE_QUAD_OUTPUT_MODE(QSPI_FLASH_PORT); // Enable SPI quad IO mode and set direction to output
            }
        }
        break;

    case DTOIO:
        break;
    }

    // Processing SPI operating parameters & Dummy model
    u32SpiParam = spi_mode & WB_Flash_SPI_PARAM_Msk;
    u32SpiModel = spi_mode & WB_Flash_SPI_DUMMY_Model_Msk;

    if (u32SpiParam & SPI_PARAM_RETURN_TXRESULT_Msk)
    {
        // Continuous SPI TX case, write data to SPI TX register when TX FIFO is not full
        if (!QSPI_GET_TX_FIFO_FULL_FLAG(QSPI_FLASH_PORT))
        {
            QSPI_WRITE_TX(QSPI_FLASH_PORT, data_input);
            /*return TRUE;*/
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (u32SpiModel == 0)
        {
            // Normal SPI TX case, write data to SPI TX
            QSPI_WRITE_TX(QSPI_FLASH_PORT, data_input);
        }
        else
        {
            // Special SPI TX case, adjust data width to match required dummy clock
            switch (u32SpiModel)
            {
            case SPI_DUMMY_MODEL_1_Msk:
                // set data width as 12 bits for 6 dummy clocks
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 12);
                QSPI_WRITE_TX(QSPI_FLASH_PORT, 0xFFF);
                while (QSPI_IS_BUSY(QSPI_FLASH_PORT))
                    ;
                QSPI_ClearRxFIFO(QSPI_FLASH_PORT);
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 8);
                break;

            case SPI_DUMMY_MODEL_2_Msk:
                // set data width as 16 bits for 4 dummy clocks
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 16);
                QSPI_WRITE_TX(QSPI_FLASH_PORT, 0xFFFF);
                while (QSPI_IS_BUSY(QSPI_FLASH_PORT))
                    ;
                QSPI_ClearRxFIFO(QSPI_FLASH_PORT);
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 8);
                break;

            case SPI_DUMMY_MODEL_3_Msk:
                // set data width as 28 bits for 7 dummy clocks (7*2*4 = 56 bits)
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 28);
                WB_SPINAND_SPIin(0x0FFFFFFF, SIO);
                WB_SPINAND_SPIin(0x0FFFFFFF, SIO);
                while (QSPI_IS_BUSY(QSPI_FLASH_PORT))
                    ;
                QSPI_ClearRxFIFO(QSPI_FLASH_PORT);
                QSPI_SET_DATA_WIDTH(QSPI_FLASH_PORT, 8);
                break;
            }
            return TRUE;
        }
    }

    if (u32SpiParam != 0)
    {
        if (u32SpiParam & SPI_PARAM_CHECK_BUSY_Msk)
        {
            while (QSPI_IS_BUSY(QSPI_FLASH_PORT))
                ; // Wait until SPI TX done

            if (u32SpiParam & SPI_PARAM_CLEAR_RXFIFO_Msk)
            {
                QSPI_ClearRxFIFO(QSPI_FLASH_PORT); // Clear RX FIFO buffer

                if (u32SpiParam & SPI_PARAM_DISABLE_FUNCTION_Msk)
                {
                    QSPI_DISABLE_DTR_MODE(QSPI_FLASH_PORT);
                    QSPI_DISABLE_DUAL_MODE(QSPI_FLASH_PORT);
                    QSPI_DISABLE_QUAD_MODE(QSPI_FLASH_PORT);

                    //QSPI_CLEAR_DATA_PORT_DIRECTION(QSPI_FLASH_PORT);
                    (QSPI_FLASH_PORT)->CTL = ((QSPI_FLASH_PORT)->CTL & ~QSPI_CTL_DATDIR_Msk);
                }
                return TRUE;
            }
            else if (u32SpiParam & SPI_PARAM_READ_RXFIFO_Msk)
            {
                u8RxData = QSPI_READ_RX(QSPI_FLASH_PORT); // Read data from RX register

                if (u32SpiParam & SPI_PARAM_DISABLE_FUNCTION_Msk)
                {
                    QSPI_DISABLE_DTR_MODE(QSPI_FLASH_PORT);
                    QSPI_DISABLE_DUAL_MODE(QSPI_FLASH_PORT);
                    QSPI_DISABLE_QUAD_MODE(QSPI_FLASH_PORT);

                    //QSPI_CLEAR_DATA_PORT_DIRECTION(QSPI_FLASH_PORT);
                    (QSPI_FLASH_PORT)->CTL = ((QSPI_FLASH_PORT)->CTL & ~QSPI_CTL_DATDIR_Msk);
                }
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            return TRUE; // for SPI_PARAM_RETURN_TXRESULT_Msk, continuous SPI TX case
        }
    }

    return u8RxData;
}

/*
Command#:           N/A
Function:           WB_CS_Low()
Description:        Enable the CS
Arguments:
Return:
Comment:            Please implement this function follow the controller
*/
static void WB_CS_Low()
{
    //  Implement the CS low follow the hardware
    QSPI_SET_SS_LOW(QSPI_FLASH_PORT);
    return;
}

/*
Command#:           N/A
Function:           WB_CS_High()
Description:        Disable the CS
Arguments:
Return:
Comment:            Please implement this function follow the controller
*/
static void WB_CS_High()
{
    //  Implement the CS high follow the hardware
    QSPI_SET_SS_HIGH(QSPI_FLASH_PORT);
    return;
}

/*
Command#:           N/A
Function:           WB_Wait_Ready()
Description:        Wait until Flash ready
Arguments:
Return:
Comment:            This command use to check ready after data write
                    Set the timeout value follow the controller
*/
void WB_Wait_Ready(void)
{
    unsigned char read_data;
    unsigned int time_out = WB_Flash_PARAM_Timeout;
    while (time_out)
    {
        read_data = WB_Read_Status_Register_0F(0xC0);
        if ((read_data & WB_Flash_MASK_WIP) == 0)
        {
            break;
        }
        // time_out--;      // Please enable the timeout carefully
    }
    return;
}

/*
Command#:
Function:           WB_Device_Reset()
Description:        Device Reset
Arguments:
Return:
Comment:
*/
void WB_Device_Reset(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Device_Reset, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_Read_JEDEC_ID()
Description:        Read out JEDEC ID
Arguments:
Return:             return 3 byte JEDEC ID
Comment:
*/
unsigned int WB_Read_JEDEC_ID(void)
{
    unsigned int JEDEC_ID = 0;
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Read_JEDEC_ID, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    JEDEC_ID = WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    JEDEC_ID <<= 8;
    JEDEC_ID += WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    JEDEC_ID <<= 8;
    JEDEC_ID += WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    JEDEC_ID <<= 8;
    JEDEC_ID += WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    WB_CS_High();
    return JEDEC_ID;
}

/*
Command#:           1
Function:           WB_Write_Enable()
Description:        Set WEL for Flash data write
Arguments:
Return:
Comment:
*/
void WB_Write_Enable(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Write_Enable, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    return;
}

/*
Command#:           3
Function:           WB_Write_Disable()
Description:        Reset WEL
Arguments:
Return:
Comment:
*/
void WB_Write_Disable(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Write_Disable, SIO | SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk);
    WB_CS_High();
    return;
}

/*
Command#:
Function:           WB_Read_Status_Register_05()
Description:        Read status register (05h)
Arguments:          Status register address
Return:             Status register value
Comment:
*/
unsigned char WB_Read_Status_Register_05(unsigned char SR_Addr)
{
    unsigned char read_data;
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Read_Status_Register_05, SIO);
    WB_SPINAND_SPIin(SR_Addr, SIO | SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk);
    read_data = WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    WB_CS_High();
    return read_data;
}

/*
Command#:
Function:           WB_Read_Status_Register_0F()
Description:        Read status register (0Fh)
Arguments:          Status register address
Return:             Status register value
Comment:
*/
unsigned char WB_Read_Status_Register_0F(unsigned char SR_Addr)
{
    unsigned char read_data;
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Read_Status_Register_0F, SIO);
    WB_SPINAND_SPIin(SR_Addr, SIO | SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk);
    read_data = WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    WB_CS_High();
    return read_data;
}


/*
Command#:
Function:           WB_Write_Status_Register_01()
Description:        Write status register command 01H
Arguments:          sr_data_write: data write to the status register
Return:
Comment:
*/
void WB_Write_Status_Register_01(unsigned char SR_Addr, unsigned char sr_data_write)
{
    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Write_Status_Register_01, SIO);
    WB_SPINAND_SPIin(SR_Addr, SIO);
    WB_SPINAND_SPIin(sr_data_write, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    WB_CS_High();
    WB_Wait_Ready();
    return;
}
/*
Command#:
Function:           WB_Write_Status_Register_1F()
Description:        Write status register command 1FH
Arguments:          sr_data_write: data write to the status register
Return:
Comment:
*/
void WB_Write_Status_Register_1F(unsigned char SR_Addr, unsigned char sr_data_write)
{
    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Write_Status_Register_1F, SIO);
    WB_SPINAND_SPIin(SR_Addr, SIO);
    WB_SPINAND_SPIin(sr_data_write, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_Block_Erase()
Description:        Block erase
Arguments:          addr:address to erase
Return:
Comment:
*/
void WB_Block_Erase(unsigned int addr)
{
    unsigned int page_address;

    page_address = addr >> PAGE_SHIFT;
    printf("[%s] page_address = 0x%x\n", __func__, page_address);

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Block_Erase, SIO);
    WB_SPINAND_SPIin((page_address & 0xFF0000) >> 16, SIO);
    WB_SPINAND_SPIin((page_address & 0xFF00) >> 8, SIO);
    WB_SPINAND_SPIin(page_address & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_Program_Data_Load()
Description:        Load data to buffer (Reset buffer)
Arguments:          column_address: starting address to load
                    load_count: number of the data to program
                    load_buf: pointer for the program data
Return:
Comment:
*/
void WB_Program_Data_Load(unsigned int column_address, unsigned int load_count, unsigned char *load_buf)
{
    unsigned int index = 0;

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Program_Data_Load, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (load_count)
    {
        *(load_buf + index) =
            WB_SPINAND_SPIin(*(load_buf + index), SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
        index++;
        load_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Random_Program_Data_Load()
Description:        Load data to buffer
Arguments:          column_address: starting address to load
                    load_count: number of the data to program
                    load_buf: pointer for the program data
Return:
Comment:
*/
void WB_Random_Program_Data_Load(unsigned int column_address, unsigned int load_count, unsigned char *load_buf)
{
    unsigned int index = 0;

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Random_Program_Data_Load, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (load_count)
    {
        *(load_buf + index) =
            WB_SPINAND_SPIin(*(load_buf + index), SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
        index++;
        load_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Quad_Program_Data_Load()
Description:        Quad Load data to buffer (Reset buffer)
Arguments:          column_address: starting address to load
                    program_count: number of the data to program
                    program_buf: pointer for the program data
Return:
Comment:
*/
void WB_Quad_Program_Data_Load(unsigned int column_address, unsigned int program_count, unsigned char *program_buf)
{
    unsigned int index = 0;
    unsigned char result;

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Quad_Program_Data_Load, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (program_count)
    {
        if (program_count > 1)
        {
            result = WB_SPINAND_SPIin(*(program_buf + index), (QIO | SPI_DIR_OUTPUT) | SPI_PARAM_RETURN_TXRESULT_Msk);
        }
        else
        {
            // last byte, clear SPI RX FIFO & disable SPI quad IO mode
            result =
                WB_SPINAND_SPIin(*(program_buf + index),
                                 (QIO | SPI_DIR_OUTPUT) | (SPI_PARAM_RETURN_TXRESULT_Msk | SPI_PARAM_CHECK_BUSY_Msk |
                                         SPI_PARAM_CLEAR_RXFIFO_Msk | SPI_PARAM_DISABLE_FUNCTION_Msk));
        }

        if (result == TRUE)
        {
            index++;
            program_count--;
        }
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Random_Quad_Program_Data_Load()
Description:        Quad Load data to buffer (Reset buffer)
Arguments:          column_address: starting address to load
                    program_count: number of the data to program
                    program_buf: pointer for the program data
Return:
Comment:
*/
void WB_Random_Quad_Program_Data_Load(unsigned int column_address, unsigned int program_count, unsigned char *program_buf)
{
    unsigned int index = 0;
    unsigned char result;

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Random_Quad_Program_Data_Load, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (program_count)
    {
        if (program_count > 1)
        {
            result = WB_SPINAND_SPIin(*(program_buf + index), (QIO | SPI_DIR_OUTPUT) | SPI_PARAM_RETURN_TXRESULT_Msk);
        }
        else
        {
            // last byte, clear SPI RX FIFO & disable SPI quad IO mode
            result =
                WB_SPINAND_SPIin(*(program_buf + index),
                                 (QIO | SPI_DIR_OUTPUT) | (SPI_PARAM_RETURN_TXRESULT_Msk | SPI_PARAM_CHECK_BUSY_Msk |
                                         SPI_PARAM_CLEAR_RXFIFO_Msk | SPI_PARAM_DISABLE_FUNCTION_Msk));
        }

        if (result == TRUE)
        {
            index++;
            program_count--;
        }
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Program_Execute()
Description:        Program execute
Arguments:          addr : address to program
Return:
Comment:
*/
void WB_Program_Execute(unsigned int addr)
{
    unsigned int page_address;

    page_address = addr >> PAGE_SHIFT;

    WB_Write_Enable();
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Program_Execute, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO);
    WB_SPINAND_SPIin((page_address >> 8), SIO);
    WB_SPINAND_SPIin((page_address & 0xFF), SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_Page_Data_Read()
Description:        Page Data Read
Arguments:          addr: address to read
Return:
Comment:
*/
void WB_Page_Data_Read(unsigned int addr)
{
    unsigned int page_address;

    page_address = addr >> PAGE_SHIFT;

    WB_CS_Low();

    QSPI_FLASH_PORT->FIFOCTL |= 0x300; //TX/RX FIFO buffer clear
    while (QSPI_FLASH_PORT->STATUS & FIFOCLR);

    QSPI_FLASH_PORT->TX = WB_Flash_CMD_Page_Data_Read;
    QSPI_FLASH_PORT->TX = 0; //dummy byte
    QSPI_FLASH_PORT->TX = (page_address >> 8);
    QSPI_FLASH_PORT->TX = (page_address & 0xFF);

    while (QSPI_FLASH_PORT->STATUS & SPI_BUSY);

    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_Read()
Description:        Read
Arguments:          column_address: starting address to load
                    read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Read(unsigned int column_address, unsigned int read_count, unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Read, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (read_count)
    {
        *(read_buf + index) =
            WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
        index++;
        read_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Fast_Read()
Description:        Fast Read
Arguments:          column_address: starting address to load
                    read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Fast_Read(unsigned int column_address, unsigned int read_count, unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Fast_Read, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (read_count)
    {
        *(read_buf + index) =
            WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
        index++;
        read_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Fast_Read_Dual_Output()
Description:        Fast Read in Dual mode
Arguments:          column_address: starting address to load
                    read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Fast_Read_Dual_Output(unsigned int column_address, unsigned int read_count, unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Fast_Read_Dual_Output, SIO);
    WB_SPINAND_SPIin(column_address >> 8, SIO);
    WB_SPINAND_SPIin(column_address & 0xFF, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (read_count)
    {
        // enable SPI dual IO mode and set direction to input
        if (read_count > 1)
        {
            *(read_buf + index) = WB_SPINAND_SPIin(
                                      WB_Flash_Dummy, (DIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
        }
        else
        {
            // last byte, disable dual IO mode
            *(read_buf + index) = WB_SPINAND_SPIin(
                                      WB_Flash_Dummy, (DIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk |
                                              SPI_PARAM_DISABLE_FUNCTION_Msk));
        }

        index++;
        read_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Fast_Read_Dual_IO()
Description:        Fast Read in Dual I/O
Arguments:          column_address: starting address to load
                    read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Fast_Read_Dual_IO(unsigned int column_address, unsigned int read_count, unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Fast_Read_Dual_IO, SIO);
    WB_SPINAND_SPIin(column_address >> 8, DIO | SPI_DIR_OUTPUT);
    WB_SPINAND_SPIin(column_address & 0xFF, DIO | SPI_DIR_OUTPUT);
    WB_SPINAND_SPIin(WB_Flash_Dummy, DIO | SPI_DIR_OUTPUT | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (read_count)
    {
        // enable SPI dual IO mode and set direction to input
        if (read_count > 1)
        {
            *(read_buf + index) = WB_SPINAND_SPIin(
                                      WB_Flash_Dummy, (DIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
        }
        else
        {
            // last byte, disable dual IO mode
            *(read_buf + index) = WB_SPINAND_SPIin(
                                      WB_Flash_Dummy, (DIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk |
                                              SPI_PARAM_DISABLE_FUNCTION_Msk));
        }

        index++;
        read_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Fast_Read_Quad_Output()
Description:        Fast Read in Quad Output
Arguments:          column_address: starting address to load
                    read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Fast_Read_Quad_Output(unsigned int column_address, unsigned int read_count, unsigned char *read_buf)
{
    unsigned int i;
    unsigned int j = 0;
    unsigned int index = 0;
    unsigned char *rx = read_buf;

    SYS_UnlockReg();
    SYS_ResetModule(PDMA0_RST);
    SYS_LockReg();

    WB_CS_Low();

    QSPI_FLASH_PORT->FIFOCTL |= 0x300; //TX/RX FIFO buffer clear
    while (QSPI_FLASH_PORT->STATUS & FIFOCLR);

    QSPI_FLASH_PORT->TX = 0x6B;
    QSPI_FLASH_PORT->TX = (column_address >> 8);
    QSPI_FLASH_PORT->TX = (column_address & 0xFF);
    QSPI_FLASH_PORT->TX = 0; //dummy byte

    while (QSPI_FLASH_PORT->STATUS & SPI_BUSY);

    QSPI_FLASH_PORT->FIFOCTL |= 0x100; //RX FIFO buffer clear
    while (QSPI_FLASH_PORT->STATUS & FIFOCLR);

    if ((read_count % 4) == 0)
    {
        /* Enable Quad mode, direction input */
        QSPI_FLASH_PORT->CTL = (QSPI_FLASH_PORT->CTL & ~(SPI_DIR_2QM)) | SPI_QUAD_EN;
        /* Set DWIDTH to 32 bits and enable byte reorder*/
        QSPI_FLASH_PORT->CTL = (QSPI_FLASH_PORT->CTL & ~(DWIDTH_MASK)) | SPI_REORDER;

        /* PDMA RX channel configuration */
        PDMA0->CHCTL |= 2; /* Enable PDMA channel 1 */
        PDMA0->REQSEL0_3 = (PDMA0->REQSEL0_3 & (~PDMA_REQSEL0_3_REQSRC1_Msk)) | (55 << PDMA_REQSEL0_3_REQSRC1_Pos); /* 55: QSPI0_RX */

        PDMA0->DSCT[1].CTL = ((read_count/4)-1)<<16 | /* Transfer count */
                             //1<<14 | /* ACK */
                             2<<12 | /* Transfer width 32 bits */
                             0<<10 | /* Increment destination address */
                             3<<8  | /* Fixed source address */
                             1<<7  | /* Table interupt disabled */
                             0<<4  | /* Burst size 128 transfers. No effect in single request type. */
                             1<<2  | /* Single request type */
                             1;      /* Basic mode */
        PDMA0->DSCT[1].SA = ptr_to_u32(&QSPI_FLASH_PORT->RX);
        PDMA0->DSCT[1].DA = ptr_to_u32(read_buf);

        /* Trigger PDMA */
        QSPI_FLASH_PORT->PDMACTL |= RXPDMAEN;

        while((PDMA0->TDSTS & 2)==0); /* check the slave's RX DMA interrupt flag */
        PDMA0->TDSTS = 2; /* Clear the PDMA transfer done interrupt flag */

        QSPI_FLASH_PORT->PDMACTL = 0;

        /* Restore to 1-bit mode */
        QSPI_FLASH_PORT->CTL = (QSPI_FLASH_PORT->CTL & ~(SPI_QUAD_EN | SPI_DIR_2QM | DWIDTH_MASK | SPI_REORDER)) | 0x800;
    }
    else
    {
        while (read_count)
        {
            // enable SPI dual IO mode and set direction to input
            if (read_count > 1)
            {
                *(read_buf + index) = WB_SPINAND_SPIin(
                                          WB_Flash_Dummy, (QIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
            }
            else
            {
                // last byte, disable Quad IO mode
                *(read_buf + index) = WB_SPINAND_SPIin(
                                          WB_Flash_Dummy, (QIO | SPI_DIR_INPUT) | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk |
                                                  SPI_PARAM_DISABLE_FUNCTION_Msk));
            }

            index++;
            read_count--;
        }
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Deep_PowerDown()
Description:        Deep Power Down
Arguments:
Return:
Comment:
*/
void WB_Deep_PowerDown(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Deep_Power_Down, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    return;
}

/*
Command#:
Function:           WB_Release_PowerDown()
Description:        Release Power Down
Arguments:
Return:
Comment:
*/
void WB_Release_PowerDown(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Release_Power_Down, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    return;
}

/*
Command#:
Function:           WB_Enable_Reset()
Description:        Enable Reset
Arguments:
Return:
Comment:
*/
void WB_Enable_Reset(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Enable_Reset, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    return;
}

/*
Command#:
Function:           WB_Reset_Device()
Description:        Reset Device
Arguments:
Return:
Comment:
*/
void WB_Reset_Device(void)
{
    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Reset_Device, SIO | SPI_PARAM_CHECK_BUSY_Msk);
    WB_CS_High();
    WB_Wait_Ready();
    return;
}

/*
Command#:
Function:           WB_BBM()
Description:        Bad Block Management
Arguments:          LBA: Logical Block Address
                    PBA: Physical Block Address
Return:
Comment:
*/
void WB_BBM(unsigned int LBA, unsigned int PBA)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_BBM, SIO);
    WB_SPINAND_SPIin((LBA & 0xFF00) >> 8, SIO);
    WB_SPINAND_SPIin(LBA & 0xFF, SIO);
    WB_SPINAND_SPIin((PBA & 0xFF00) >> 8, SIO);
    WB_SPINAND_SPIin(PBA & 0xFF, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    WB_CS_High();
}

/*
Command#:
Function:           WB_Read_BBM_LUT()
Description:        Read Bad Block Management Look up table
Arguments:          read_count: number of the data to read
                    read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Read_BBM_LUT(unsigned int read_count, unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Read_BBM_LUT, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));

    while (read_count)
    {
        *(read_buf + index) =
            WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
        index++;
        read_count--;
    }

    WB_CS_High();
}

/*
Command#:
Function:           WB_Last_ECC_Fail_Page_Addr()
Description:        Last ECC Failure Page Address
Arguments:          read_buf: pointer for the read data
Return:
Comment:
*/
void WB_Last_ECC_Fail_Page_Addr(unsigned char *read_buf)
{
    unsigned int index = 0;

    WB_CS_Low();
    WB_SPINAND_SPIin(WB_Flash_CMD_Last_ECC_Fail_Page_Addr, SIO);
    WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_CLEAR_RXFIFO_Msk));
    *read_buf = WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));
    *(read_buf + 1) = WB_SPINAND_SPIin(WB_Flash_Dummy, SIO | (SPI_PARAM_CHECK_BUSY_Msk | SPI_PARAM_READ_RXFIFO_Msk));

    WB_CS_High();
}


