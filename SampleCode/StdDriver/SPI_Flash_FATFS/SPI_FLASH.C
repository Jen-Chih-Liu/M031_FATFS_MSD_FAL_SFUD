#include <stdio.h>
#include <stdint.h>
#include "NuMicro.h"

#define SPI_Flash SPI0

#define DRVSPIFLASH_PAGE_SIZE       256
#define DRVSPIFLASH_SECTOR_SIZE     4096

/* SPI Flash Status */
#define DRVSPIFLASH_SPR             0x80    /* Status Register Protect      */
#define DRVSPIFLASH_R               0x40    /* Reserved Bit                 */
#define DRVSPIFLASH_TB              0x20    /* Top / Bottom Block Protect   */
#define DRVSPIFLASH_BP2             0x10    /* Block Protect Bit 2          */
#define DRVSPIFLASH_BP1             0x8     /* Block Protect Bit 1          */
#define DRVSPIFLASH_BP0             0x4     /* Block Protect Bit 0          */
#define DRVSPIFLASH_WEL             0x2     /* Write Enable Latch           */
#define DRVSPIFLASH_BUSY            0x1     /* BUSY                         */


/* SPI Flash Command */
#define DRVSPIFLASH_ZERO            0x00
#define DRVSPIFLASH_DUMMY           0xFF
#define DRVSPIFLASH_WRITE_ENABLE    0x06
#define DRVSPIFLASH_WRITE_DISABLE   0x04
#define DRVSPIFLASH_READ_STATUS     0x05
#define DRVSPIFLASH_WRITE_STATUS    0x01
#define DRVSPIFLASH_FAST_READ       0x0B
#define DRVSPIFLASH_FAST_RD_DUAL    0x3B
#define DRVSPIFLASH_PAGE_PROGRAM    0x02
#define DRVSPIFLASH_BLOCK_ERASE     0xD8
#define DRVSPIFLASH_SECTOR_ERASE    0x20
#define DRVSPIFLASH_CHIP_ERASE      0xC7
#define DRVSPIFLASH_POWER_DOWN      0xB9
#define DRVSPIFLASH_RELEASE_PD_ID   0xAB
#define DRVSPIFLASH_FAST_READ_PARA  0x5B
#define DRVSPIFLASH_PROGRAM_PARA    0x52
#define DRVSPIFLASH_ERASE_PARA      0xD5
#define DRVSPIFLASH_DEVICE_ID       0x90
#define DRVSPIFLASH_JEDEC_ID        0x9F


uint32_t g_sectorBuf[1024]; /* 4096 bytes */

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

//porting define
void SpiInit(void)
{
    /* Setup SPI0 multi-function pins */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL = SYS_GPA_MFPL_PA0MFP_SPI0_MOSI | SYS_GPA_MFPL_PA1MFP_SPI0_MISO | SYS_GPA_MFPL_PA2MFP_SPI0_CLK | SYS_GPA_MFPL_PA3MFP_SPI0_SS ;



    /* Init GPIO for SPI0 */
    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Configure SPI0 as a master, SPI clock rate 24 MHz,
    clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    SPI_Open(SPI_Flash, SPI_MASTER, SPI_MODE_0, 32, 12000000);

    /* Disable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_DisableAutoSS(SPI_Flash);
    SPI_SET_SS_HIGH(SPI_Flash);
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
}

#define Check_SPI_BUSY                   while(SPI_IS_BUSY(SPI_Flash))
#define SPI_SetBitLength(n)         SPI_SET_DATA_WIDTH(SPI_Flash,8*(n))
#define SPI_READ                SPI_READ_RX(SPI_Flash)
#define  SS_HIGH                            SPI_SET_SS_HIGH(SPI_Flash);
#define  SS_LOW                             SPI_SET_SS_LOW(SPI_Flash);
#define SPI_WRITE(n)           SPI_WRITE_TX(SPI_Flash, n)

void WINBOND25X16A_SendCommandData(uint32_t u32Cmd, uint32_t u32CmdSize)
{
    SPI_SetBitLength(u32CmdSize);
    SPI_WRITE(u32Cmd);
    // while(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI_Flash)==0){};
    Check_SPI_BUSY;

}

void WINBOND25X16A_ReadDataByByte(uint8_t *volatile pu8Data)
{
    uint32_t u32Data;
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
    SPI_SetBitLength(1);
    SPI_WRITE(0xff);
    // while(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI_Flash)==0){};

    Check_SPI_BUSY;
    u32Data = SPI_READ;
    *pu8Data = u32Data & 0xFF;
}

void WINBOND25X16A_ReadDataByWord(uint8_t *pu8Data)
{
    uint32_t u32Data;
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
    SPI_SetBitLength(4);
    SPI_WRITE(0xffffffff);
    //while(SPI_GET_TX_FIFO_EMPTY_FLAG(SPI_Flash)==0){};
    Check_SPI_BUSY;
    u32Data = SPI_READ;
    pu8Data[0] = (u32Data >> 24) & 0xFF;
    pu8Data[1] = (u32Data >> 16) & 0xFF;
    pu8Data[2] = (u32Data >>  8) & 0xFF;
    pu8Data[3] = u32Data & 0xFF;
}

void WINBOND25X16A_GetStatus(uint8_t *pu8Status)
{
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
    SS_LOW;
    WINBOND25X16A_SendCommandData(DRVSPIFLASH_READ_STATUS, 0x1);
    WINBOND25X16A_ReadDataByByte(pu8Status);
    SS_HIGH;

}


uint8_t WINBOND25X16A_WaitIdle(void)
{
    uint8_t u8Status = 0;

    do
    {
        WINBOND25X16A_GetStatus(&u8Status);
    } while ((u8Status & DRVSPIFLASH_BUSY));

    return u8Status;
}


void WINBOND25X16A_EnableWrite(uint8_t bEnable)
{
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
    SS_LOW;

    if (bEnable)
        WINBOND25X16A_SendCommandData(DRVSPIFLASH_WRITE_ENABLE, 0x1);
    else
        WINBOND25X16A_SendCommandData(DRVSPIFLASH_WRITE_DISABLE, 0x1);

    SS_HIGH;
    WINBOND25X16A_WaitIdle();

}

void WINBOND25X16A_EraseSector(uint32_t u32StartSector)
{
    uint32_t u32StartAddr;
    uint8_t  u8Status = 0;

    WINBOND25X16A_EnableWrite(TRUE);
    WINBOND25X16A_WaitIdle();
    SS_LOW;
    u32StartAddr = u32StartSector & 0x0FFFF000;
    WINBOND25X16A_SendCommandData(((DRVSPIFLASH_SECTOR_ERASE << 24) | u32StartAddr), 0x4);
    SS_HIGH;

    do
    {
        WINBOND25X16A_GetStatus(&u8Status);
    } while ((u8Status & DRVSPIFLASH_WEL));

}

int32_t WINBOND25X16A_ProgramPage(uint32_t u32StartPage, uint8_t *pu8Data)
{
    uint32_t u32StartAddr;
    uint32_t u32LoopCounter;
    uint32_t u32Data;
    uint8_t  u8Status;

    WINBOND25X16A_EnableWrite(TRUE);

    u8Status = WINBOND25X16A_WaitIdle();

    if (u8Status & DRVSPIFLASH_WEL)
    {
        SS_LOW;
        u32StartAddr = u32StartPage & 0x0FFFFF00;
        WINBOND25X16A_SendCommandData(((DRVSPIFLASH_PAGE_PROGRAM << 24) | u32StartAddr), 0x4);

        for (u32LoopCounter = 0; u32LoopCounter < DRVSPIFLASH_PAGE_SIZE; u32LoopCounter += 4)
        {
            u32Data = ((((pu8Data[u32LoopCounter] << 24) | (pu8Data[u32LoopCounter + 1] << 16)) |
                        (pu8Data[u32LoopCounter + 2] << 8)) + pu8Data[u32LoopCounter + 3]);

            WINBOND25X16A_SendCommandData(u32Data, 0x4);
        }

        SS_HIGH;
        WINBOND25X16A_WaitIdle();
        pu8Data += DRVSPIFLASH_PAGE_SIZE;
    }

    return 0;
}


int32_t WINBOND25X16A_ReadPage(
    uint8_t u8ReadMode,
    uint32_t u32StartPage,
    uint8_t *pu8Data
)
{
    uint32_t u32StartAddr = (u32StartPage & 0x0FFFFF00);
    uint32_t u32DataSize  = DRVSPIFLASH_PAGE_SIZE;
    uint32_t u32LoopCounter;

    SS_LOW;

    if (u8ReadMode == 0)
        WINBOND25X16A_SendCommandData(((DRVSPIFLASH_FAST_READ << 24) | u32StartAddr), 0x4);
    else
        WINBOND25X16A_SendCommandData(((DRVSPIFLASH_FAST_RD_DUAL << 24) | u32StartAddr), 0x4);

    WINBOND25X16A_SendCommandData(DRVSPIFLASH_DUMMY, 0x1);

    for (u32LoopCounter = 0; u32LoopCounter < u32DataSize; u32LoopCounter += 4)
        WINBOND25X16A_ReadDataByWord(&pu8Data[u32LoopCounter]);

    SS_HIGH;
    return 0;
}



void SpiRead(uint32_t addr, uint32_t size, uint32_t buffer)
{
    /* This is low level read function of USB Mass Storage */
    int32_t len;

    len = (int32_t)size;

    while (len >= DRVSPIFLASH_PAGE_SIZE)
    {
        WINBOND25X16A_ReadPage(0, addr, (uint8_t *)buffer);
        addr   += DRVSPIFLASH_PAGE_SIZE;
        buffer += DRVSPIFLASH_PAGE_SIZE;
        len    -= DRVSPIFLASH_PAGE_SIZE;
    }
}


void SpiWrite(uint32_t addr, uint32_t size, uint32_t buffer)
{
    /* This is low level write function of USB Mass Storage */
    int32_t len, i, offset;
    uint32_t *pu32;
    uint32_t alignAddr;

    len = (int32_t)size;

    if (len == DRVSPIFLASH_SECTOR_SIZE && ((addr & (DRVSPIFLASH_SECTOR_SIZE - 1)) == 0))
    {
        /* one-sector length & the start address is sector-alignment */
        WINBOND25X16A_EraseSector(addr);

        //WINBOND25X16A_EnableWrite(g_SpiPort, g_SlaveSel, TRUE);

        while (len >= DRVSPIFLASH_PAGE_SIZE)
        {
            WINBOND25X16A_ProgramPage(addr, (uint8_t *)buffer);
            len    -= DRVSPIFLASH_PAGE_SIZE;
            buffer += DRVSPIFLASH_PAGE_SIZE;
            addr   += DRVSPIFLASH_PAGE_SIZE;
        }
    }
    else
    {
        do
        {
            /* alignAddr: sector address */
            alignAddr = addr & 0x1FFFF000;

            /* Get the sector offset*/
            offset = (addr & (DRVSPIFLASH_SECTOR_SIZE - 1));

            if (offset || (size < DRVSPIFLASH_SECTOR_SIZE))
            {
                /* if the start address is not sector-alignment or the size is less than one sector, */
                /* read back the data of the destination sector to g_sectorBuf[].                    */
                SpiRead(alignAddr, DRVSPIFLASH_SECTOR_SIZE, (uint32_t)&g_sectorBuf[0]);
            }

            /* Update the data */
            pu32 = (uint32_t *)buffer;
            len = DRVSPIFLASH_SECTOR_SIZE - offset; /* len: the byte count between the start address and the end of a sector. */

            if (size < len) /* check if the range of data arrive at the end of a sector. */
                len = size; /* Not arrive at the end of a sector. "len" indicate the actual byte count of data. */

            for (i = 0; i < len / 4; i++)
            {
                g_sectorBuf[offset / 4 + i] = pu32[i];
            }

            WINBOND25X16A_EraseSector(alignAddr);


            //WINBOND25X16A_EnableWrite(g_SpiPort, g_SlaveSel, TRUE);


            for (i = 0; i < 16; i++) /* one sector (16 pages) */
            {
                WINBOND25X16A_ProgramPage(alignAddr + (i << 8), (uint8_t *)g_sectorBuf + (i << 8));
            }

            size -= len;
            addr += len;
            buffer += len;

        } while (size > 0);
    }
}



void SpiChipErase(void)
{
    WINBOND25X16A_EnableWrite(TRUE);

    SS_LOW;

    WINBOND25X16A_SendCommandData(DRVSPIFLASH_CHIP_ERASE, 0x1);

    SS_HIGH;

    WINBOND25X16A_WaitIdle();
    SPI_ClearRxFIFO(SPI_Flash);
    SPI_ClearTxFIFO(SPI_Flash);
}

uint32_t SpiReadMidDid(void)
{

    uint32_t au32DestinationData;

    SS_LOW;

    WINBOND25X16A_SendCommandData(DRVSPIFLASH_DEVICE_ID, 0x1);

    WINBOND25X16A_SendCommandData(DRVSPIFLASH_ZERO, 0x3);

    WINBOND25X16A_SendCommandData(DRVSPIFLASH_ZERO, 0x2);
    SS_HIGH;
    au32DestinationData = SPI_READ & 0XFFFF;


    SPI_ClearRxFIFO(SPI_Flash);//clear fifo
    SPI_ClearTxFIFO(SPI_Flash);
    return au32DestinationData;

}


