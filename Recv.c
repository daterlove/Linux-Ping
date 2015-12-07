/*************************************************************************
    > File Name: Recv.c
    > Author: 浅握双手
    > ---------------- 
    > Created Time: 2015年12月03日 16时31分01秒 CST
 ************************************************************************/

#include"common.h"
extern struct timeval g_end[MAX_COUNT];
extern struct timeval g_start[MAX_COUNT];
extern struct sockaddr_in g_from;//接收时候的来源地址
extern INT32 g_nRecv;
extern INT32 g_nNeedSend;//需要发送的数据包个数
extern UINT16 g_nWait;
extern INT32 g_sockfd;//sock套接字
extern INT8 g_RecvPacket[PACKET_SIZE];

/*解析数据包,传入需要解析的缓冲区和长度,正确返回0,错误返回-1*/
int unpack(char *buf,int len)
{
	struct ip *ip;
        struct icmp *icmp;
	struct timeval tval;

	ip=(struct ip *)buf;//ip头与缓冲区关联
	INT32 nIplen=ip->ip_hl<<2; /*求ip报头长度,即ip报头的长度标志乘4*/ 

	icmp=(struct icmp *)(buf+nIplen);  /*越过ip报头,指向ICMP报头*/
	
	if(len-nIplen<8)//如果icmp报头长度小于8,则不合理
	{
		printf("解包时错误:ICMP报头过小\n");	
		return -1;
	}
	
	pid_t pid;//进程pid
	pid=getpid();
	if( (icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id==pid))//如果是对应的回包
	{
		gettimeofday(&tval,NULL); 
		g_end[icmp->icmp_seq]=tval;//记录接收的时间
		UINT32 rtt=time_sub(&g_end[icmp->icmp_seq],&g_start[icmp->icmp_seq]);

		/*
		printf("第%d个接收时间%ld-%ld\n",icmp->icmp_seq,tval.tv_sec,tval.tv_usec);
		printf("rtt=%ld,icmp-seq=:%d\n",rtt,icmp->icmp_seq);
		*/

		/*显示相关信息*/
                printf("%d byte from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
                        len,inet_ntoa(g_from.sin_addr),icmp->icmp_seq,ip->ip_ttl,(float)rtt/1000.0);
		g_nRecv++;//(这时候收到正确数据包)记录收到的数据包数
	}
	else
	{
		return -1;
	}

	return 0;

}

/*接收数据包*/
void recv_packet(int count)
{
	signal(SIGALRM,Statistic);
	
	INT32 n;//返回的字节数
	INT32 nfromlen=sizeof(g_from);//来源地址大小
	//int i;
	while(g_nRecv<g_nNeedSend)
	{
		/*设置闹钟*/
		alarm(g_nWait);
		
		n=recvfrom(g_sockfd,g_RecvPacket,sizeof(g_RecvPacket),0,(struct sockaddr *)&g_from,&nfromlen);
		
		if(n<0)
		{
			perror("接收数据错误");
			continue;
		}
		//解析数据包
		int ret=unpack(g_RecvPacket,n);
		if(ret==-1)//发生错误
		{
			continue;
		}
		//Sleep(300); //停留300毫秒   
		
	}
	printf("数据接收完成\n");
}

