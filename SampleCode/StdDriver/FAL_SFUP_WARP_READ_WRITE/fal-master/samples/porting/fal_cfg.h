/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

//nclude <rtconfig.h>
//#include <board.h>
#define FAL_PART_HAS_TABLE_CFG


/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev m031_onchip_flash;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                     \
{                                                               \
  &nor_flash0,                                                  \
	&m031_onchip_flash,                                           \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
	  {FAL_PART_MAGIC_WORD,       "bank0", 	      "norflash0",   	 0, 		 4194304, 0}, \
    {FAL_PART_MAGIC_WORD,       "bank1",     "m031_onchip",         0,   64*1024, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
