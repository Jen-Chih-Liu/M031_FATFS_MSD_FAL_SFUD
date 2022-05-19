/**************************************************************************//**
* @file     main.c
* @version  V1.00
* @brief    Demonstrate how to access SPI flash.
*
* SPDX-License-Identifier: Apache-2.0
* @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "diskio.h"
#include "ff.h"
#include "string.h"
void SYS_Init(void)
{
    /* Enable HIRC clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Enable module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Set module clock */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();
}

void UART0_Init()
{
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}


DWORD acc_size;             /* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
FATFS FatFs[_DRIVES];       /* File system object for logical drive */
char Line[256];             /* Console input buffer */
#if _USE_LFN
    char Lfname[512];
#endif
BYTE Buff[1024] ;       /* Working buffer */

uint8_t data[512];

void put_rc(FRESULT rc)
{
    const TCHAR *p =
        _T("OK\0DISK_ERR\0INT_ERR\0NOT_READY\0NO_FILE\0NO_PATH\0INVALID_NAME\0")
        _T("DENIED\0EXIST\0INVALID_OBJECT\0WRITE_PROTECTED\0INVALID_DRIVE\0")
        _T("NOT_ENABLED\0NO_FILE_SYSTEM\0MKFS_ABORTED\0TIMEOUT\0LOCKED\0")
        _T("NOT_ENOUGH_CORE\0TOO_MANY_OPEN_FILES\0");

    uint32_t i;

    for (i = 0; (i != (UINT)rc) && *p; i++)
    {
        while (*p++) ;
    }

    printf(_T("rc=%u FR_%s\n"), (UINT)rc, p);
}
int main(void)
{
    uint32_t p1, s1, s2;
    FATFS *fs;              /* Pointer to file system object */
    DIR dir;                /* Directory object */
    FILINFO Finfo;

    char *ptr = "\\";
    FRESULT res;
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
    res = (FRESULT)disk_initialize(0);

    if (res)
    {
        put_rc(res);
        printf("\n\nDon't initialize SD card\n");
    }

  		 res = f_mount(0, &FatFs[0]);

		
    if (res)
    {
        put_rc(res);
        printf("Don't mount file system\n");
    } 

#if 1
    res = f_mkfs(0, 0, _MAX_SS);

    if (res)
    {
        put_rc(res);
        printf("Don't format\n");
    }
#endif

    // List direct information
    put_rc(f_opendir(&dir, ptr));
    p1 = s1 = s2 = 0;

    for (;;)
    {
        res = f_readdir(&dir, &Finfo);

        if ((res != FR_OK) || !Finfo.fname[0]) break;

        if (Finfo.fattrib & AM_DIR)
        {
            s2++;
        }
        else
        {
            s1++;
            p1 += Finfo.fsize;
        }

        printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s\n",
               (Finfo.fattrib & AM_DIR) ? 'D' : '-',
               (Finfo.fattrib & AM_RDO) ? 'R' : '-',
               (Finfo.fattrib & AM_HID) ? 'H' : '-',
               (Finfo.fattrib & AM_SYS) ? 'S' : '-',
               (Finfo.fattrib & AM_ARC) ? 'A' : '-',
               (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
               (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname);
    }

    printf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);

    if (f_getfree(ptr, (DWORD *)&p1, &fs) == FR_OK)
        printf(", %10lu bytes free\n", p1 * fs->csize * 512);

		while(1){};
}
