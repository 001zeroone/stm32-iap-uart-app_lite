#ifndef __IAP_H_
#define __IAP_H_
#include "sys.h"


#define APPRUN_FLAG_DATA      0x0000   //APP����Ҫ���κδ���ֱ������״̬
#define INIT_FLAG_DATA        0xFFFF   //Ĭ�ϱ�־������(��Ƭ�ӵ����)
#define UPDATE_FLAG_DATA      0xEEEE   //���ر�־������
#define UPLOAD_FLAG_DATA      0xDDDD   //�ϴ���־������
#define ERASE_FLAG_DATA       0xCCCC   //������־������

#define IAP_FLASH_SIZE  0x3000//Bootloader�����С
#define IAP_FLASH_FLAG_ADDR  0x8002800//��־���ݴ�ŵ�ַ�������Bootloader������
#define ApplicationAddress  0x8003000//Ӧ�ó����ַ

void IAP_FLASH_WriteFlag(u16 flag);
u32 IAP_FLASH_ReadFlag(void);
void IAP_Init(void);
void IAP_Handle(u8 * cmd);
#endif
