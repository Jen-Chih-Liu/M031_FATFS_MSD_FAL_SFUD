#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__
void WINBOND25X16A_SendCommandData( uint32_t u32Cmd, uint32_t u32CmdSize);
void WINBOND25X16A_ReadDataByByte(uint8_t * volatile pu8Data);
void WINBOND25X16A_ReadDataByWord(uint8_t * pu8Data);
void WINBOND25X16A_GetStatus(uint8_t * pu8Status);	
uint8_t WINBOND25X16A_WaitIdle(void);	
void WINBOND25X16A_EnableWrite(uint8_t bEnable);	
void WINBOND25X16A_EraseSector(uint32_t u32StartSector);
int32_t WINBOND25X16A_ProgramPage(uint32_t u32StartPage, uint8_t * pu8Data);
int32_t WINBOND25X16A_ReadPage(uint8_t u8ReadMode,uint32_t u32StartPage, uint8_t * pu8Data);
void SpiInit(void);	
void SpiRead(uint32_t addr, uint32_t size, uint32_t buffer);	
void SpiWrite(uint32_t addr, uint32_t size, uint32_t buffer);
void SpiChipErase(void);
uint32_t SpiReadMidDid(void);
#endif
