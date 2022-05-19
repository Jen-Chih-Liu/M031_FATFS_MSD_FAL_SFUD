/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-26     armink       the first version
 */

#include <fal.h>
#include "NuMicro.h"

static int init(void)
{
    /* do nothing now */

    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = m031_onchip_flash.addr + offset;
    for (i = 0; i < size; i++, addr++, buf++)
    {
        *buf = *(uint8_t *) addr;
    }

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    size_t i;
    uint32_t addr = m031_onchip_flash.addr + offset;
    uint32_t read_data;
    uint32_t *buf_32 = (uint32_t *)buf;
    
 /* Unlock protected registers to operate FMC ISP function */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();
	
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */

	
    for (i = 0; i < size; i += 4, buf_32++, addr += 4) {
        /* write data */
        FMC_Write(addr, *buf_32);
        read_data = *(uint32_t *)addr;
        /* check data */
        if (read_data != *buf_32) {
            return -1;
        }
    }

    FMC_DISABLE_AP_UPDATE();            /* Disable APROM update. */

    /* Disable FMC ISP function */
    FMC_Close();

    /* Lock protected registers */
    SYS_LockReg();
    return size;
}

static int erase(long offset, size_t size)
{
size_t i=0;

    size_t erase_pages=0;
    uint32_t addr = m031_onchip_flash.addr + offset;

    /* calculate pages */
    erase_pages = size / FMC_FLASH_PAGE_SIZE;
    if (size % FMC_FLASH_PAGE_SIZE != 0) {
        erase_pages++;
    }

 /* Unlock protected registers to operate FMC ISP function */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();
	
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */

    for (i = 0; i < erase_pages; i++) {
        FMC_Erase(addr + (FMC_FLASH_PAGE_SIZE * i));
    
    }
    FMC_DISABLE_AP_UPDATE();            /* Disable APROM update. */

    /* Disable FMC ISP function */
    FMC_Close();

    /* Lock protected registers */
    SYS_LockReg();

    return size;
}

const struct fal_flash_dev m031_onchip_flash =
{
    .name       = "m031_onchip",
    .addr       = 256*1024,
    .len        = 256*1024,
    .blk_size   = 2*1024,
    .ops        = {init, read, write, erase},
    .write_gran = 32
};
