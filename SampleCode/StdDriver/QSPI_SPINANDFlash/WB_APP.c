/*
    COPYRIGHT 2024 Winbond Electronics Corp.
    SPI FLASH Reference Code
    Version  V1.00

    The functions in this file describe the following applications:

    1. How to read out the W25N04KW JEDEC ID and Status Register information.
    2. How to perform erase, read, and program operations on the W25N04KW.
    This includes Single-SPI, Dual-SPI, Quad-SPI, and DTR (Double Transfer Rate) Read commands.
    3. How to suspend and resume operations during the erase and program processes.
    4. Regarding a 16-byte program length requirement for the W25N04KW: Show how to check the ECC protection status (ECCD) for the W25N04KW,
    ensuring most of the data is under ECC protection and how to refresh the ECC protection.
    5. How to perform a one-time program (OTP) on the status register.

*/
#include "WB_Command.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Flash_Block_Count      0x1000 // W25N01KW block counts = 4096.
#define Flash_Pages_Per_Block  64

#define ECCD_Mask         0x01
#define SEC_Mask          0x80

/*
    Application Function prototype
*/
void SPI_NAND_ID_SR_Read();              // Reads the JEDEC ID and Status Register.
void SPI_NAND_ERASE_WRITE_READ_Test_1(uint32_t addr); // Erase/Program/Read function test, check comment in each function for detail
// information.
void SPI_NAND_ERASE_WRITE_READ_Test_2(uint32_t addr);
void SPI_NAND_ERASE_WRITE_READ_Test_3();
void SPI_NAND_ERASE_WRITE_READ_Test_4();
void SPI_NAND_READ_Test(uint32_t addr);
/*
    Main Program - SPI_NAND_APP_MainRoutine()
*/

void SPI_NAND_APP_MainRoutine()
{
    unsigned char status;
    int result;
    int address = 0x0;
    int cksum = 0;
    unsigned char page[2] = {0,0};

    printf("\n+----------------------------------------------------------------------------------+\n");
    printf("SPI_NAND_APP_MainRoutine ... (%s %s)\n\n", __DATE__, __TIME__);

    /*
        Reset
    */

    printf("Reset test (0xFF) ...\n\n");
    WB_Device_Reset();

    /*
        Basic ID and status register data check
    */

    SPI_NAND_ID_SR_Read();
    printf("Erase program test ...\n\n");

    //Unprotect
    WB_Write_Enable();
    status = WB_Read_Status_Register_0F(0xA0);
    printf("Status-1 = 0x%x\n", status);
    status &= 0x83;
    printf("Write 0x%x to Status-1\n", status);
    WB_Write_Status_Register_1F(0xA0, status);

    /*
        Basic programming test includes Block Erase (D8h), Read Data (03h), Page Program (02h) and
        Fast Read (0Bh).
    */
    SPI_NAND_ERASE_WRITE_READ_Test_1(0x100000);

    SPI_NAND_ERASE_WRITE_READ_Test_2(0x100000);

    SPI_NAND_ERASE_WRITE_READ_Test_3();

    SPI_NAND_ERASE_WRITE_READ_Test_4();

    printf("\nAll test item is over...\n\n");
}

/*
    Read out basic information from Flash
*/
void SPI_NAND_ID_SR_Read()
{
    unsigned int JEDECID;
    unsigned char SR_A0, SR_B0, SR_C0;

    JEDECID = WB_Read_JEDEC_ID();

    printf("Flash JEDEC ID = 0x%X\n", JEDECID);

    SR_A0 = WB_Read_Status_Register_0F(0xA0);
    SR_B0 = WB_Read_Status_Register_0F(0xB0);
    SR_C0 = WB_Read_Status_Register_0F(0xC0);
    printf("Read by command 0FH SR_A0 = 0x%X, SR_B0 = 0x%X, SR_C0 = 0x%X\n", SR_A0, SR_B0, SR_C0);


    return;
}

/*
    SPI_NAND_ERASE_WRITE_READ_Test_1

    Basic programming check to whole Flash, the sequence is as follows.
    - Block Erase (D8h)
    - Page data Read (13h)
    - Read (03h)
    - Program data load(02h): 0x55
    - Program execute(10h)
    - Page data Read (13h)
    - Fast Read (0Bh)
*/
void SPI_NAND_ERASE_WRITE_READ_Test_1(uint32_t addr)
{
    unsigned char program_page_0to0xff[PAGE_SIZE] = {0};
    unsigned char read_buf[PAGE_SIZE];
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k;
    unsigned char status;

    printf("\n+------------------------------------------------------------------------+\n");
    printf("|                      SPI_NAND_ERASE_WRITE_READ_Test_1                   |\n");
    printf("+------------------------------------------------------------------------+\n");
    for (k = 0; k < PAGE_SIZE; k++)
        program_page_0to0xff[k] = k % 256;


    // Block Erase (128KB)
    printf("\n[Block Erase] 0x%08x ... ", addr);
    WB_Block_Erase(addr);
    printf("Done\n");

    // Read Data (128KB)
    printf("[Block Read] 0x%08x  ... ", addr);
    WB_Page_Data_Read(addr);
    WB_Read(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xFF)
        {
            printf("Erase verify fail at block 0 address 0x%x = 0x%x\n", k, read_buf[k]);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    // Page Program 0x55
    WB_Program_Data_Load(0, PAGE_SIZE, program_page_0to0xff);
    WB_Program_Execute(addr);
    printf("[Page Program 0to0xff] 0x%08x ... Done\n", addr);

    // Read (128KB)
    printf("[Read] 0x%08x  ... ", addr);
    WB_Page_Data_Read(addr);
    WB_Fast_Read(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != (k % 256))
        {
            printf("Program verify fail at addr 0x%X = 0x%x\n", addr+k, read_buf[k]);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    printf("\n  Test Finish !!\n");
}

/*
    SPI_NAND_ERASE_WRITE_READ_Test_2

    Basic programming check to whole Flash, the sequence is as follows.
    - Block Erase (D8h)
    - Page data Read (13h)
    - Fast Read Dual Output (3Bh)
    - Program data load(02h): 0xAA
    - Program execute(10h)
    - Page data Read (13h)
    - Fast Read Dual IO (BBh)
*/
void SPI_NAND_ERASE_WRITE_READ_Test_2(uint32_t addr)
{
    unsigned char program_page_0xAA[PAGE_SIZE] = {0};
    unsigned char read_buf[PAGE_SIZE];
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k;
    unsigned char status;

    printf("\n+------------------------------------------------------------------------+\n");
    printf("|                      SPI_NAND_ERASE_WRITE_READ_Test_2                   |\n");
    printf("+------------------------------------------------------------------------+\n");
    memset(program_page_0xAA, 0xAA, PAGE_SIZE);


    // Block Erase (128KB)
    printf("\n[Block Erase] 0x%08x ... ", addr);
    WB_Block_Erase(addr);
    printf("Done\n");

    // Read Data (128KB)
    printf("[Block Read] 0x%08x  ... ", addr);
    WB_Page_Data_Read(addr);
    WB_Fast_Read_Dual_Output(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xFF)
        {
            printf("Erase verify fail at block 0 address 0x%x = 0x%x\n", k, read_buf[k]);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    // Page Program 0xAA
    WB_Program_Data_Load(0, PAGE_SIZE, program_page_0xAA);
    WB_Program_Execute(addr);
    printf("[Page Program 0xAA] 0x%08x ... Done\n", addr);

    // Read (128KB)
    printf("[Fast Read Dual IO] 0x%08x  ... ", addr);
    WB_Page_Data_Read(addr);
    WB_Fast_Read_Dual_IO(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xAA)
        {
            printf("Program verify fail at block 0x%X0000 address 0x%x\n", addr, k);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    printf("\n  Test Finish !!\n");
}

/*
    SPI_NAND_ERASE_WRITE_READ_Test_3

    Basic programming check to whole Flash, the sequence is as follows.
    - Block Erase (D8h)
    - Page data Read (13h)
    - Fast Read Quad Output (6Bh)
    - Quad Program Data Load(32h): 0x55
    - Program execute(10h)
    - Page data Read (13h)
    - Fast Read Quad IO (EBh)
*/
void SPI_NAND_ERASE_WRITE_READ_Test_3()
{
    unsigned char program_page_0x55[PAGE_SIZE] = {0};
    unsigned char read_buf[PAGE_SIZE];
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k;
    unsigned char status;

    printf("\n+------------------------------------------------------------------------+\n");
    printf("|                      SPI_NAND_ERASE_WRITE_READ_Test_3                   |\n");
    printf("+------------------------------------------------------------------------+\n");
    memset(program_page_0x55, 0x55, PAGE_SIZE);

    // Block Erase (128KB)
    printf("\n[Block Erase] 0x%08x ... ", 0 * BLOCK_SIZE);
    WB_Block_Erase(0 * BLOCK_SIZE);
    printf("Done\n");

    // Read Data (128KB)
    printf("[Block Read] 0x%08x  ... ", 0 * BLOCK_SIZE);
    WB_Page_Data_Read(0 * PAGE_SIZE);
    WB_Fast_Read_Quad_Output(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xFF)
        {
            printf("Erase verify fail at block 0 address 0x%x = 0x%x\n", k, read_buf[k]);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    // Page Program 0x55
    WB_Quad_Program_Data_Load(0, PAGE_SIZE, program_page_0x55);
    WB_Program_Execute(i * PAGE_SIZE);
    printf("[Quad Page Program 0x55] 0x%08x ... Done\n", i * 0x10000 + j * 0x100);

    // Read (128KB)
    printf("[Fast Read Quad IO] 0x%08x  ... ", i * 0x10000);
    WB_Page_Data_Read(i * PAGE_SIZE);
    WB_Fast_Read_Quad_IO(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0x55)
        {
            printf("Program verify fail at block 0x%X0000 address 0x%x\n", i, k);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    printf("\n  Test Finish !!\n");
}

/*
    SPI_NAND_ERASE_WRITE_READ_Test_4

    Basic programming check to whole Flash, the sequence is as follows.
    - Block Erase (D8h)
    - Page data Read (13h)
    - Fast Read Quad Output (6Bh)
    - Random Quad Program Data Load(34h): 0xAA
    - Program execute(10h)
    - Page data Read (13h)
    - Fast Read Quad IO (EBh)
*/
void SPI_NAND_ERASE_WRITE_READ_Test_4()
{
    unsigned char program_page_0xAA[PAGE_SIZE] = {0};
    unsigned char read_buf[PAGE_SIZE];
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k;
    unsigned char status;

    printf("\n+------------------------------------------------------------------------+\n");
    printf("|                      SPI_NAND_ERASE_WRITE_READ_Test_4                   |\n");
    printf("+------------------------------------------------------------------------+\n");
    memset(program_page_0xAA, 0xAA, PAGE_SIZE);


    // Block Erase (128KB)
    printf("\n[Block Erase] 0x%08x ... ", 0 * BLOCK_SIZE);
    WB_Block_Erase(0 * BLOCK_SIZE);
    printf("Done\n");

    // Read Data (128KB)
    printf("[Block Read] 0x%08x  ... ", 0 * BLOCK_SIZE);
    WB_Page_Data_Read(0 * PAGE_SIZE);
    WB_Fast_Read_Quad_Output(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xFF)
        {
            printf("Erase verify fail at block 0 address 0x%x = 0x%x\n", k, read_buf[k]);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    // Page Program 0xAA
    WB_Random_Quad_Program_Data_Load(0, PAGE_SIZE, program_page_0xAA);
    WB_Program_Execute(i * PAGE_SIZE);
    printf("[Random Quad Page Program 0xAA] 0x%08x ... Done\n", i * 0x10000 + j * 0x100);

    // Read (128KB)
    printf("[Fast Read Quad IO] 0x%08x  ... ", i * 0x10000);
    WB_Page_Data_Read(i * PAGE_SIZE);
    WB_Fast_Read_Quad_IO(0, PAGE_SIZE, read_buf);
    for (k = 0; k < PAGE_SIZE; k++)
    {
        if (read_buf[k] != 0xAA)
        {
            printf("Program verify fail at block 0x%X0000 address 0x%x\n", i, k);
            while (1)
                ;
        }
    }
    printf("PASS\n");

    printf("\n  Test Finish !!\n");
}
