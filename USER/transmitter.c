#include <string.h>
#include "key.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"

//#define USE_IAP
static u8 current_ch = 0;
static u16 current_val = 0;
static u16 ch1_val, ch2_val, ch3_val, ch4_val = 0;

#define CH1 1            //PA6
#define CH2 2            //PA7
#define CH3 3            //PB0
#define CH4 4            //PB1

#define FUN_KEY 1        //PA13
#define VAL_ADD_KEY 2    //PA15
#define VAL_SUB_KEY 3    //PA0

#define CHx_Change_Val(rule, value) \
{\
	if(rule == VAL_ADD_KEY)\
		value = value >= 700 ? 700 : value + 10;\
	else if(rule == VAL_SUB_KEY)\
		value = value <= 10 ? 0 : value - 10;\
	current_val = value;\
}

void Calc_CHx_Val(u8 rule)
{
	switch(current_ch)
	{
		case CH1:
			CHx_Change_Val(rule, ch1_val);
			break;
		case CH2:
			CHx_Change_Val(rule, ch2_val);
			break;
		case CH3:
			CHx_Change_Val(rule, ch3_val);
			break;
		case CH4:
			CHx_Change_Val(rule, ch4_val);
			break;
		default: 
			break;
	}
}

void Send_Meg(u8 rule)
{
	if(rule == FUN_KEY)
	{
		USART_RX_BUF[0] = '@';
		USART_RX_BUF[1] = current_ch + '0';
		USART_RX_BUF[2] = '\r';
		USART_RX_BUF[3] = '\n';
		printf("%s", (char *)USART_RX_BUF);
		memset(USART_RX_BUF, 0, 4);
	}
	else
	{
		USART_RX_BUF[0] = '$';
		USART_RX_BUF[1] = current_val / 100 + '0';
		USART_RX_BUF[2] = current_val % 100 / 10 + '0';
		USART_RX_BUF[3] = current_val % 10 + '0';
		USART_RX_BUF[4] = '\r';
		USART_RX_BUF[5] = '\n';
		printf("%s", (char *)USART_RX_BUF);	
		memset(USART_RX_BUF, 0, 6);
	}

}

int main(void)
{
	IAP_Init();          //�������֮������ִ��iap��ʼ�� ����־λ���
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);   //���ڳ�ʼ��Ϊ115200
 	KEY_Init();	         //LED�˿ڳ�ʼ��
	while(1)
	{
		switch(KEY_Scan())
		{
			case FUN_KEY:
				current_ch = (current_ch + 1) % 5;
				current_ch = current_ch == 0 ? 1 : current_ch;
				Send_Meg(FUN_KEY);
				break;
			case VAL_ADD_KEY:
				Calc_CHx_Val(VAL_ADD_KEY);
				Send_Meg(VAL_ADD_KEY);
				break;
			case VAL_SUB_KEY:
				Calc_CHx_Val(VAL_SUB_KEY);
				Send_Meg(VAL_SUB_KEY);
				break;
			default: 
				break;
		}
	}	 
}
