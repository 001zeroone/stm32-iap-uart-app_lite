#include <string.h>
#include "led.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"
//A6 A7 B0 B1

//#define USE_IAP
#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4


int main(void)
{
	IAP_Init();          //�������֮������ִ��iap��ʼ�� ����־λ���
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);   //���ڳ�ʼ��Ϊ9600
 	LED_Init();	         //LED�˿ڳ�ʼ��
	TIM3_PWM_Init(719, 0);//����Ƶ��PWMƵ��=72000/719=10Khz
	while(1)
	{
		if(USART_RX_STA&0x80)
		{					 
			USART_RX_STA=0;
			IAP_Handle(USART_RX_BUF);
		}
	}	 
}
