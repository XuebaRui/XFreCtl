#include "stdio.h"
#include "string.h"
#include "mytypedef.h"
#if DeBug
	#include "BSP_ZN200.h"
#endif
static void split(char *src,const char *separator,char **dest,int *num) {
      char *pNext;
       int count = 0;
       if (src == NULL || strlen(src) == 0)
          return;
       if (separator == NULL || strlen(separator) == 0)
         return;    
      pNext = strtok(src,separator);
      while(pNext != NULL) {
           *dest++ = pNext;
           ++count;
          pNext = strtok(NULL,separator);  
     }  
     *num = count;
}     
extern Sys_Para cur_SysPara;
u8 paser(char *s,char *rtn_cmd ,char *rtn_data)
{
	char *head[2] = {0};
	char *cmd[2] = {0};
	char addr[4] = {0};
	int num = 0;
	split(s,"/",head,&num);
	if(num == 0 || head[0] == NULL || head[1] == NULL)
		return 0;
	sprintf(addr,"<%d",cur_SysPara.addr);
	if(!strcmp(head[0],addr)) //
	{
		split(head[1],"_",cmd,&num);
		if(num == 0 || cmd[0] == NULL || cmd[1] == NULL)
			return 0;
		else
		{
			strcpy(rtn_cmd,cmd[0]);	
			strcpy(rtn_data,cmd[1]);
			#if DeBug
				//Udp_SendStr("Parser Successful !!!");
			#endif
			return 1;
		}
	}
	else
		return 0;
} 
/************************END OF FILE**********************/