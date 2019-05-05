#include "oled.h" 
//----------------- �˿ڶ���  SPI----------------  					   
/* ����IO��ģ��SPIͨѶ��ʽ����oledģ��

 P4 ��
 SCLK  ~ PB13
SDIN ~ PB15
DC_OLED  ~ PB10
RES_OLED  ~ PB11
CS_OLED ~ PB12
SDO_ZK ~ PB14
CS_ZK ~ PB1

SCLK  ~ PC6
SDIN ~ PC7
DC_OLED  ~ PC8
RES_OLED  ~ PC9
CS_OLED ~ PC10
SDO_ZK ~ PC11
CS_ZK ~ PC12
*/
unsigned char  FontBuf[128];//�ֿ⻺��	

void GBZK_ROMOUTSET(void)		//�ֿ��������
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);		  	 

}

void GBZK_ROMOUTRESET(void)	 	//�ֿ��������
{		
	GPIO_InitTypeDef GPIO_InitStructure;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 	//��©��� 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);		  	 
}

void GBZK_GPIO_Config(void)	 //�ֿ�оƬ��ʼ��
{		
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 ; //	sdo cs
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_4);
}


/****��ָ��ֿ�IC***/
void ZK_command( unsigned char  data )
{
    unsigned char  i;
	 
	for(i=0;i<8;i++ )
	{
		if(data&0x80)
			{SDIN_Set(); 
			}
		else
			{SDIN_Clr(); 
			}
			data = data<<1;
			SCLK_Clr();  //�ֿ�ʱ������ 
			SCLK_Set();  //�ֿ�ʱ������
			}

}

/****���ֿ�IC��ȡ���ֻ��ַ����ݣ�1���ֽڣ�***/
unsigned char  get_data_from_ROM(void)
{
	unsigned char  i;
	unsigned char  ret_data=0; //�������ݳ�ʼ��
	SCLK_Set();//�ֿ�ʱ������
	GBZK_ROMOUTSET();  //��������
	for(i=0;i<8;i++)
	{
		ZK_OUT_Set(); 
		SCLK_Clr();  //�ֿ�ʱ������  
		ret_data=ret_data<<1;
		if( ZK_OUT() )
			{ret_data=ret_data+1; 
			}
		else
			{ret_data=ret_data+0; 
			}
		SCLK_Set(); //�ֿ�ʱ������
	}
//	GBZK_ROMOUTRESET();	 //��©���
	return(ret_data); //���ض�����һ���ֽ�
}

/*����ص�ַ��AddrHigh����ַ���ֽ�,AddrMid����ַ���ֽ�,AddrLow����ַ���ֽڣ�����������DataLen���ֽڵ����ݵ� pBuff�ĵ�ַ*/
/*������ȡ*/
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen )
{
	unsigned char  i;
	ZK_CS_Clr(); //�ֿ�Ƭѡ
	SCLK_Clr();  //�ֿ�ʱ������
	ZK_command(0x03);//дָ��
	ZK_command(AddrHigh);//д��ַ���ֽ�
	ZK_command(AddrMid);//д��ַ���ֽ�
	ZK_command(AddrLow);//д��ַ���ֽ�
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();//��һ���ֽ�����
	ZK_CS_Set();//ȡ���ֿ�Ƭѡ
}



 //*************************************************************************
 //****	Display_GB2312_String���������ַ�����ʾ����	***//
 //zk_num���壺���ֿ�ı�ʾ�� 1:GB2312_12*12,2:GB2312_15*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x�������� y��������
 //text[]��Ҫ��ʾ�ĺ���
 //**************************************************************************
void Display_GB2312_String(unsigned char zk_num,unsigned int  x,unsigned int  y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ
	
	unsigned long  FontAddr=0; //�ֵ�ַ
	unsigned long  BaseAdd=0; //�ֿ����ַ	
	unsigned char  n= 0, d=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x0; n=24 ;d=12;  break;
		case '2':  BaseAdd=0x2C9D0; n=32;d=16;  break;
	    case '3':  BaseAdd=0x68190; n=72; d=24; break;
	    case '4':  BaseAdd=0xEDF00; n=128;d=32; break;
			}
		
	while((text[i]>0x00))
	{
	    if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//�������

		if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
		{						
			/*������壨GB2312�������� �ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; ������ȡ��ַ*/
		 	FontAddr = (text[i]- 0xA1)*94; 
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GB2312(zk_num,x,y);//��ʾһ�����ֵ�OLED��/ 
					
			x=x+d; //��һ��������
			i+=2;  //�¸��ַ�
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		{						
			/*������壨GB2312�� �ֿ�IC�еĵ�ַ�����¹�ʽ�����㣺*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; ������ȡ��ַ*/
			 
			FontAddr = (text[i]- 0xB0)*94; 
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[ ]"*/
			Display_GB2312(zk_num,x,y);//��ʾһ�����ֵ�OLED��/
	 		x=x+d; //��һ��������
			i+=2;  //�¸��ַ�

		}
		
	}
	
}

//***************************************************************
//  ��ʾ12*12 15*16 24*24 32*32 GB2312������ 2015-11���²���ͨ��
//   zk_num������ ���е�����  GB_1212 1616 2424 3232 
//   x: Start Column  ��ʼ�� 
//   y: Start Row   ��ʼ�� 0~63 
//*************************************************************** 
void Display_GB2312( unsigned char zk_num,unsigned int x, unsigned int y )
{
   	unsigned char j ,x0,y0,n;
	unsigned int x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=3;y0=11; n=24;  break;
		case '2':  x0=3;y0=15; n=32;  break;
	    case '3':  x0=5;y0=23; n=72;  break;
	    case '4':  x0=7;y0=31; n=128;  break;
			}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
	}

}





  //*************************************************************************
 //****	Display_GBasc_String����������չ�ַ��ַ�����ʾ����	***//
 //zk_num���壺 �ֿ�ı�ʾ�� 1: 6X12 �������չ�ַ�,2:8X16 �������չ�ַ�,3:8X16 �������չ�����ַ�, 4:12X24 �������չ�ַ�, 5:16X32 �������չ�ַ�
 //x�������� y��������
 //text[]��Ҫ��ʾ���ַ�
 //**************************************************************************
void Display_GBasc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned short  FontCode  ; // �ַ�����
	unsigned char  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	unsigned long  FontAddr=0; //�ֵ�ַ
	unsigned long  BaseAdd=0; //�ֿ����ַ	
	unsigned char  n= 0, d=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DBE0C; n=12 ;d=8;  break;	 //	   6X12 �������չ�ַ�
		case '2':  BaseAdd=0x1DD790; n=16;d=8;  break;	 //8X16 �������չ�ַ�
		case '3':  BaseAdd=0x1F2880; n=16;d=8;  break;	 //8X16 �������չ�����ַ�
	    case '4':  BaseAdd=0x1DFF30; n=48; d=12; break;	 //12X24 �������չ�ַ�
	    case '5':  BaseAdd=0x1E5A90; n=64;d=16; break;	 // 16X32 �������չ�ַ�
			}
		
	while((text[i]>0x00))
	{	
	     if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//�������

 	     FontCode =	0xAA00|(text[i]|0x80);	//ascii���λΪ1
		if((FontCode>=0xAAA1) &&(FontCode<=0xAAFE))
		{						
		    FontAddr = 	FontCode-0xAAA1;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*��ʾһ���ַ���OLED�� */
			i+=1;
			x+=d;//��һ��������
		 
		}
		else
		
		  if((FontCode>=0xABA1) &&(FontCode<=0xABC0))	 
		{						
		    FontAddr = 	(FontCode-0xABA1)+95;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*��ʾһ���ַ���OLED�� */
			i+=1;
			x+=d;//��һ��������
		 
		}
		 else
			
			if((FontCode>=0xACA1) &&(FontCode<=0xACDF))		// 8X16 �������չ�����ַ�
		{						
		    FontAddr = 	FontCode-0xACA1;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  //��ַ�ĸ�8λ,��24λ/
			AddrMid = (FontAddr&0xff00)>>8;      //��ַ����8λ,��24λ/
			AddrLow = FontAddr&0xff;	     //��ַ�ĵ�8λ,��24λ/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//ȡһ�����ֵ����ݣ��浽"FontBuf[]"/
			Display_GBasc(zk_num,x,y);//��ʾһ���ַ���OLED�� /
			i+=1;
			x+=d;//��һ��������
		 
		}


	}
	
}
 
//***************************************************************
//  ��ʾ6*12 8*16 12*24 16*32 GB2312����ASCII 2015-11���²���ͨ��
//zk_num���壺 �ֿ�ı�ʾ�� 1: 6X12 �������չ�ַ�,2:8X16 �������չ�ַ�,3:8X16 �������չ�����ַ�, 4:12X24 �������չ�ַ�, 5:16X32 �������չ�ַ�
//   x: Start Column  ��ʼ�� ��Χ 0~��256-16��
//   y: Start Row   ��ʼ�� 0~63 
//*************************************************************** 
void Display_GBasc( unsigned char zk_num,unsigned int x, unsigned int y )
{
    //extern unsigned char  FontBuf[128];//�ֿ⻺��	
	unsigned char j ,x0,y0,n;
	unsigned int  x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=1;y0=11; n=12;  break;
		case '2':  x0=1;y0=15; n=16;  break;
		case '3':  x0=1;y0=15; n=16;  break;
	    case '4':  x0=3;y0=23; n=48;  break;
	    case '5':  x0=3;y0=31; n=64;  break;
			}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
	}

}
 







 //*************************************************************************
 //****	Display_Asc_String  ASCII�ַ�����ʾ����	***//
 //zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
 //x�������� y��������
 //text[]��Ҫ��ʾ��ASCII
 //**************************************************************************
void Display_Asc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	unsigned long  FontAddr=0; //�ֵ�ַ
	unsigned long  BaseAdd=0; //�ֿ����ַ	
    unsigned char  n= 0, d=0,c=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DDF80; n=8 ;d=8 ;c=8;  break;	 //	  5x7 ASCII
		case '2':  BaseAdd=0x1DE280; n=8;d=8; c=8;  break;	 //   7x8 ASCII
	    case '3':  BaseAdd=0x1DBE00; n=12; d=8;c=12; break;	 //  6x12 ASCII
	    case '4':  BaseAdd=0x1DD780; n=16;d=8;c=16; break;	 //  8x16 ASCII
		case '5':  BaseAdd=0x1DFF00; n=48;d=12; c=24;break;	 //  12x24 ASCII
		case '6':  BaseAdd=0x1E5A50; n=64;d=16;c=32; break;	 //  16x32 ASCII

			}
		
	while((text[i]>0x00))
	{	
		 if(x>(256-d))
	      {  y=y+c; 
		     x=0; 
			 	 }//�������


	   		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*��ַ�ĸ�8λ,��24λ*/
			AddrMid = (FontAddr&0xff00)>>8;      /*��ַ����8λ,��24λ*/
			AddrLow = FontAddr&0xff;	     /*��ַ�ĵ�8λ,��24λ*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*ȡһ�����ֵ����ݣ��浽"FontBuf[]"*/

			Display_Asc(zk_num,x,y);/*��ʾһ��ascii��OLED�� */
            i+=1;
			x+=d;//��һ��������
		 
		}

	}
	
}


//***************************************************************
//  ��ʾ ASCII 2015-11���²���ͨ��
//zk_num���壺���ֿ�ı�ʾ�� 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  ��ʼ��  
//   y: Start Row   ��ʼ�� 0~63 
//*************************************************************** 
void Display_Asc( unsigned char zk_num,unsigned int x, unsigned int y )
{
    //extern unsigned char  FontBuf[128];//�ֿ⻺��	
    unsigned char j ,x0,y0,n,k;
	unsigned int x1=x/4; 
		
	switch(zk_num)
	{
	    case '1':  x0=1;y0=7; n=8; k=0; break;   //	  5x7 ASCII
		case '2':  x0=1;y0=7; n=8; k=1; break;   //	  7x8 ASCII
	    case '3':  x0=1;y0=11; n=12;k=1;  break;   //  6x12 ASCII
	    case '4':  x0=1;y0=15; n=16; k=1;  break;   //  8x16 ASCII
	    case '5':  x0=3;y0=23; n=48; k=1;  break;   //  12x24 ASCII
	    case '6':  x0=3;y0=31; n=64; k=1;  break;   //  16x32 ASCII
			}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	
	if(k==0)
	{
		for(j=0;j<n-1;j++)
		{
			 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
			}
		 OLED_WR_Byte(0x00,OLED_DATA);	    //дǰ2��
		 OLED_WR_Byte(0x00,OLED_DATA);               //д��2��	  ����4��
	}
	else
	{
	 	for(j=0;j<n;j++)
		{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
			}

	}
}






















/*

 //---------------------------------------------------------------------
 // 	Display_GB2312_String���������ַ�����ʾ����	 
 //zk_num���壺���ֿ�ı�ʾ�� 1:GB2312_12*12,2:GB2312_16*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x�������� y��������
 //text[]��Ҫ��ʾ�ĺ���
 
void Display_Arial_String(unsigned char zk_num,unsigned char  x,unsigned char  y, unsigned char  text[])
{
   
	unsigned char  i= 2;
//	unsigned short  FontCode  ; // �ַ�����
	unsigned char  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	unsigned long  FontAddr=0; //�ֵ�ַ
	unsigned long  BaseAdd=0; //�ֿ����ַ	
	unsigned char  n= 0, d=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DC400; n=26 ;d=16 ;  break;	 //	  16x12 ASCII
		case '2':  BaseAdd=0x1DE280; n=8;d=7;  break;	 //   7x8 ASCII
	    case '3':  BaseAdd=0x1DBE00; n=12; d=6; break;	 //  6x12 ASCII
	    case '4':  BaseAdd=0x1DD780; n=16;d=8; break;	 //  8x16 ASCII
		case '5':  BaseAdd=0x1DFF00; n=48;d=12; break;	 //  12x24 ASCII
		case '6':  BaseAdd=0x1E5A50; n=64;d=16; break;	 //  16x32 ASCII

			}
		
	while((text[i]>0x00))
	{	
	   		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		 
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  //��ַ�ĸ�8λ,��24λ/
			AddrMid = (FontAddr&0xff00)>>8;      //��ַ����8λ,��24λ/
			AddrLow = FontAddr&0xff;	     //��ַ�ĵ�8λ,��24λ/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//ȡһ�����ֵ����ݣ��浽"FontBuf[]"/

			Display_Asc(zk_num,x,y);//��ʾһ�����ֵ�OLED�� /
			i+=1;
			x+=d;//��һ��������
			if(x>(255-d))
			   {x=0; y+=d;}//�������
		}

	}
	
}











 //---------------------------------------------------------------------------------
 // 
 //zk_num���壺���ֿ�ı�ʾ�� 1:GB2312_12*12,2:GB2312_16*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x�������� y��������
 //text[]��Ҫ��ʾ�ĺ���
 // -----------------------------------------------------------------------------------
void Display_Arial_Asc_String(unsigned char zk_num,unsigned char  x,unsigned char  y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	//	unsigned char  i= 0;
//	unsigned short  FontCode  ; // �ַ�����
	unsigned char  AddrHigh,AddrMid,AddrLow ; //�ָߡ��С��͵�ַ

	unsigned long  FontAddr=0; //�ֵ�ַ
	unsigned long  BaseAdd=0; //�ֿ����ַ	
    unsigned char  n= 0, d=0,c=0;// ��ͬ�����ֿ�ļ������
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DC400; n=26 ;d=8 ;c=8;  break;	 //	  12���󲻵ȿ�ARIAL ASCII
		case '2':  BaseAdd=0x1DE280; n=8;d=8; c=8;  break;	 //   7x8 ASCII
	    case '3':  BaseAdd=0x1DBE00; n=12; d=8;c=12; break;	 //  6x12 ASCII
	    case '4':  BaseAdd=0x1DD780; n=16;d=8;c=16; break;	 //  8x16 ASCII
		case '5':  BaseAdd=0x1DFF00; n=48;d=12; c=24;break;	 //  12x24 ASCII
		case '6':  BaseAdd=0x1E5A50; n=64;d=16;c=32; break;	 //  16x32 ASCII

			}
		
	while((text[i]>0x00))
	{	
	   		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		 
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  //��ַ�ĸ�8λ,��24λ 
			AddrMid = (FontAddr&0xff00)>>8;      //��ַ����8λ,��24λ 
			AddrLow = FontAddr&0xff;	     //��ַ�ĵ�8λ,��24λ /
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//ȡһ�����ֵ����ݣ��浽"FontBuf[]" 

			Display_Arial_Asc(zk_num,x,y);//��ʾһ�����ֵ�OLED��  
            i+=1;
			x+=d;//��һ��������
			if(x>(255-d))
			   {x=0; y+=c;}//�������
		}

	}
	
}


 
//-----------------------------------------------------------------------
//  ��ʾ6*12 8*16 12*24 16*32 GB2312����ASCII 2015-11���²���ͨ��
//  ȡģ��ʽΪ������ȡģ���λ,��������:�����Ҵ��ϵ���   
//   zk_num�����ֿ��е�ASCII   GB ASCII_6X12 8X16 12X24 16X32 
//   x: Start Column  ��ʼ�� ��Χ 0~��256-16��
//   y: Start Row   ��ʼ�� 0~63 
//-------------------------------------------------------------------------------
void Display_Arial_Asc( unsigned char zk_num,unsigned char x, unsigned char y )
{
    //extern unsigned char  FontBuf[128];//�ֿ⻺��	
	unsigned char x1,j ,x0,y0,n;
	x1=x/4; 
		
	switch(zk_num)
	{
		case '1':  x0=FontBuf[1]/4;y0=12; n=26;  break;   //	  12x12 ASCII
		case '2':  x0=1;y0=7; n=8;  break;   //	  7x8 ASCII
	    case '3':  x0=1;y0=11; n=12;  break;   //  6x12 ASCII
	    case '4':  x0=1;y0=15; n=16;  break;   //  8x16 ASCII
	    case '5':  x0=3;y0=23; n=48;  break;   //  12x24 ASCII
	    case '6':  x0=3;y0=31; n=64;  break;   //  16x32 ASCII
			}
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // ���������꣬shiftΪ��ƫ������1322������ 
	Set_Row_Address(y,y+y0); //����������
	Set_Write_RAM();	 //	д�Դ�
	 
	for(j=2;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//����ת������������������SSD1322
	}

}


 

void ZK_map( unsigned char *getdate,unsigned char *putdata,unsigned char high,unsigned char width)
{
	unsigned char i,j,hbyte,wbyte;
	unsigned char i_8,j_8;
	unsigned char  temp[32]; 
	wbyte = (width+7)/8;
	hbyte = (high+7)/8;

	   	for( i = 0; i < high; i++ )			  //  ���ú��� ת ���ú���
			for( j = 0; j < width; j++ )
			{
				i_8 = i/8;
				if((*(putdata+i_8*width+j)&(0x01<<(i%8))) > 0)
					temp[wbyte*i+j/8] |= (0x80>>(j%8));
				else
					temp[wbyte*i+j/8] &= (~(0x80>>(j%8)));
			   }


  		for( i = 0; i < high; i++ )		 //���ú��� ת ��������
			for( j = 0; j < width; j++ )
			{
				j_8 = j/8;
				if((*(temp+wbyte*i+j/8)&(0x80>>(j%8))) > 0)
					getdate[j_8*high+i] |= (0x80>>(j%8));
				else
					getdate[j_8*high+i] &= (~(0x80>>(j%8)));
			}

 }

  */
/************************END OF FILE**********************/
