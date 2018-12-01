/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"		  
//#include "platform_config.h"
#include "usb_pwr.h"
#include "sys.h"	   
#include "led.h"
		   

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
	bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
	else
	{
		bDeviceState = ATTACHED;
	}	    
}  

//USB�ж�����
void USB_Interrupts_Config(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;


	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
											  //  ������18�ϵ��ж�
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; // USB resume from suspend mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//line 18���¼��������ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 	 

	/* Enable the USB interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	//��2�����ȼ���֮ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USB Wake-up interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;   //��2�����ȼ����	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);   	 
}							 		   

/* Define the STM32F10x hardware depending on the used evaluation board */
#define USB_DISCONNECT                      GPIOA  
#define USB_DISCONNECT_PIN                  GPIO_Pin_15
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOA
	   					  
/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/  
void USB_Cable_Config (FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
//		LED1=1;
		GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
	else
	{
//		LED1=0;
    	GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
}


/*******************************************************************************
* Function Name  : Speaker_Timer_Config
* Description    : Configure and enable the timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Speaker_Config(void)
{ 
 
		 
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  u32 Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(u32*)(0x1FFFF7E8);
  Device_Serial1 = *(u32*)(0x1FFFF7EC);
  Device_Serial2 = *(u32*)(0x1FFFF7F0);

  if (Device_Serial0 != 0)
  {
    Speaker_StringSerial[2] = (u8)(Device_Serial0 & 0x000000FF);
    Speaker_StringSerial[4] = (u8)((Device_Serial0 & 0x0000FF00) >> 8);
    Speaker_StringSerial[6] = (u8)((Device_Serial0 & 0x00FF0000) >> 16);
    Speaker_StringSerial[8] = (u8)((Device_Serial0 & 0xFF000000) >> 24);

    Speaker_StringSerial[10] = (u8)(Device_Serial1 & 0x000000FF);
    Speaker_StringSerial[12] = (u8)((Device_Serial1 & 0x0000FF00) >> 8);
    Speaker_StringSerial[14] = (u8)((Device_Serial1 & 0x00FF0000) >> 16);
    Speaker_StringSerial[16] = (u8)((Device_Serial1 & 0xFF000000) >> 24);

    Speaker_StringSerial[18] = (u8)(Device_Serial2 & 0x000000FF);
    Speaker_StringSerial[20] = (u8)((Device_Serial2 & 0x0000FF00) >> 8);
    Speaker_StringSerial[22] = (u8)((Device_Serial2 & 0x00FF0000) >> 16);
    Speaker_StringSerial[24] = (u8)((Device_Serial2 & 0xFF000000) >> 24);
  }
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

 
//����USBʱ��,USBclk=48Mhz
void Set_USBClock(void)
{
 	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);//USBclk=PLLclk/1.5=48Mhz	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);	 //USBʱ��ʹ��				 
}

//����Usb��������
void usb_pullup_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

  /* Configure USB pull-up pin */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP  ;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

}

// Usb����һ����ʼ��
void usb_all_init(void)
{
	usb_pullup_init();
	USB_Cable_Config(DISABLE);
 	USB_Interrupts_Config();    
 	Set_USBClock(); 
}
