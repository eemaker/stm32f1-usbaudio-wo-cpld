#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_dma.h"
#include "i2splayer.h"

u8 audiostatus=0;							//0,��ͣ����;1,�������� 
u16 Play_ptr=0;							//�������ŵ���Ƶ֡������
u16 Write_ptr=0;							//��ǰ���浽����Ƶ������ 
								
u32 i2s_buf[(i2s_BUFSIZE+1)]; 					//��Ƶ����:16bit x 2ch -> 32bit DWORD,1024 samples

u32 underrun_counter=0;

u32 const DMAsetting= (0 << 14) | // �Ǵ洢�����洢��ģʽ
            (2 << 12) | // ͨ�����ȼ���
            (0 << 11) | // �洢�����ݿ��16bit
            (1 << 10) | // �洢�����ݿ��16bit
            (0 <<  9) | // �������ݿ��16bit
            (1 <<  8) | // �������ݿ��16bit
            (1 <<  7) | // �洢����ַ����ģʽ
            (0 <<  6) | // �����ַ����ģʽ(����)
            (0 <<  5) | // ��ѭ��ģʽ
            (1 <<  4) | // �Ӵ洢����
            (1 <<  3) | // ����������ж�(������)
            (0 <<  2) | // ����봫���ж�  (������)
            (1 <<  1) | // ����������ж�
            (0);        // ͨ������		   (�Ȳ�����)


void i2s2_dma(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;


  /* DMA1  */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  /* DMA1 Channel5 (triggered by SPI2 Tx event) Config */
	DMA1_Channel5->CCR = 0 ;	//�ȹر�DMA������д�룡
    DMA1_Channel5->CPAR = (u32) &SPI2->DR; //�����ַ
    DMA1_Channel5->CMAR = (u32) &i2s_buf[0]; //BUF�����ַ
    DMA1_Channel5->CNDTR = 2 ; //���䳤��
    DMA1_Channel5->CCR = DMAsetting | (0); // ͨ������(�Ȳ�����)
 

  /* Enable DMA channel5 IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}




void i2s2_initspi2(void)
{
  I2S_InitTypeDef I2S_InitStructure;

//��ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//��λ
  SPI_I2S_DeInit(SPI2);
//����
  I2S_StructInit( &I2S_InitStructure );   //��ʼ��
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;			//����� MSB;//�Ҷ��� LSB;//��׼IIS Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;	//MCLK���Ӱ��ʱ�Ӿ���
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_44k;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;					//������ LRCK�ߵ�ƽ
  I2S_Init(SPI2, &I2S_InitStructure);


SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);//���ͻ�����DMAʹ�� 


}

void I2S_2_gpio(void)
{   
  GPIO_InitTypeDef GPIO_InitStructure;

//����������� IO pins����Ϊû�ȴ�����Ļ��������ȷ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
}                                                                                                                                                 




void DAC_PLAY(void)
{

//���� DMA ��ȡ��ڵ�ַ												   
    DMA1_Channel5->CMAR = (u32) &(i2s_buf[Play_ptr]);
//���䳤����Ҫ����д��
    DMA1_Channel5->CNDTR = 2 ;
		
//����DMA
    DMA1_Channel5->CCR = DMAsetting | (1);// ͨ������ (��������)

//��λ����������iisģ�飬������ҷ�ת

	i2s2_initspi2();  //��Щ�Ǹ�λ���������д������
//����iisģ��
	I2S_Cmd(SPI2, ENABLE);

	audiostatus=1;
	GPIO_SetBits(GPIOA, GPIO_Pin_0); //��������ָʾ��

}

__inline void stopi2s2(void)
{
	I2S_Cmd(SPI2, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);//��λ��Ҫ���������������
//�ر�DMA�����ı�����
    DMA1_Channel5->CCR = DMAsetting; // �ر�DMA
		
	audiostatus=0;	
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);//Ϩ�𲥷�ָʾ

}


void DAC_STOP(void)	
{
   stopi2s2();
}


void DAC_INIT(void)
{
		i2s2_dma();//����DMA��ʼ������λ����������д
		I2S_2_gpio();//gpio��ʼ��������Ҫ������һ�μ���
		stopi2s2();//��ʼ��Ϊֹͣ״̬���������һ������������
					//��ʼ��Ϊֹͣ״̬ʱ����Ҫ��ʼ��i2sģ��
					//��Ȼû���������ȥ�����������ͷ���
					//��ÿ�ο�ʼ����ʱ��Ҫ��λ����ʼ��i2sģ��
}


void DMA1_Channel5_IRQHandler()
{
u16 next_Playptr;
//����DMA�����жϱ�־
    DMA_ClearITPendingBit(DMA1_IT_GL5);
//�ȹر�DMA����д��
	DMA1_Channel5->CCR = DMAsetting;        // ͨ���ر�;	


	//����һ������֡
	if (Play_ptr < i2s_BUFSIZE ) next_Playptr = Play_ptr + 1;

	else  next_Playptr = 0;    //ѭ����ͷ��

    
	if( next_Playptr == Write_ptr)	//ײβ�� 
	{
		stopi2s2();
		underrun_counter++;
		return;
	}else
	{
		Play_ptr = next_Playptr;//���Կ�������ȥ��һ������֡
	}


	//���� DMA ��ȡ��ڵ�ַ												   
    DMA1_Channel5->CMAR = (u32) &(i2s_buf[Play_ptr]);

	//���䳤����Ҫ����д��
    DMA1_Channel5->CNDTR = 2 ;
	
//����DMA
    DMA1_Channel5->CCR = DMAsetting|(1); //ͨ������


}
