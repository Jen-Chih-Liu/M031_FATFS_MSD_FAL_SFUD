/*-----------------------------------------------------------------------*/
/* Low level disk control module for Win32              (C)ChaN, 2007    */
/*-----------------------------------------------------------------------*/


#include <stdio.h>

#include <string.h>
#include <stdint.h>
#include "SPI_FLASH.h"
#include "diskio.h"

extern uint32_t SpiReadMidDid(void);
extern void SpiInit(void);	
extern void SpiRead(uint32_t addr, uint32_t size, uint32_t buffer);	
extern void SpiWrite(uint32_t addr, uint32_t size, uint32_t buffer);


#define STORAGE_BUFFER_SIZE 1024        /* Data transfer buffer size in 512 bytes alignment */
unsigned int Storage_Block[STORAGE_BUFFER_SIZE / 4];
#define STORAGE_DATA_BUF   ((uint32_t)&Storage_Block[0])

//extern unsigned char SD_WriteDisk(unsigned char *buf,unsigned int  sector,unsigned char cnt);
//extern unsigned char SD_ReadDisk(unsigned char *buf,unsigned int sector,unsigned char cnt);
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
	   uint32_t u32SpiMidDid;
	/*	
	if(SD_Initialize()==0)
	{	
	sta = 	RES_OK;
		printf("SDCard Open success\n");
	}
	else
	{
	sta = STA_NOINIT;
		printf("SDCard Open failed\n");
	}
	*/
	SpiInit();
	    /* read MID & DID */
	u32SpiMidDid = SpiReadMidDid();
	if ((u32SpiMidDid & 0xFFFF) == 0xEF15)
	{	
	sta = 	RES_OK;
	printf("SPI FLASH Open success\n");
	}
	else
	{
	sta = STA_NOINIT;
	printf("SPI FLASH Open failed\n");
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
	if (drv) 
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
		  SpiRead(address, size, (uint32_t)buff);			

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
		SpiWrite(address, size,(uint32_t)buff);
	//SD_WriteDisk((unsigned char *)buff,sector,count);
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
		//DrvSDCARD_GetCardSize(buff);
	 *(DWORD*)buff = 4096*2;
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




