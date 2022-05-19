/*-----------------------------------------------------------------------*/
/* Low level disk control module for Win32              (C)ChaN, 2007    */
/*-----------------------------------------------------------------------*/


#include <stdio.h>

#include <string.h>
#include <stdint.h>
#include "diskio.h"
#include "fal.h"
extern int FAL_FLASH_Read(const char *partiton_name,uint32_t addr, uint32_t size, uint32_t buffer);
extern int FAL_FLASH_Write(const char *partiton_name, uint32_t addr, uint32_t size, uint32_t buffer);
extern uint32_t fal_partition_size(const char *partiton_name);

void RoughDelay(unsigned int t)
{
    volatile int delay;

    delay = t;

    while(delay-- >= 0);
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber */
)
{
	DSTATUS sta;

if (fal_init()>=0)
{
	fal_show_part_table();
	sta = 	RES_OK;
	}
	else
	{
	sta = STA_NOINIT;
	
	}
	return sta;
	

}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{   
	DSTATUS sta1=STA_OK;
	if (drv!=0) 
		sta1 =   STA_NOINIT;
	return sta1;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
extern unsigned char  SD_Type;

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	DRESULT res;
	  uint32_t size,address;
	  	if (drv) {
			res = (DRESULT)STA_NOINIT;	
		return res;
		}


     if(count==0||count>=2)
	   	{	 
		res =   (DRESULT)STA_NOINIT;
		return res;
	}
	    address= sector*512;
	    size=count*512;
	//printf("r a:0x%x, size:%d ",address,size)	;	
    FAL_FLASH_Read("bank1",address,size,(uint32_t)buff);
		res =RES_OK;	/* Clear STA_NOINIT */;

	return res;
}

/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */
unsigned long get_fattime (void)
{
	unsigned long tmr;

    tmr=0x00000;

	return tmr;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{

	DRESULT  res;	
      uint32_t size;
	uint32_t address;

 	if (drv) {
		res = (DRESULT)STA_NOINIT;	
		return res;
	}


	     
    if(count==0||count>=2)
	{	 
		res = (DRESULT)  STA_NOINIT;
		return res;
	}
	    size=count*512;

		address= sector*512;
	//printf("r a:0x%x, size:%d ",address,size)	;	
 FAL_FLASH_Write("bank1",address,size,(uint32_t)buff);
	    res = RES_OK;

	return res;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;

//	BYTE n;

	if (drv) return RES_PARERR;

	switch (ctrl) {
	case CTRL_SYNC :		/* Make sure that no pending write process */
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
   printf("size %d\n\r",fal_partition_size("bank1"));
	 *(DWORD*)buff = fal_partition_size("bank1")/512;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
		*(DWORD*)buff = 512;	//512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		*(DWORD*)buff = 1;
		res = RES_OK;
		break;


	default:
		res = RES_PARERR;
	}


	res = RES_OK;


	return res;
}




