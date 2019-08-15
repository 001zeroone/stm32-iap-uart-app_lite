#include "timer.h"
#include "delay.h"
#include "sys.h"

/***************************************************************************/

#define DBG_LED_RCC     RCC_APB2Periph_GPIOA
#define DBG_LED_GPIO    GPIOA
#define DBG_LED_PIN     GPIO_Pin_9
#define DBG_LED_INDEX   9
#define DEG_LED_MODE    GPIO_Mode_Out_OD
#define DBG_LED_SPEED   GPIO_Speed_50MHz
/***************************************************************************/

typedef enum
{ 
    All_Off = 0,
    General,
    Single_Blue,
    Single_Red,
    Pwm_Blue
}LedStatus;
volatile LedStatus currentState = All_Off;

typedef enum
{
    OFF = 0,
    ON
}LedValue;
volatile LedValue currentLedValue = OFF;

void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DBG_LED_RCC , ENABLE);  
    
    GPIO_InitStructure.GPIO_Pin = DBG_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_ResetBits(DBG_LED_GPIO, DBG_LED_PIN);	
    GPIO_Init(DBG_LED_GPIO, &GPIO_InitStructure);
}

void LED_ConfigValue(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, LedValue nextValue)
{
    uint8_t posbit = 0;
    if(currentLedValue == nextValue) return;
    if((currentState == All_Off) || (currentState == Pwm_Blue)) return;
    switch(currentState)
    {
        case General:
        {
            if(nextValue == ON) GPIOx->BSRR = 0x01 << GPIO_Pin;
            else GPIOx->BRR = 0x01 << GPIO_Pin;
        }
        break;
        case Single_Blue:
        {            
            if(nextValue == ON) GPIOx->BRR = 0x1 << GPIO_Pin;
            else GPIOx->BSRR = 0x1 << GPIO_Pin;
        }
        break;
        case Single_Red:
        {
            posbit = (GPIO_Pin >= 8) ? (4 * (GPIO_Pin - 8)) : (4 * GPIO_Pin);
            if(nextValue == ON ) {
                if(GPIO_Pin >= 8) GPIOx->CRH &= ~(0xC << posbit);
                else GPIOx->CRL &= ~(0xC << posbit);
            }
            else {
                if(GPIO_Pin >= 8) GPIOx->CRH |= (0x4 << posbit); 
                else GPIOx->CRL |= (0x4 << posbit); 
            }
        }
        break;
        default: break;
    }
    currentLedValue = nextValue;
}

void LED_State_Handler(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, LedStatus nextState)
{
    uint8_t posbit = 0;
    if(currentState == nextState) return;
    posbit = (GPIO_Pin >= 8) ? (4 * (GPIO_Pin - 8)) : (4 * GPIO_Pin);
    
    switch(nextState)
    {
        case All_Off: {
            if(currentState == Pwm_Blue) {
                PWM_TIM_OFF();
            }
            GPIOx->BSRR = 0x1 << GPIO_Pin; 
            if(GPIO_Pin >= 8) {
                GPIOx->CRH &= ~(0xC << posbit); 
                GPIOx->CRH |= 0x4 << posbit;            
            } else {
                GPIOx->CRL &= ~(0xC << posbit); 
                GPIOx->CRL |= 0x4 << posbit;                
            }
        } 
        break;
        case General: {
            if(currentState == Pwm_Blue) {
                PWM_TIM_OFF();
            }
            if(GPIO_Pin >= 8) {
                GPIOx->CRH &= ~(0xC << posbit);
            } else {
                GPIOx->CRL &= ~(0xC << posbit);
            }
            GPIOx->BSRR = 0x1 << GPIO_Pin;
        } 
        break;
        case Single_Blue: {
            if(currentState == Pwm_Blue) {
                PWM_TIM_OFF();
            }
            GPIOx->BSRR = 0x1 << GPIO_Pin; 
            if(GPIO_Pin >= 8) {
                GPIOx->CRH &= ~(0xC << posbit);            
                GPIOx->CRH |= (0x4 << posbit);            
            } else {
                GPIOx->CRL &= ~(0xC << posbit);            
                GPIOx->CRL |= (0x4 << posbit);           
            }
        } 
        break;
        case Single_Red: {
            if(currentState == Pwm_Blue) {
                PWM_TIM_OFF();
            }
            GPIOx->BSRR = 0x1 << GPIO_Pin;  
            if(GPIO_Pin >= 8) {
                GPIOx->CRH &= ~(0xC << posbit);
                GPIOx->CRH |= (0x4 << posbit);          
            } else {
                GPIOx->CRL &= ~(0xC << posbit);
                GPIOx->CRL |= (0x4 << posbit);
            }
        } 
        break;
        case Pwm_Blue: {
            PWM_TIM_ON();
            GPIOx->BRR = 0x1 << GPIO_Pin;  
            if(GPIO_Pin >= 8) {
                GPIOx->CRH &= ~(0xC << posbit); 
                GPIOx->CRH |= 0xC << posbit;           
            } else {
                GPIOx->CRL &= ~(0xC << posbit); 
                GPIOx->CRL |= 0xC << posbit;
            }
        } 
        break;
        default: break;
    }
    currentState = nextState;
}

uint8_t GetPinPos(uint16_t PINx)
{
    uint8_t pos = 0;
    assert_param(IS_GET_GPIO_PIN(PINx));
    if(PINx >= GPIO_Pin_8)
    {
        while((0x01<<(pos + 8)) != PINx) pos++;
        return pos + 8;  
    }
    else
    {
        while((0x01<<pos) != PINx) pos++;
        return pos; 
    }
}



int main(void)
{
    uint8_t cnt = 0;
    //NVIC_SetVectorTable(FLASH_BASE, 0x4000);//???????
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    LED_Init();
    TIM1_PWM_Init(99, 71);     //PWM = 72000000/(99+1)/(71+1)=10Khz
    TIM3_Int_Init(9999,71);    //T = 10ms not enable
    
    //Test1************************************************
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, General);
    for(cnt = 0; cnt < 5; cnt++)
    {
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, ON);
        delay_ms(500);
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, OFF);
        delay_ms(500);
    }
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, All_Off);
    delay_ms(1000);
    
    //Test2************************************************
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, Single_Blue);
    for(cnt = 0; cnt < 5; cnt++)
    {
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, ON);
        delay_ms(500);
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, OFF);
        delay_ms(500);
    }
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, All_Off);
    delay_ms(1000);

    //Test3************************************************
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, Single_Red);
    for(cnt = 0; cnt < 5; cnt++)
    {
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, ON);
        delay_ms(500);
        LED_ConfigValue(DBG_LED_GPIO, DBG_LED_INDEX, OFF);
        delay_ms(500);
    }
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, All_Off);
    delay_ms(1000);
    
    //Test4************************************************
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, Pwm_Blue);
    for(cnt = 0; cnt < 5; cnt++)
    {
        delay_ms(1000);
    }
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, All_Off);
    delay_ms(1000);
    
    //Test5************************************************
    LED_State_Handler(DBG_LED_GPIO, DBG_LED_INDEX, Pwm_Blue);
	while(1)
	{
        ;
	}
}
