/*************************************************************************
    > File Name: PrintInfo.c
    > Author: 浅握双手
    > ---------------- 
    > Created Time: 2015年12月03日 16时31分18秒 CST
 ************************************************************************/

#include"common.h"
extern INT32 g_nSend;
extern INT32 g_nRecv;
extern INT32 g_sockfd;//sock套接字
extern bool g_isCount;
extern UINT16 g_nCount;
extern bool g_isTTL;
extern UINT16 g_nTTL;
extern bool g_isWait;
extern UINT16 g_nWait;
extern char g_szDesIp[20];

/*统计*/
void Statistic()
{	
	float nRate;
	if(g_nSend!=0)
	{
		nRate=(float)(g_nSend - g_nRecv)/(float)g_nSend*100;
	}
	else
	{
		nRate=0;
	}	
	printf("--------------------------------------------------\n");
	printf("已发送%d数据包,收到%d回包,%.2f%%丢包率\n",g_nSend,g_nRecv,nRate);
	printf("--------------------------------------------------\n");
	close(g_sockfd);
        exit(1);
}

/*打印所有参数的状态*/
void print_allArg()
{
	printf("\ng_isCount=%d\ng_nCount=%d\n",g_isCount,g_nCount);
	printf("g_isTTL=%d\ng_nTTL=%d\n",g_isTTL,g_nTTL);
	printf("g_isWait=%d\ng_nWait=%d\n",g_isWait,g_nWait);
	printf("主机=%s\n\n",g_szDesIp);
}

/*打印帮助信息*/
void print_help()
{
	printf("Usage:Ping [-c count] [-t ttl] [-w timeout] destination\n");
}

