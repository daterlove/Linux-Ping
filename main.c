/*************************************************************************
    > File Name: main.c
    > Author: 浅握双手
    > ---------------- 
    > Created Time: 2015年12月03日 16时29分10秒 CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>

#include"common.h"


bool g_isCount;
UINT16 g_nCount;

bool g_isTTL;
UINT16 g_nTTL;

bool g_isWait;
UINT16 g_nWait;

char g_szDesIp[20];

struct sockaddr_in g_dest_addr;//远程主机地址
struct sockaddr_in g_from;//接收时候的来源地址

INT32 g_sockfd;//sock套接字
INT32 g_datalen=16;//报文数据部分大小

INT32 g_nNeedSend;//需要发送的数据包个数
INT32 g_nSend=0,g_nRecv=0;//记录发送给接受的包数

/*发送和接收的缓冲区*/

INT8 g_SendPacket[PACKET_SIZE];
INT8 g_RecvPacket[PACKET_SIZE];

/*记录发送和接收时间的数组*/
struct timeval g_start[MAX_COUNT];
struct timeval g_end[MAX_COUNT];

void Sleep(int ms)  
{  
    struct timeval delay;  
    delay.tv_sec = 0;  
    delay.tv_usec = ms * 1000; // 20 ms  
    select(0, NULL, NULL, NULL, &delay);  
}  


/*两个发送接收时间的timeval结构相减*/
int time_sub(struct timeval *end,struct timeval *start)
{       
	INT32 nResult;
	nResult=(end->tv_sec - start->tv_sec)*1000000+(end->tv_usec - start->tv_usec);
	/*
	printf("--  end->tv_sec:%ld,  end->tv_usec:%ld\n",end->tv_sec,end->tv_usec);
	printf("--start->tv_sec:%ld,start->tv_usec:%ld\n",start->tv_sec,start->tv_usec);
	printf("--end->tv_sec - start->tv_sec=%ld\n",(end->tv_sec - start->tv_sec));
	printf("--Test:%ld,%ld-----------\n\n",nResult,end->tv_usec - start->tv_usec);
	*/
	return nResult;
}

/*分析判断程序参数,并设置相关全局变量*/
void GetArgments(int argc,char **argv)
{
	/*初始化全局变量*/
/*	g_isCount=false;
	g_isTTL=false;
	g_isWait=false;
*/
	/*如果没有提供参数*/
	if(argc==1)
	{
		//printf("No Argments!\n");
		//print_help();
		return;
	}

	/*开始判断参数*/
	int i;
	for(i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			switch(tolower(argv[i][1]))
			{
				case 'h':
					print_help();
					exit(0);
					break;
				
				case 'c':
					if(i<argc-1)
					{
						g_isCount=true;
						g_nCount=atoi(argv[++i]);
					}
					break;

				case 't':
					if(i<argc-1)
					{
						g_isTTL=true;
						g_nTTL=atoi(argv[++i]);
					}
					break;
					
				case 'w':
					if(i<argc-1)
					{
						g_isWait=true;
						g_nWait=atoi(argv[++i]);
					}
					break;
			}
		}
		else
		{
			strcpy(g_szDesIp,argv[i]);
		}
	}
}


int main(int argc,char *argv[])
{	
	GetArgments(argc,argv);
	//printf("\n所有参数状态:\n");
	//print_allArg();
	
	/*主机参数为空*/
	if(strcmp(g_szDesIp,"")==0)
	{
		printf("目的主机不能为空!\n");
		print_help();
		exit(0);
	}
	else
	{
		GetIpByName(g_szDesIp);
	}
	 
	/*既那里RAW套接字,需要root权限*/
	g_sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	if(g_sockfd<0)
	{
		perror("socket建立错误");
	}

	if(g_isTTL)//有设置TTL选项
	{
		UINT32 ttl = g_nTTL;
		setsockopt(g_sockfd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl)); 
	}

	if(!g_isWait)//如果没有制定等待时间,则默认5秒
	{
		g_nWait=5;
	}
	signal( SIGINT,Statistic);
	pthread_t tid;//建立一个线程id
	int nCount=4;//默认发送个数
	
	if(g_isCount)//是否有数据包个数选项
	{
		int ret=pthread_create(&tid,NULL,(void *)send_packet,(void*)&g_nCount);
		if(ret!=0)
		{
			perror("创建线程错误");
		}

		g_nNeedSend=g_nCount;//设置需要发送的数据包个数
//		send_packet(g_nCount);
		recv_packet(g_nCount);	
	}
	else
	{
		g_nNeedSend=nCount;//设置需要发送的数据包个数
		
		int ret=pthread_create(&tid,NULL,(void *)send_packet,(void*)&nCount);
		if(ret!=0)
		{
			perror("创建线程错误");
		}
		
		/*send_packet(4);*/
		recv_packet(nCount);
	}
	Statistic();
}
