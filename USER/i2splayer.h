
extern u8 audiostatus;							//bit0:0,��ͣ����;1,�������� 


extern u16 Play_ptr;							//�������ŵ���Ƶ֡������
extern u16 Write_ptr;							//��ǰ���浽����Ƶ������ 

#define i2s_BUFSIZE	2048							
extern u32 i2s_buf[(i2s_BUFSIZE+1)]; 					//��Ƶ����:16bit x 2ch -> 32bit DWORD samples

extern void DAC_INIT(void);
extern void DAC_PLAY(void);
extern void DAC_STOP(void);
extern void DAC_SHUTDOWN(void);
