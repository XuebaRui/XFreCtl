#include "stm32f10x.h"
#include "my_usart1.h"
#include "delay.h"
#include "bsp_zn200.h"
#include "string.h"
#include "oled.h"
#include "mytypedef.h"
#include "HMC832.h"
#include "func.h"
#include "ioport.h"
#include "bmp.h"
#include "key.h"
#include "adc.h"
#include "myflash.h"
#define Tab_CG   0x01
#define Tab_ATT  0x02
#define Tab_AGC  0x03
#define Tab_CF   0x04 
extern u8 Que_Len ;            
extern u8 **Parse_Que ;        //测试应该更改为数组缓存     
extern u8 paser(char *s,char *rtn_cmd ,char *rtn_data);
extern u8 Spi_RecBuff[10];
Sys_Para cur_SysPara;
void SystemClock_Config(void)
{
	RCC_DeInit(); 
	RCC_HSICmd(ENABLE);          													//打开内部时钟
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET); 	//等待内部时钟就绪
	RCC_HCLKConfig(RCC_SYSCLK_Div1);   										//PHB=系统时钟
  RCC_PCLK2Config(RCC_HCLK_Div1);      									//APB2=PHB
	RCC_PCLK1Config(RCC_HCLK_Div2);     									//APB1=PHB/2
	FLASH_SetLatency(FLASH_Latency_2);     								//FLASH CTL
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16); //锁相环倍频 8MHZ/2 * 14 = 64MHZ
	RCC_PLLCmd(ENABLE);																	
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);		//等待锁相环就绪
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);						//锁相环时钟作为系统时钟
	while(RCC_GetSYSCLKSource()!=0x08);										//确认锁相环为系统时钟
}
void IWDG_Configuration(void) //ns 看门狗
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
    IWDG_SetPrescaler(IWDG_Prescaler_256); 
    IWDG_SetReload(0x190); //0x190*256/40k=2.56s 
    IWDG_ReloadCounter();
    IWDG_Enable(); 
}
int main(void)
{
	u8 i = 0;
	signed char cur_AGCVal1 = 0;
	signed char cur_AGCVal2 = 0;
	signed char old_AGCVal1 = 51;
	u8 agc_count1 = 0;
	u8 agc_count2 = 0;
	u8 cur_agcvval = 0;
	signed char CG_Sel = 0 , old_CG_Sel = 0;        //CG设置
	signed char ATT_Sel = 0 , old_ATT_Sel = 0;      //ATT设置
	signed char AGC_Sel = 0;  
	signed char CF_Sel = 0 , old_CF_Sel = 0;        //CF各位设置
	signed char Tab = 0, old_Tab = 0;               //选择参数
	u8 DisBuffer[32] = {0};
	Sys_Para sys_cmd;
	double tmp_double = 0;
	u32 tmp_int = 0;
	char cmd[10] = {0}; 												//命令
	char cmd_data[10] = {0};  									//命令内容
	u8 key_flag = 0;                            
	u8 key_delay = 0;
	u32 key_cont = 0;
	enum{
		Main_Page,
		Addr_Page,
		Agccal_Page,
	}Page,oldPage;
	#if DeBug
	volatile RCC_ClocksTypeDef get_rcc_clock;
	#endif
	memset(&sys_cmd,0,sizeof(sys_cmd));
	memset(&cur_SysPara,0,sizeof(cur_SysPara));
//	cur_SysPara.rem = LOCAL;
//	cur_SysPara.cg = 50;
//	cur_SysPara.cf = 1698.0;
	Que_Len = 0;
	/***********************BSP************************/	
	SystemClock_Config();												//系统时钟 内部 倍频 48mhz
	BSP_NVIC_Configuration(); 									//中断向量组
	Delay_TIM4_Init();        									//延时初始化
	Key_IOInit();
  OLED_Init();																//OLED初始化 
	GBZK_GPIO_Config(); 												//OLED字库IO初始化
	#if PowON_Pic
	Show_Pattern(gImage_AV,20,((92 + 80)/4 - 1),11,52);
//sprintf(DisBuffer,"Amplifier Electronics");
//Display_Asc_String('1',100,31,DisBuffer);
	#endif
	Fill_RAM(0x00);
	delay_ms(50);
	//OLED_WR_Byte(0xAE,OLED_CMD); //	Display Off
	OLED_WR_Byte(0xAF,OLED_CMD);
	#if DeBug
		RCC_GetClocksFreq(&get_rcc_clock);   			//系统工作时钟
		sprintf(DisBuffer,"DeBug...");
		Display_Asc_String('1',20,0,DisBuffer);
		sprintf(DisBuffer,"Frequency: %d MHz",get_rcc_clock.SYSCLK_Frequency/1000000);
		Display_Asc_String('1',20,8,DisBuffer);
	#endif
	Usart_TIM2_Init();													//定时器2做数据传输完成判断
	#if DeBug
		sprintf(DisBuffer,"Usart_Init...");
		Display_Asc_String('1',20,16,DisBuffer);
		delay_ms(50);
	#endif
	ZN200_Init();				 												//以太网初始化
	IOPort_Init();
	adc_init();                                 //ADC初始化
	#if DeBug
		sprintf(DisBuffer,"IO_Init...");
		Display_Asc_String('1',20,24,DisBuffer);
		delay_ms(50);
	#endif
	LowPW_Init(); 															//低电量中断 电容不够保持足够的时间
	#if DeBug
		sprintf(DisBuffer,"LowPowerDet_Init...");
		Display_Asc_String('1',20,32,DisBuffer);
	#endif
	HMC832A_Init();
	HMC832B_Init();
	#if DeBug
		sprintf(DisBuffer,"FreInit...");
		Display_Asc_String('1',20,40,DisBuffer);
	#endif
	//cur_SysPara = Load_SysPara();               //加载上次关机的参数
	#if DeBug
		sprintf(DisBuffer,"Load_SysPara...");
		Display_Asc_String('1',20,48,DisBuffer);
	#endif
	IWDG_Configuration();												//看门狗初始化
	IWDG->KR=0XAAAA; 
	#if DeBug
		sprintf(DisBuffer,"OPEN_IWDG...FINISH...");
		Display_Asc_String('1',20,56,DisBuffer);
//		Vertical_Scroll(1,1,1000);
//		sprintf(DisBuffer,"FINISH...");
//		Display_Asc_String('1',10,72,DisBuffer);		
	#endif
	Fill_RAM(0x00);
	delay_ms(50);
	
	sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
	Display_Asc_String('1',20,4,DisBuffer);
	sprintf(DisBuffer,"ATT: %01d%01d dB",cur_SysPara.att / 10 , cur_SysPara.att % 10);
	Display_Asc_String('1',20,18,DisBuffer);
	if(cur_SysPara.bw == BW_180Mhz)	
		sprintf(DisBuffer,"BW: %s","180MHZ");
	else if(cur_SysPara.bw == BW_250Mhz)
		sprintf(DisBuffer,"BW: %s","250MHZ");
	else 
		sprintf(DisBuffer,"BW: %s","500MHZ");
	Display_Asc_String('1',20,32,DisBuffer);
	sprintf(DisBuffer,"AGC: %s",(cur_SysPara.agc == AGC)?"AGC":"MGC" );
	Display_Asc_String('1',20,32,DisBuffer);
	sprintf(DisBuffer,"CF : %01d%01d%01d%01d.%01d%01d%01d MHz",(u32)((cur_SysPara.cf+0.0005) * 10000)/10000000,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000000/1000000,
																															(u32)((cur_SysPara.cf+0.0005) * 10000)%1000000/100000,(u32)((cur_SysPara.cf+0.0005) * 10000)%100000/10000			
																															,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000/1000 ,(u32)((cur_SysPara.cf+0.0005) * 10000)%1000/100,(u32)((cur_SysPara.cf+0.0005) * 10000)%100/10);
	Display_Asc_String('1',20,46,DisBuffer);
	sprintf(DisBuffer,"%s",(cur_SysPara.rem == LOCAL)?"LOCAL ":"REMOTE");
	Display_Asc_String('1',200,56,DisBuffer);
	//增益设置 SPI 发送给从机
	CGA_Set(50-cur_SysPara.cg);
	CGB_Set(50-cur_SysPara.cg);
	ATTA_Set(cur_SysPara.att);
	ATTB_Set(cur_SysPara.att);
	if(HMC832B_FreSet(cur_SysPara.cf) && HMC832A_FreSet(cur_SysPara.cf)) //设置频率成功
	{					
	}
	Page = Main_Page;	
	oldPage = Main_Page;
	//Fill_Block(0xff,15,15,26,26);
	//Fill_Block(0xff,17,17,26,26);
	//Fill_Block(0xff,15,19,40,40);
	//Fill_Block(0xff,15,15,54,54);
	//Fill_Block(0xff,17,17,54,54);
	//Fill_Block(0xff,19,19,54,54);
	//Fill_Block(0xff,21,21,54,54);
	//Fill_Block(0xff,25,25,54,54);
	//Fill_Block(0xff,27,27,54,54);
	//Fill_Block(0xff,29,29,54,54);
  while(1)
	{
		if(Page == Main_Page)                           //主界面
		{
			if(Page != oldPage)                           //从其他界面跳转到main 需要更新界面
			{
				Fill_RAM(0x00);
				delay_ms(50);
				sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
				Display_Asc_String('1',20,4,DisBuffer);
				sprintf(DisBuffer,"ATT: %01d%01d dB",cur_SysPara.att / 10 , cur_SysPara.att % 10);
				Display_Asc_String('1',20,18,DisBuffer);
				sprintf(DisBuffer,"AGC: %s",(cur_SysPara.agc == AGC)?"AGC":"MGC" );
				Display_Asc_String('1',20,32,DisBuffer);
				sprintf(DisBuffer,"CF : %01d%01d%01d%01d.%01d%01d%01d MHz",(u32)((cur_SysPara.cf+0.0005) * 10000)/10000000,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000000/1000000,
																																		(u32)((cur_SysPara.cf+0.0005) * 10000)%1000000/100000,(u32)((cur_SysPara.cf+0.0005) * 10000)%100000/10000			
																																		,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000/1000 ,(u32)((cur_SysPara.cf+0.0005) * 10000)%1000/100,(u32)((cur_SysPara.cf+0.0005) * 10000)%100/10);
				Display_Asc_String('1',20,46,DisBuffer);
				sprintf(DisBuffer,"%s",(cur_SysPara.rem == LOCAL)?"LOCAL ":"REMOTE");
				Display_Asc_String('1',200,56,DisBuffer);
				CGA_Set(50-cur_SysPara.cg);
				CGB_Set(50-cur_SysPara.cg);
				Save_SysPara(cur_SysPara,AGC_BASIC1,AGC_BASIC2);
				oldPage = Page;
			}
			if(Que_Len > 0)                								//队列长度大于0 
			{ //远端操作
				if(paser(*Parse_Que,cmd,cmd_data))						//解码
				{	
					if(strcmp(cmd,"REM") == 0)
					{
						if(strcmp(cmd_data,"OFF\r") == 0)
						{
							cur_SysPara.rem = LOCAL;	
							Udp_SendStr(">%d/REM_OFF\n",cur_SysPara.addr);
						}
						else if(strcmp(cmd_data,"ON\r") == 0)
						{
							cur_SysPara.rem = REMOTE;
							Udp_SendStr(">%d/REM_ON\n",cur_SysPara.addr);
						}
						else
							Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); 				//返回参数错误
					}
					else if(strcmp(cmd,"CF") == 0)             	  //远程设置/查询频率
					{
							if(strcmp(cmd_data,"?\r") == 0)
								Udp_SendStr(">%d/CF_%07.3f\n",cur_SysPara.addr,cur_SysPara.cf);
							else
							{
								if(cur_SysPara.rem == REMOTE)
								{
									tmp_double = atof(cmd_data); 							//远程设置频率 
									if(tmp_double >= 1698.000 && tmp_double <= 1710.000)
									{
										cur_SysPara.cf = tmp_double;
										if(HMC832B_FreSet(cur_SysPara.cf) && HMC832A_FreSet(cur_SysPara.cf)) //设置频率成功
										{
											Udp_SendStr(">%d/CF_%07.3f\n",cur_SysPara.addr,cur_SysPara.cf);		
										}
										else
										{
											//报错失锁
										}
									}
									else
										Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
								}
								else
									Udp_SendStr(">%d/REM ERR!\n",cur_SysPara.addr); 				//返回非法远程操作
							}					
					}
					else if(strcmp(cmd,"CG") == 0)								  //远程设置/查询变频增益
					{
						if(strcmp(cmd_data,"?\r") == 0)
						{
							if(cur_SysPara.agc == MGC)
								Udp_SendStr(">%d/CG_%02d\n",cur_SysPara.addr,cur_SysPara.cg);
							else
							{
								Udp_SendStr(">%d/CG_%02d\n",cur_SysPara.addr,cur_AGCVal1);
							}
						}
						else
						{
							if(cur_SysPara.rem == REMOTE)
							{
								if(cur_SysPara.agc == MGC)
								{
									tmp_int = atoi(cmd_data); 							//远程设置变频增益
									if(tmp_int <= 50) //增益控制范围0-50dB
									{
										cur_SysPara.cg = tmp_int;
										CGA_Set(50-cur_SysPara.cg); //设置A通道增益
										CGB_Set(50-cur_SysPara.cg); //设置B通道增益
										Udp_SendStr(">%d/CG_%02d\n",cur_SysPara.addr,cur_SysPara.cg);
									}
									else
										Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
								}
								else
									Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
							}
							else
								Udp_SendStr(">%d/REM ERR!\n",cur_SysPara.addr); 				//返回非法远程操作
						}
					}
					else if(strcmp(cmd,"ATT") == 0)							//远程设置/查询衰减
					{
						if(strcmp(cmd_data,"?\r") == 0)
							Udp_SendStr(">%d/ATT_%02d\n",cur_SysPara.addr,cur_SysPara.att);
						else
						{
							if(cur_SysPara.rem == REMOTE)
							{
								tmp_int = atoi(cmd_data); 
								if(tmp_int <= 30)   //末级衰减 范围 0 - 30dB
								{
									cur_SysPara.att = tmp_int;
									ATTA_Set(cur_SysPara.att);
									ATTB_Set(cur_SysPara.att);
									Udp_SendStr(">%d/ATT_%02d\n",cur_SysPara.addr,cur_SysPara.att);
								}
								else
									Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
							}
							else
								Udp_SendStr(">%d/REM ERR!\n",cur_SysPara.addr); 				//返回非法远程操作
						}		
					}
					else if(strcmp(cmd,"BW") == 0)								//远程设置/查询带宽
					{
	//					if(strcmp(cmd_data,"180")
	//						cur_SysPara.bw =  BW_180Mhz;
	//					else if(strcmp(cmd_data,"250")
	//						cur_SysPara.bw =  BW_250Mhz;
	//					else if(strcmp(cmd_data,"500")
	//						cur_SysPara.bw =  BW_500Mhz;
	//					else if(strcmp(cmd_data,"?")
	//					;
	//					else
	//						;
					}
					else if(strcmp(cmd,"AGC") == 0)							//远程设置AGC/MGC工作模式
					{
						if(cur_SysPara.rem == REMOTE)
						{
							if(strcmp(cmd_data,"1\r") == 0)
							{
								cur_SysPara.agc = AGC;
								Udp_SendStr(">%d/AGC_1\n",cur_SysPara.addr);
							}
							else if(strcmp(cmd_data,"0\r") == 0)
							{
								cur_SysPara.agc = MGC;
								cur_SysPara.cg = 50;
								CGA_Set(50 - cur_SysPara.cg);
								CGB_Set(50 - cur_SysPara.cg);
								Udp_SendStr(">%d/AGC_0\n",cur_SysPara.addr);
							}
							else
								Udp_SendStr(">%d/PARA ERR!\n"); //返回参数错误
						}
						else
							Udp_SendStr(">%d/REM ERR!\n",cur_SysPara.addr); 				//返回非法远程操作
					}
					else if(strcmp(cmd,"FA") == 0)								//远程查询本振工作状态
					{
						if(strcmp(cmd_data,"?\r") == 0)
							Udp_SendStr(">%d/FA_%02d\n",cur_SysPara.addr,FA_Check());
						else
							Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
					}
					else if(strcmp(cmd,"STATUS") == 0)						//远程查询现行工作状态
					{
						if(strcmp(cmd_data,"?\r") == 0)
						{
							if(cur_SysPara.rem == REMOTE)
							{
								Udp_SendStr(">%d/REM_ON,CF_%08.3f,CG_%02d,ATT_%02d,AGC_%01d,FA_%02d\n",cur_SysPara.addr,cur_SysPara.cf,cur_SysPara.cg,cur_SysPara.att,cur_SysPara.agc,FA_Check());
							}
							else
							{
								Udp_SendStr(">%d/REM_OFF,CF_%08.3f,CG_%02d,ATT_%02d,AGC_%01d,FA_%02d\n",cur_SysPara.addr,cur_SysPara.cf,cur_SysPara.cg,cur_SysPara.att,cur_SysPara.agc,FA_Check()); 
							}
						}
						else
							Udp_SendStr(">%d/PARA ERR!\n",cur_SysPara.addr); //返回参数错误
					}
					else
					{
						Udp_SendStr(">%d/CMD ERR!\n",cur_SysPara.addr); 			//UDP返回非法指令错误
					}
					
					if(cur_SysPara.agc == MGC)
					{
						sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
						Display_Asc_String('1',20,4,DisBuffer);
					}
					else
					{
						sprintf(DisBuffer,"CG : xx dB");
						Display_Asc_String('1',20,4,DisBuffer);
					}
					sprintf(DisBuffer,"ATT: %01d%01d dB",cur_SysPara.att / 10 , cur_SysPara.att % 10);
					Display_Asc_String('1',20,18,DisBuffer);
					sprintf(DisBuffer,"AGC: %s",(cur_SysPara.agc == AGC)?"AGC":"MGC" );
					Display_Asc_String('1',20,32,DisBuffer);
					sprintf(DisBuffer,"CF : %01d%01d%01d%01d.%01d%01d%01d MHz",(u32)((cur_SysPara.cf+0.0005) * 10000)/10000000,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000000/1000000,
																																			(u32)((cur_SysPara.cf+0.0005) * 10000)%1000000/100000,(u32)((cur_SysPara.cf+0.0005) * 10000)%100000/10000			
																																			,(u32)((cur_SysPara.cf+0.0005) * 10000)%10000/1000 ,(u32)((cur_SysPara.cf+0.0005) * 10000)%1000/100,(u32)((cur_SysPara.cf+0.0005) * 10000)%100/10);
					Display_Asc_String('1',20,46,DisBuffer);
					sprintf(DisBuffer,"%s",(cur_SysPara.rem == LOCAL)?"LOCAL ":"REMOTE");
					Display_Asc_String('1',200,56,DisBuffer);
					Save_SysPara(cur_SysPara,AGC_BASIC1,AGC_BASIC2);
				}
				else
					Udp_SendStr(">%d/CMD ERR!\n",cur_SysPara.addr); 						//UDP返回非法指令错误
				Que_Len--;
			}
			if(cur_SysPara.rem == LOCAL)									//本地工作模式
			{
				if(!key_flag)                                   //按键 显示
				{
					if(Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok)
					{
						key_delay++;
						if((Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok) && (key_delay > 200))
						{
							key_delay = 0;
							if(Key_Up)
							{
								switch(Tab)
								{
									case Tab_CG:
										switch(CG_Sel)
										{
											case 1: 
												if(cur_SysPara.cg <= 40)
												{
													cur_SysPara.cg = cur_SysPara.cg + 10;
												}
												break;
											case 2: 				
												if(cur_SysPara.cg < 50)
												{
													cur_SysPara.cg = cur_SysPara.cg + 1;
												}						
												break;
											default: break;
										}
										CGA_Set(50-cur_SysPara.cg);
										CGB_Set(50-cur_SysPara.cg);
										sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
										Display_Asc_String('1',20,4,DisBuffer);	
										break;
									case Tab_ATT:
										switch(ATT_Sel)
										{
											case 1: 
												if(cur_SysPara.att <= 20)
												{
													cur_SysPara.att = cur_SysPara.att + 10;
												}
												break;
											case 2: 				
												if(cur_SysPara.att < 30)
												{
													cur_SysPara.att = cur_SysPara.att + 1;
												}						
												break;
											default: break;
										}
										ATTA_Set(cur_SysPara.att);
										ATTB_Set(cur_SysPara.att);
										sprintf(DisBuffer,"ATT: %01d%01d dB",cur_SysPara.att / 10 , cur_SysPara.att % 10);
										Display_Asc_String('1',20,18,DisBuffer);
										break;
									case 3:
										if(AGC_Sel)
										{
											cur_SysPara.agc = !cur_SysPara.agc;
											if(cur_SysPara.agc == MGC)
											{
												cur_SysPara.cg = 50;
												CGA_Set(50 - cur_SysPara.cg);
												CGB_Set(50 - cur_SysPara.cg);
												sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
												Display_Asc_String('1',20,4,DisBuffer);	
											}
											else
											{
													sprintf(DisBuffer,"CG : xx dB");
													Display_Asc_String('1',20,4,DisBuffer);		
											}
											sprintf(DisBuffer,"AGC: %s",(cur_SysPara.agc == AGC)?"AGC":"MGC" );
											Display_Asc_String('1',20,32,DisBuffer);
										}
										break;
									case 4:
										switch(CF_Sel)
										{
											case 1: 
												break;
											case 2: 				
												if((int)((cur_SysPara.cf+0.0005)*10000) / 1000000 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 100	;	
												break;
											case 3: 	
												if((int)((cur_SysPara.cf+0.0005)*10000) / 100000 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 10	;		
					
												break;
											case 4: 				
													if((int)((cur_SysPara.cf+0.0005)*10000)/10000 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 1	;	
												break;
											case 5: 				
												if((int)((cur_SysPara.cf+0.0005)*10000)/1000 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 0.1	;	
												break;
											case 6: 				
												if((int)((cur_SysPara.cf+0.0005)*10000) / 100 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 0.01	;											
												break;
											case 7: 
												if((int)((cur_SysPara.cf+0.0005)*10000) / 100 % 10 <= 9)
													cur_SysPara.cf = cur_SysPara.cf + 0.001	;										
												break;
											default: break;
										}
										if(cur_SysPara.cf >= 1710.000 )
										{
											cur_SysPara.cf = 1710.000;
										}
										if(cur_SysPara.cf <= 1698.000 )
										{
											cur_SysPara.cf = 1698.000;
										}
										
										if(HMC832B_FreSet(cur_SysPara.cf) && HMC832A_FreSet(cur_SysPara.cf)) //设置频率成功
										{
											sprintf(DisBuffer,"CF : %01d%01d%01d%01d.%01d%01d%01d MHz",(int)((cur_SysPara.cf+0.0005) * 10000)/10000000,(int)((cur_SysPara.cf+0.0005) * 10000)%10000000/1000000,
																																		(int)((cur_SysPara.cf+0.0005) * 10000)%1000000/100000,(int)((cur_SysPara.cf+0.0005) * 10000)%100000/10000			
																																		,(int)((cur_SysPara.cf+0.0005) * 10000)%10000/1000 ,(int)((cur_SysPara.cf+0.0005) * 10000)%1000/100,(int)((cur_SysPara.cf+0.0005) * 10000)%100/10);
											Display_Asc_String('1',20,46,DisBuffer);									
										}
										break;
									default: break;
								}
							}
							else if(Key_Down)
							{
								switch(Tab)
								{
									case Tab_CG:
										switch(CG_Sel)
										{
											case 1: 
												if(cur_SysPara.cg >= 10)
												{
													cur_SysPara.cg = cur_SysPara.cg - 10;
												}
												break;
											case 2: 				
												if(cur_SysPara.cg > 0)
												{
													cur_SysPara.cg = cur_SysPara.cg - 1;
												}						
												break;
											default: break;
										}
										CGA_Set(50 - cur_SysPara.cg);
										CGB_Set(50 - cur_SysPara.cg);
										sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
										Display_Asc_String('1',20,4,DisBuffer);	
										break;
									case Tab_ATT:
										switch(ATT_Sel)
										{
											case 1: 
												if(cur_SysPara.att >= 10)
												{
													cur_SysPara.att = cur_SysPara.att - 10;
												}
												break;
											case 2: 				
												if(cur_SysPara.att > 0)
												{
													cur_SysPara.att = cur_SysPara.att - 1;
												}						
												break;
											default: break;
										}
										ATTA_Set(cur_SysPara.att);
										ATTB_Set(cur_SysPara.att);
										sprintf(DisBuffer,"ATT: %01d%01d dB",cur_SysPara.att / 10 , cur_SysPara.att % 10);
										Display_Asc_String('1',20,18,DisBuffer);
										break;
									case 3:
										if(AGC_Sel)
										{
											cur_SysPara.agc = !cur_SysPara.agc;
											if(cur_SysPara.agc == MGC)
											{
												cur_SysPara.cg = 50;
												CGA_Set(50 - cur_SysPara.cg);
												CGB_Set(50 - cur_SysPara.cg);
												sprintf(DisBuffer,"CG : %01d%01d dB",cur_SysPara.cg / 10 , cur_SysPara.cg%10);
												Display_Asc_String('1',20,4,DisBuffer);	
											}
											else
											{
													sprintf(DisBuffer,"CG : xx dB");
													Display_Asc_String('1',20,4,DisBuffer);		
											}
											sprintf(DisBuffer,"AGC: %s",(cur_SysPara.agc == AGC)?"AGC":"MGC" );
											Display_Asc_String('1',20,32,DisBuffer);
										}
										break;
									case 4:
										switch(CF_Sel)
										{
											case 1: 
												break;
											case 2: 				
												if((int)((cur_SysPara.cf+0.0005)*1000) / 100000 % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 100	;		
												break;
											case 3: 	
												if((int)((cur_SysPara.cf+0.0005)*1000)  / 10000 % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 10	;		
												break;
											case 4: 				
												if((int)((cur_SysPara.cf+0.0005)*1000) /1000 % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 1	;	
												break;
											case 5: 				
												if((int)((cur_SysPara.cf+0.0005)*1000) /100 % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 0.1	;	
												break;
											case 6: 				
												if((int)((cur_SysPara.cf+0.0005)*1000) /10 % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 0.01	;											
												break;
											case 7: 	
												if((int)((cur_SysPara.cf+0.0005)*1000)  % 10 >= 0)
													cur_SysPara.cf = cur_SysPara.cf - 0.001	;										
												break;
											default: break;
										}
										if(cur_SysPara.cf >= 1710.000 )
										{
											cur_SysPara.cf = 1710.000;
										}
										if(cur_SysPara.cf <= 1698.000 )
										{
											cur_SysPara.cf = 1698.000;
										}
										if(HMC832B_FreSet(cur_SysPara.cf) && HMC832A_FreSet(cur_SysPara.cf)) //设置频率成功
										{
											sprintf(DisBuffer,"CF : %01d%01d%01d%01d.%01d%01d%01d MHz",(int)((cur_SysPara.cf+0.0005) * 10000)/10000000,(int)((cur_SysPara.cf+0.0005) * 10000)%10000000/1000000,
																																		(int)((cur_SysPara.cf+0.0005) * 10000)%1000000/100000,(int)((cur_SysPara.cf+0.0005) * 10000)%100000/10000			
																																		,(int)((cur_SysPara.cf+0.0005) * 10000)%10000/1000 ,(int)((cur_SysPara.cf+0.0005) * 10000)%1000/100,(int)((cur_SysPara.cf+0.0005) * 10000)%100/10);
											Display_Asc_String('1',20,46,DisBuffer);									
										}
										break;
									default: break;
								}
								
							}
							else if(Key_Left)
							{
								switch(Tab)
								{
									case Tab_CG:
										CG_Sel--;
										if(CG_Sel <= 0)
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0x00,17,17,12,12);
											CG_Sel = 3;
										}
										else if(CG_Sel == 1)
										{
											Fill_Block(0x00,17,17,12,12); //下划线
											Fill_Block(0xff,15,15,12,12);
										}
										else if(CG_Sel == 2)
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0xff,17,17,12,12);
										}	
										else if(CG_Sel == 3)
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0x00,17,17,12,12);
										}
										else
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0x00,17,17,12,12);
										}
										break;
									case Tab_ATT:
										ATT_Sel--;
										if(ATT_Sel <= 0)
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0x00,17,17,26,26);
											ATT_Sel = 3;
										}
										else if(ATT_Sel == 1)
										{
											Fill_Block(0x00,17,17,26,26); //下划线
											Fill_Block(0xff,15,15,26,26);
										}
										else if(ATT_Sel == 2)
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0xff,17,17,26,26);
										}	
										else if(ATT_Sel == 3)
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0x00,17,17,26,26);
										}
										else
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0x00,17,17,26,26);
										}
										break;
									case 3:
										AGC_Sel = !AGC_Sel;
										if(AGC_Sel)
											Fill_Block(0xff,15,19,40,40);
										else
											Fill_Block(0x00,15,19,40,40);
										break;
									case 4:
										if(CF_Sel == 0)
											CF_Sel = 8;
										CF_Sel --;
										switch(CF_Sel)
										{
											case 0:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 1:
												Fill_Block(0xff,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 2:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0xff,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 3:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0xff,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 4:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0xff,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 5:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0xff,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 6:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0xff,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 7:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0xff,29,29,54,54);
												break;
											default: 
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
											break;
										}
										break;
									default: break;
								}
							}
							else if(Key_Right)
							{
								switch(Tab)
								{
									case 1:
										CG_Sel++;
										if(CG_Sel == 1)
										{
											Fill_Block(0x00,17,17,12,12); //清除上一条下划线
											Fill_Block(0xff,15,15,12,12);
										}
										else if(CG_Sel == 2)
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0xff,17,17,12,12);
										}	
										else if(CG_Sel >= 3)
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0x00,17,17,12,12);
											CG_Sel = 0;
										}
										else
										{
											Fill_Block(0x00,15,15,12,12);
											Fill_Block(0x00,17,17,12,12);
										}
										break;
									case Tab_ATT:
										ATT_Sel++;
										if(ATT_Sel == 1)
										{
											Fill_Block(0x00,17,17,26,26); //清除上一条下划线
											Fill_Block(0xff,15,15,26,26);
										}
										else if(ATT_Sel == 2)
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0xff,17,17,26,26);
										}	
										else if(ATT_Sel >= 3)
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0x00,17,17,26,26);
											ATT_Sel = 0;
										}
										else
										{
											Fill_Block(0x00,15,15,26,26);
											Fill_Block(0x00,17,17,26,26);
										}
										break;
									case 3:
										AGC_Sel = !AGC_Sel;
										if(AGC_Sel)
											Fill_Block(0xff,15,19,40,40);
										else
											Fill_Block(0x00,15,19,40,40);
										break;
									case 4:
										CF_Sel ++;
										if(CF_Sel == 8)
											CF_Sel = 0;
										switch(CF_Sel)
										{
											case 0:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 1:
												Fill_Block(0xff,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 2:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0xff,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 3:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0xff,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 4:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0xff,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 5:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0xff,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 6:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0xff,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
												break;
											case 7:
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0xff,29,29,54,54);
												break;
											default: 
												Fill_Block(0x0,15,15,54,54);
												Fill_Block(0x0,17,17,54,54);
												Fill_Block(0x0,19,19,54,54);
												Fill_Block(0x0,21,21,54,54);
												Fill_Block(0x0,25,25,54,54);
												Fill_Block(0x0,27,27,54,54);
												Fill_Block(0x0,29,29,54,54);
											break;
										}

										break;
									default: break;
								}
							}
							else if(Key_Ok)
							{
								if(Page == Main_Page)
								{
									Tab++;
									Fill_Block(0x00,15,15,12,12); //清下划线
									Fill_Block(0x00,17,17,12,12);
									Fill_Block(0x00,15,15,26,26);
									Fill_Block(0x00,17,17,26,26);
									Fill_Block(0x00,15,19,40,40);					
									Fill_Block(0x00,15,15,54,54);
									Fill_Block(0x00,17,17,54,54);
									Fill_Block(0x00,19,19,54,54);
									Fill_Block(0x00,21,21,54,54);
									Fill_Block(0x00,25,25,54,54);
									Fill_Block(0x00,27,27,54,54);
									Fill_Block(0x00,29,29,54,54);
									if(Tab > 0 && Tab != old_Tab)
									{
										if(Tab > 1)
											Display_Asc_String('1',15,((Tab - 1) * 14) - 10," ");	
										if(Tab < 5)
											Display_Asc_String('1' , 15 , (Tab*14) - 10 , "-");	
										old_Tab = Tab;
									}
									if(Tab == 5)
									{
										Tab = 0;
									}
								}
							}
							else
								;	
							key_flag = 1;
						}
					}
					else
					{
						key_delay = 0;
					}
				}
				else
				{
					if(!Key_Up && !Key_Down && !Key_Left && !Key_Right && !Key_Ok) //判断按键弹起
					{
						if(!Key_Up && !Key_Down)
							Save_SysPara(cur_SysPara,AGC_BASIC1,AGC_BASIC2);
						key_flag = 0;
						key_cont = 0;
					}
					else																											//判断长按
					{
						if(Key_Ok)
							key_cont++;
						else
							key_cont = 0;
						if(key_cont > 500000)
						{
								Page = Addr_Page;   
						}
					}
				}
						
			}
			else 																					//远程工作模式 检测按键 变为本地模式
			{
				if(!key_flag)                                   //
				{
					if(Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok)
					{
						key_delay++;
						if((Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok) && (key_delay > 200))
						{
							key_delay = 0;
							if(cur_SysPara.rem != LOCAL)
							{
								cur_SysPara.rem = LOCAL;
								sprintf(DisBuffer,"%s",(cur_SysPara.rem == LOCAL)?"LOCAL ":"REMOTE");
								Display_Asc_String('1',200,56,DisBuffer);
							}
							key_flag = 1;
						}
					}
					else
					{
						key_delay = 0;
					}
				}
				else
				{
					if(!Key_Up && !Key_Down && !Key_Left && !Key_Right && !Key_Ok) //判断按键弹起
					{
						key_flag = 0;
					}
				}
			}
		}
		else if(Page == Addr_Page)											//地址设置 界面  长按右键 AGC 校准
		{
			if(Page != oldPage)                           //从其他界面跳转到main 需要更新界面
			{
				Fill_RAM(0x00);
				delay_ms(50);
				Fill_RAM(0x00);
				sprintf(DisBuffer,"ADDR: %d  ", cur_SysPara.addr);
				Display_Asc_String('1',100,25,DisBuffer);	
				oldPage = Page;
				
			}
			if(!key_flag)                                   //按键 显示
			{
				if(Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok)
				{
					key_delay++;
					if((Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok) && (key_delay > 200))
					{
						key_delay = 0;
						if(Key_Up)
						{
								cur_SysPara.addr = cur_SysPara.addr + 1;
							sprintf(DisBuffer,"ADDR: %d  ", cur_SysPara.addr);
							Display_Asc_String('1',100,25,DisBuffer);	
						}
						else if(Key_Down)
						{
							if(cur_SysPara.addr == 0)
								cur_SysPara.addr = 0;
							else
								cur_SysPara.addr = cur_SysPara.addr - 1;
							sprintf(DisBuffer,"ADDR: %d  ", cur_SysPara.addr);
							Display_Asc_String('1',100,25,DisBuffer);	
						}
						else if(Key_Ok)
						{
							Page = Main_Page;
						}
						else
							;	
						key_flag = 1;
					}
				}
				else
				{
					key_delay = 0;
				}
			}
			else
			{
				if(!Key_Up && !Key_Down && !Key_Left && !Key_Right && !Key_Ok) //判断按键弹起  右键长按 AGC校准模式
				{
					key_flag = 0;
					key_cont = 0;
				}
				else																											//判断长按
				{
					if(Key_Right)
						key_cont++;
					else
						key_cont = 0;
					if(key_cont > 500000)
					{
							Page = Agccal_Page;   
					}
				}
			}
		}
		else if(Page == Agccal_Page)
		{
			if(Page != oldPage)                           //从其他界面跳转到main 需要更新界面  上键 校准A    下键校准B
			{
				Fill_RAM(0x00);
				delay_ms(50);
				Fill_RAM(0x00);
				sprintf(DisBuffer,"AGC_RF1VAL: %.3f  ", AGC_BASIC1);
				Display_Asc_String('1',80,20,DisBuffer);
				sprintf(DisBuffer,"AGC_RF2VAL: %.3f  ", AGC_BASIC2);
				Display_Asc_String('1',80,30,DisBuffer);	
				oldPage = Page;
			}
			if(!key_flag)                                  
			{
				if(Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok)
				{
					key_delay++;
					if((Key_Up || Key_Down || Key_Left || Key_Right || Key_Ok) && (key_delay > 200))
					{
						key_delay = 0;
						if(Key_Up)
						{
							AGC_BASIC1 = Cal_AGCVal(RF_1);
							sprintf(DisBuffer,"AGC_RF1VAL: %.3f  ", AGC_BASIC1);
							Display_Asc_String('1',80,20,DisBuffer);
						}
						else if(Key_Down)
						{
							AGC_BASIC2 = Cal_AGCVal(RF_2);
							sprintf(DisBuffer,"AGC_RF1VAL: %.3f  ", AGC_BASIC2);
							Display_Asc_String('1',80,30,DisBuffer);	
						}
						else if(Key_Ok)
						{
							Page = Main_Page;
						}
						else
							;	
						key_flag = 1;
					}
				}
				else
				{
					key_delay = 0;
				}
			}
			else
			{
				if(!Key_Up && !Key_Down && !Key_Left && !Key_Right && !Key_Ok) //判断按键弹起  右键长按 AGC校准模式
				{
					key_flag = 0;
					key_cont = 0;
				}
			}
		}
		else
			;
		if(cur_SysPara.agc == AGC) //AGC 模式
		{
			cur_AGCVal1 += AGC_Ctl(AGC_BASIC1 ,0.022,RF_1); 
			if(cur_AGCVal1 < 0)
				cur_AGCVal1 = 0;
			if(cur_AGCVal1 > 50)
				cur_AGCVal1 = 50;
			CGA_Set(cur_AGCVal1);
			cur_AGCVal2 += AGC_Ctl(AGC_BASIC2 ,0.022,RF_2);
			if(cur_AGCVal2 < 0)
				cur_AGCVal2 = 0;
			if(cur_AGCVal2 > 50)
				cur_AGCVal2 = 50;
			CGB_Set(cur_AGCVal2);
		}		
		IWDG->KR=0XAAAA;  												      //喂狗
	}
}
/************************END OF FILE**********************/

