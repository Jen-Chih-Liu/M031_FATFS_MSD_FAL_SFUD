#include <stdio.h>
#include "NuMicro.h"
#include "fal.h"
uint32_t g_sectorBuf[1024]; /* 4096 bytes */

int FAL_FLASH_Read(const char *partiton_name,uint32_t addr, uint32_t size, uint32_t buffer)
{
  int32_t len;

    len = (int32_t)size;
    const struct fal_flash_dev *flash_dev = NULL;
    const struct fal_partition *partition = NULL;
  int ret;
    if (!partiton_name)
    {
        printf("Input param partition name is null!");
        return -1;
    }

    partition = fal_partition_find(partiton_name);
    if (partition == NULL)
    {
        printf("Find partition (%s) failed!", partiton_name);
        ret = -1;
        return ret;
    }
		
		
		flash_dev = fal_flash_device_find(partition->flash_name);
    if (flash_dev == NULL)
    {
        printf("Find flash device (%s) failed!", partition->flash_name);
        ret = -1;
        return ret;
    }

    do
    {        
			  ret = fal_partition_read(partition, addr, (uint8_t *)buffer, flash_dev->blk_size);
        if (ret < 0)
        {
            printf("Partition (%s) read failed!", partition->name);
            ret = -1;
            return ret;
        }
			
        addr   += flash_dev->blk_size;
        buffer += flash_dev->blk_size;
        len    -= flash_dev->blk_size;
    }while( len > 0);
	  ret = 0;
    return ret;
}

int FAL_FLASH_Write(const char *partiton_name, uint32_t addr, uint32_t size, uint32_t buffer)
{

 int32_t len, i, offset;
    uint32_t *pu32;
    uint32_t alignAddr;

    len = (int32_t)size;
    const struct fal_flash_dev *flash_dev = NULL;
    const struct fal_partition *partition = NULL;
  int ret;
    if (!partiton_name)
    {
        printf("Input param partition name is null!");
        return -1;
    }

    partition = fal_partition_find(partiton_name);
    if (partition == NULL)
    {
        printf("Find partition (%s) failed!", partiton_name);
        ret = -1;
        return ret;
    }
		
		flash_dev = fal_flash_device_find(partition->flash_name);
    if (flash_dev == NULL)
    {
        printf("Find flash device (%s) failed!", partition->flash_name);
        ret = -1;
        return ret;
    }
	
		
	if (len == flash_dev->blk_size && ((addr % flash_dev->blk_size) == 0))
    {
       
        fal_partition_erase(partition,addr,flash_dev->blk_size);
			  fal_partition_write(partition, addr,  (uint8_t *)buffer, flash_dev->blk_size);
    }
    else
    {
        do
        {
            /* alignAddr: sector address */
            alignAddr = (addr / flash_dev->blk_size);

            /* Get the sector offset*/
            offset = addr%flash_dev->blk_size;

            if (offset || (size < flash_dev->blk_size))
            {
                /* if the start address is not sector-alignment or the size is less than one sector, */
                /* read back the data of the destination sector to g_sectorBuf[].                    */
                FAL_FLASH_Read(partiton_name ,alignAddr*flash_dev->blk_size, flash_dev->blk_size, (uint32_t)&g_sectorBuf[0]);
            }

            /* Update the data */
            pu32 = (uint32_t *)buffer;
            len = flash_dev->blk_size - offset; /* len: the byte count between the start address and the end of a sector. */

            if (size < len) /* check if the range of data arrive at the end of a sector. */
                len = size; /* Not arrive at the end of a sector. "len" indicate the actual byte count of data. */

            for (i = 0; i < len / 4; i++)
            {
                g_sectorBuf[offset / 4 + i] = pu32[i];
            }

            
            fal_partition_erase(partition,alignAddr*flash_dev->blk_size,flash_dev->blk_size);            				  
            fal_partition_write(partition, alignAddr*flash_dev->blk_size,  (uint8_t *)g_sectorBuf, flash_dev->blk_size);

            size -= len;
            addr += len;
            buffer += len;

        } while (size > 0);
    }	
		
		
		
	 ret = 0;
   return ret;
}


uint32_t fal_partition_size(const char *partiton_name)
{
    const struct fal_flash_dev *flash_dev = NULL;
    const struct fal_partition *partition = NULL;

    if (!partiton_name)
    {
        printf("Input param partition name is null!");
    
    }

    partition = fal_partition_find(partiton_name);
    if (partition == NULL)
    {
        printf("Find partition (%s) failed!", partiton_name);
    }
		

    flash_dev = fal_flash_device_find(partition->flash_name);
    if (flash_dev == NULL)
    {
        printf("Find flash device (%s) failed!", partition->flash_name);
      
    }
		return flash_dev->len;
}
