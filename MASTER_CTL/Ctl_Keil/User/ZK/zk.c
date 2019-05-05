#include "oled.h" 
//----------------- 端口定义  SPI----------------  					   
/* 采用IO口模拟SPI通讯方式驱动oled模块

 P4 口
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
unsigned char  FontBuf[128];//字库缓存	

void GBZK_ROMOUTSET(void)		//字库输出设置
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);		  	 

}

void GBZK_ROMOUTRESET(void)	 	//字库输出设置
{		
	GPIO_InitTypeDef GPIO_InitStructure;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 	//开漏输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);		  	 
}

void GBZK_GPIO_Config(void)	 //字库芯片初始化
{		
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 ; //	sdo cs
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_4);
}


/****送指令到字库IC***/
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
			SCLK_Clr();  //字库时钟拉低 
			SCLK_Set();  //字库时钟拉高
			}

}

/****从字库IC中取汉字或字符数据（1个字节）***/
unsigned char  get_data_from_ROM(void)
{
	unsigned char  i;
	unsigned char  ret_data=0; //返回数据初始化
	SCLK_Set();//字库时钟拉高
	GBZK_ROMOUTSET();  //下拉输入
	for(i=0;i<8;i++)
	{
		ZK_OUT_Set(); 
		SCLK_Clr();  //字库时钟拉低  
		ret_data=ret_data<<1;
		if( ZK_OUT() )
			{ret_data=ret_data+1; 
			}
		else
			{ret_data=ret_data+0; 
			}
		SCLK_Set(); //字库时钟拉高
	}
//	GBZK_ROMOUTRESET();	 //开漏输出
	return(ret_data); //返回读出的一个字节
}

/*从相关地址（AddrHigh：地址高字节,AddrMid：地址中字节,AddrLow：地址低字节）中连续读出DataLen个字节的数据到 pBuff的地址*/
/*连续读取*/
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen )
{
	unsigned char  i;
	ZK_CS_Clr(); //字库片选
	SCLK_Clr();  //字库时钟拉低
	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);//写地址高字节
	ZK_command(AddrMid);//写地址中字节
	ZK_command(AddrLow);//写地址低字节
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();//读一个字节数据
	ZK_CS_Set();//取消字库片选
}



 //*************************************************************************
 //****	Display_GB2312_String（）汉字字符串显示程序	***//
 //zk_num定义：汉字库的标示符 1:GB2312_12*12,2:GB2312_15*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x：列坐标 y：行坐标
 //text[]：要显示的汉字
 //**************************************************************************
void Display_GB2312_String(unsigned char zk_num,unsigned int  x,unsigned int  y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	
	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
	unsigned char  n= 0, d=0;// 不同点阵字库的计算变量
	
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
			 	 }//溢出换行

		if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
		{						
			/*国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址*/
		 	FontAddr = (text[i]- 0xA1)*94; 
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/ 
					
			x=x+d; //下一个字坐标
			i+=2;  //下个字符
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		{						
			/*国标简体（GB2312） 字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址*/
			 
			FontAddr = (text[i]- 0xB0)*94; 
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[ ]"*/
			Display_GB2312(zk_num,x,y);//显示一个汉字到OLED上/
	 		x=x+d; //下一个字坐标
			i+=2;  //下个字符

		}
		
	}
	
}

//***************************************************************
//  显示12*12 15*16 24*24 32*32 GB2312点阵汉字 2015-11晶奥测试通过
//   zk_num：汉字 库中的类型  GB_1212 1616 2424 3232 
//   x: Start Column  开始列 
//   y: Start Row   开始行 0~63 
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
	}

}





  //*************************************************************************
 //****	Display_GBasc_String（）国标扩展字符字符串显示程序	***//
 //zk_num定义： 字库的标示符 1: 6X12 点国标扩展字符,2:8X16 点国标扩展字符,3:8X16 点国标扩展特殊字符, 4:12X24 点国标扩展字符, 5:16X32 点国标扩展字符
 //x：列坐标 y：行坐标
 //text[]：要显示的字符
 //**************************************************************************
void Display_GBasc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned short  FontCode  ; // 字符内码
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
	unsigned char  n= 0, d=0;// 不同点阵字库的计算变量
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DBE0C; n=12 ;d=8;  break;	 //	   6X12 点国标扩展字符
		case '2':  BaseAdd=0x1DD790; n=16;d=8;  break;	 //8X16 点国标扩展字符
		case '3':  BaseAdd=0x1F2880; n=16;d=8;  break;	 //8X16 点国标扩展特殊字符
	    case '4':  BaseAdd=0x1DFF30; n=48; d=12; break;	 //12X24 点国标扩展字符
	    case '5':  BaseAdd=0x1E5A90; n=64;d=16; break;	 // 16X32 点国标扩展字符
			}
		
	while((text[i]>0x00))
	{	
	     if(x>(256-d))
	      {  y=y+d; 
		     x=0; 
			 	 }//溢出换行

 	     FontCode =	0xAA00|(text[i]|0x80);	//ascii最高位为1
		if((FontCode>=0xAAA1) &&(FontCode<=0xAAFE))
		{						
		    FontAddr = 	FontCode-0xAAA1;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*显示一个字符到OLED上 */
			i+=1;
			x+=d;//下一个字坐标
		 
		}
		else
		
		  if((FontCode>=0xABA1) &&(FontCode<=0xABC0))	 
		{						
		    FontAddr = 	(FontCode-0xABA1)+95;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_GBasc(zk_num,x,y);/*显示一个字符到OLED上 */
			i+=1;
			x+=d;//下一个字坐标
		 
		}
		 else
			
			if((FontCode>=0xACA1) &&(FontCode<=0xACDF))		// 8X16 点国标扩展特殊字符
		{						
		    FontAddr = 	FontCode-0xACA1;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位/
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位/
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"/
			Display_GBasc(zk_num,x,y);//显示一个字符到OLED上 /
			i+=1;
			x+=d;//下一个字坐标
		 
		}


	}
	
}
 
//***************************************************************
//  显示6*12 8*16 12*24 16*32 GB2312点阵ASCII 2015-11晶奥测试通过
//zk_num定义： 字库的标示符 1: 6X12 点国标扩展字符,2:8X16 点国标扩展字符,3:8X16 点国标扩展特殊字符, 4:12X24 点国标扩展字符, 5:16X32 点国标扩展字符
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
//*************************************************************** 
void Display_GBasc( unsigned char zk_num,unsigned int x, unsigned int y )
{
    //extern unsigned char  FontBuf[128];//字库缓存	
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	 
	for(j=0;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
	}

}
 







 //*************************************************************************
 //****	Display_Asc_String  ASCII字符串显示程序	***//
 //zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
 //x：列坐标 y：行坐标
 //text[]：要显示的ASCII
 //**************************************************************************
void Display_Asc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
    unsigned char  n= 0, d=0,c=0;// 不同点阵字库的计算变量
	
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
			 	 }//溢出换行


	   		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/

			Display_Asc(zk_num,x,y);/*显示一个ascii到OLED上 */
            i+=1;
			x+=d;//下一个字坐标
		 
		}

	}
	
}


//***************************************************************
//  显示 ASCII 2015-11晶奥测试通过
//zk_num定义：汉字库的标示符 1:5x7 ASCII,2:7x8 ASCII, 3:6x12 ASCII, 4:8x16 ASCII,	5: 12x24 ASCII,6:16x32 ASCII;
//   x: Start Column  开始列  
//   y: Start Row   开始行 0~63 
//*************************************************************** 
void Display_Asc( unsigned char zk_num,unsigned int x, unsigned int y )
{
    //extern unsigned char  FontBuf[128];//字库缓存	
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	
	if(k==0)
	{
		for(j=0;j<n-1;j++)
		{
			 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
			}
		 OLED_WR_Byte(0x00,OLED_DATA);	    //写前2列
		 OLED_WR_Byte(0x00,OLED_DATA);               //写后2列	  共计4列
	}
	else
	{
	 	for(j=0;j<n;j++)
		{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
			}

	}
}






















/*

 //---------------------------------------------------------------------
 // 	Display_GB2312_String（）汉字字符串显示程序	 
 //zk_num定义：汉字库的标示符 1:GB2312_12*12,2:GB2312_16*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x：列坐标 y：行坐标
 //text[]：要显示的汉字
 
void Display_Arial_String(unsigned char zk_num,unsigned char  x,unsigned char  y, unsigned char  text[])
{
   
	unsigned char  i= 2;
//	unsigned short  FontCode  ; // 字符内码
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
	unsigned char  n= 0, d=0;// 不同点阵字库的计算变量
	
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
			
			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位/
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位/
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"/

			Display_Asc(zk_num,x,y);//显示一个汉字到OLED上 /
			i+=1;
			x+=d;//下一个字坐标
			if(x>(255-d))
			   {x=0; y+=d;}//溢出换行
		}

	}
	
}











 //---------------------------------------------------------------------------------
 // 
 //zk_num定义：汉字库的标示符 1:GB2312_12*12,2:GB2312_16*16, 3:GB2312_24*24, 4:GB2312_32*32,
 //x：列坐标 y：行坐标
 //text[]：要显示的汉字
 // -----------------------------------------------------------------------------------
void Display_Arial_Asc_String(unsigned char zk_num,unsigned char  x,unsigned char  y, unsigned char  text[])
{
   
	unsigned char  i= 0;
	//	unsigned char  i= 0;
//	unsigned short  FontCode  ; // 字符内码
	unsigned char  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址	
    unsigned char  n= 0, d=0,c=0;// 不同点阵字库的计算变量
	
	switch(zk_num)
	{
		case '1':  BaseAdd=0x1DC400; n=26 ;d=8 ;c=8;  break;	 //	  12点阵不等宽ARIAL ASCII
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
			
			AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位 
			AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位 
			AddrLow = FontAddr&0xff;	     //地址的低8位,共24位 /
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]" 

			Display_Arial_Asc(zk_num,x,y);//显示一个汉字到OLED上  
            i+=1;
			x+=d;//下一个字坐标
			if(x>(255-d))
			   {x=0; y+=c;}//溢出换行
		}

	}
	
}


 
//-----------------------------------------------------------------------
//  显示6*12 8*16 12*24 16*32 GB2312点阵ASCII 2015-11晶奥测试通过
//  取模方式为：横向取模左高位,数据排列:从左到右从上到下   
//   zk_num：汉字库中的ASCII   GB ASCII_6X12 8X16 12X24 16X32 
//   x: Start Column  开始列 范围 0~（256-16）
//   y: Start Row   开始行 0~63 
//-------------------------------------------------------------------------------
void Display_Arial_Asc( unsigned char zk_num,unsigned char x, unsigned char y )
{
    //extern unsigned char  FontBuf[128];//字库缓存	
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
		
	Set_Column_Address(Shift+x1,Shift+x1+x0); // 设置列坐标，shift为列偏移量由1322决定。 
	Set_Row_Address(y,y+y0); //设置行坐标
	Set_Write_RAM();	 //	写显存
	 
	for(j=2;j<n;j++)
	{
		 Con_4_byte(FontBuf[j]);//数据转换，把数组数据送入SSD1322
	}

}


 

void ZK_map( unsigned char *getdate,unsigned char *putdata,unsigned char high,unsigned char width)
{
	unsigned char i,j,hbyte,wbyte;
	unsigned char i_8,j_8;
	unsigned char  temp[32]; 
	wbyte = (width+7)/8;
	hbyte = (high+7)/8;

	   	for( i = 0; i < high; i++ )			  //  竖置横排 转 横置横排
			for( j = 0; j < width; j++ )
			{
				i_8 = i/8;
				if((*(putdata+i_8*width+j)&(0x01<<(i%8))) > 0)
					temp[wbyte*i+j/8] |= (0x80>>(j%8));
				else
					temp[wbyte*i+j/8] &= (~(0x80>>(j%8)));
			   }


  		for( i = 0; i < high; i++ )		 //横置横排 转 横置竖排
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
