/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Endpoint routines
*******************************************************************************/
#include "usb_lib.h"
#include "usb_istr.h"
#include "i2splayer.h"//output program
 
u32 temp_buf[(ENDP1_Size/4)];
u8 fb_buf[3];

u16 rx_bytes_count;
u16 rx_frames_count;
u32 overrun_counter=0;


void fb(u32 freq)
{
u32 fbvalue, nInt,nFrac;

	// example freq:44100
	// 10.10fb=  44 << 14 + 100 << 4
	
	nInt=freq/1000;
	nFrac=(freq - nInt*1000);
		
	fbvalue=(nInt <<14) | (nFrac<<4);	
	
	fb_buf[0]=fbvalue & 0xff;
	fb_buf[1]=(fbvalue>>8) & 0xff;
	fb_buf[2]=(fbvalue>>16) & 0xff;

//{fb_buf[0]=0x66;fb_buf[1]=0x06;fb_buf[2]=0x0b;}
}



// EP OUT 0x01
void EP1_OUT_Callback(void)
{

u16 i;
u16 nextbuf;
	
//ͬ���˵���ǿ��ʹ��˫����ģ�û��ѡ��
	if (GetENDPOINT(ENDP1) & EP_DTOG_TX)
	{
		rx_bytes_count = GetEPDblBuf0Count(ENDP1);
		PMAToUserBufferCopy((u8 *)&temp_buf, ENDP1_BUF0Addr, rx_bytes_count); 
	}else
	{
		rx_bytes_count = GetEPDblBuf1Count(ENDP1);
		PMAToUserBufferCopy((u8 *)&temp_buf, ENDP1_BUF1Addr, rx_bytes_count);
	}
	FreeUserBuffer(ENDP1, EP_DBUF_OUT);

    rx_frames_count	=	rx_bytes_count / 4; //32λһ֡��16λÿ���� X 2����-> 4 bytes

//temp�����λ��濽������
	for (i=0; i<rx_frames_count; i++ )   //CPU�Ű���һ����һ���ֵĿ�
	{
	//���㼴��д���λ��
		if (Write_ptr < i2s_BUFSIZE ) nextbuf=Write_ptr+1 ;
		else nextbuf=0;
	   
		if (nextbuf != Play_ptr ) //���û��׷β������һ֡����д�뵽���λ���
		{
			Write_ptr = nextbuf;
			i2s_buf[Write_ptr] = temp_buf[i];

		}
		else //ײ����
		{
			if (audiostatus==0) DAC_PLAY();

			overrun_counter++;//��OVER-RUN FLAG
			break;//�����ˣ�ʣ������ȫ����Ҫ��
		}
	}




}



// EP IN 0x82
void EP2_IN_Callback (void)
{


}



// SOF
void SOF_Callback (void)
{

u16 data_remain;

//���㻺��������
	if (Write_ptr > Play_ptr) data_remain= Write_ptr - Play_ptr;
			
	else data_remain= i2s_BUFSIZE - Play_ptr + Write_ptr;

//�������ﵽһ�룬��������
	if ( (audiostatus==0) && (data_remain > i2s_BUFSIZE/2 ) ) DAC_PLAY();

//���㷴������

	if (data_remain > i2s_BUFSIZE/3*2 ) fb(44060);

	else if (data_remain > i2s_BUFSIZE/3*1 ) fb(44100);

	else fb(44140);

//ͬ���˵���ǿ��ʹ��˫�����
	if (GetENDPOINT(ENDP2) & EP_DTOG_RX)
	{
		UserToPMABufferCopy(fb_buf, ENDP2_TXADDR, 3);
		SetEPDblBuf0Count(ENDP2, EP_DBUF_IN, 3); 
	}else
	{
		UserToPMABufferCopy(fb_buf, ENDP2_TX2ADDR, 3);
		SetEPDblBuf1Count(ENDP2, EP_DBUF_IN, 3);
	}
		FreeUserBuffer(ENDP2, EP_DBUF_IN);
		SetEPTxValid(ENDP2);

}

