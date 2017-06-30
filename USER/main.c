#include "led.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "iap.h"
//A6 A7 B0 B1
int main(void)
{
 	u16 led0pwmval=0;
	u8 dir=1;	
	//IAP_Init();//�������֮������ִ��iap��ʼ�� ����־λ���
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);   //���ڳ�ʼ��Ϊ9600
 	LED_Init();	         //LED�˿ڳ�ʼ��
	TIM3_PWM_Init(899,0);	 //����Ƶ��PWMƵ��=72000/900=8Khz
	while(1)
	{
		if(USART_RX_STA&0x80)
		{					 
			USART_RX_STA=0;
			IAP_Handle(USART_RX_BUF);
		}
		delay_ms(10);   
		if(dir)led0pwmval++;
		else led0pwmval--;

 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;										 
		TIM_SetCompare1(TIM3,led0pwmval);	
		TIM_SetCompare2(TIM3,led0pwmval);
		TIM_SetCompare3(TIM3,led0pwmval);
		TIM_SetCompare4(TIM3,led0pwmval);		

	}	 
}

