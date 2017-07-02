#include "iap.h"
#include "stmflash.h"
#include <string.h>
void IAP_FLASH_WriteFlag(u16 flag) 
{
	FLASH_Unlock();
	STMFLASH_Write(IAP_FLASH_FLAG_ADDR, &flag, 1);
	FLASH_Lock();
}

u32 IAP_FLASH_ReadFlag(void)
{
	return STMFLASH_ReadHalfWord(IAP_FLASH_FLAG_ADDR);   
}

void IAP_Init(void)
{
#ifdef USE_IAP
	u16 clearFlag = 0x0000;  //��־Ϊ����״̬
	IAP_FLASH_WriteFlag( clearFlag );//���IAP������־
	NVIC_SetVectorTable(ApplicationAddress, IAP_FLASH_SIZE);//�����ж�������
#endif
}

void IAP_Handle(u8 * cmd)
{
#ifdef USE_IAP
	if(strcmp((char *)cmd, "update") == 0)
	{
		IAP_FLASH_WriteFlag(UPDATE_FLAG_DATA);
		NVIC_SystemReset();
	}
	else if(strcmp((char *)cmd, "erase") == 0)
	{
		IAP_FLASH_WriteFlag(ERASE_FLAG_DATA);
		NVIC_SystemReset();		
	}
	else if(strcmp((char *)cmd, "menu") == 0)
	{
		IAP_FLASH_WriteFlag(INIT_FLAG_DATA);
		NVIC_SystemReset();	
	}
	else if(strcmp((char *)cmd, "runapp") == 0)//reset
	{
		NVIC_SystemReset();	
	}
	else
	{
		//printf("ָ������\r\n");
		NVIC_SystemReset();	
	}
#endif
}

