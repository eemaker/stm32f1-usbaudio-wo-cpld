//ALIENTEK ������POS�� STM32������ʵ��51
//USB�첽���� ʵ��  
//����֮�� rush �����޸�
#include "led.h"
#include "delay.h"
//#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	 

#include "i2splayer.h"


 	 
int main(void)
{	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//JTAG->SWD
 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��
 	LED_Init();			     //LED�˿ڳ�ʼ��
//	KEY_Init();	 	
  
	//USB����
	usb_all_init();  
 	USB_Init();	 
	DAC_INIT();
//	LED0=1;

	   			   
 //	printf("USB Connecting...");	 
	while(1)
	{
/*		if(bDeviceState==CONFIGURED)//USB�������ˣ�
		{
			LED0=1;							 
			//printf("USB Connected"); 
		}else
		{
 			LED0=0;							
			//printf("USB DisConnected");	 
		}	    
		LED1=!LED1;
*/
//		delay_ms(500);

		
	};  										    			    
}



