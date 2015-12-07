/*************************************************************************
    > File Name: Send.c
    > Author: 浅握双手
    > ---------------- 
    > Created Time: 2015年12月03日 16时30分51秒 CST
 ************************************************************************/

#include"common.h"
extern INT8 g_SendPacket[PACKET_SIZE];
extern INT32 g_datalen;
extern struct timeval g_start[MAX_COUNT];
extern INT32 g_sockfd;//sock套接字
extern struct sockaddr_in g_dest_addr;//远程主机地址
extern INT32 g_nSend;

/*校验和算法*/
unsigned short cal_chksum(unsigned short *addr,int len)
{       INT32 nleft=len;//长度
        INT32 sum=0;//和
        unsigned short *w=addr;
        unsigned short answer=0;
       /*把ICMP报头二进制数据以2字节为单位累加起来*/
        while(nleft>1)
        {       sum+=*w++;
                nleft-=2;
        }
        /*若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高
       //字节，这个2字节数据的低字节为0，继续累加*/
        if( nleft==1)
        {       *(unsigned char *)(&answer)=*(unsigned char *)w;
                sum+=answer;
        }
        sum=(sum>>16)+(sum&0xffff);
        sum+=(sum>>16);
        answer=~sum;
        return answer;
}
/*给定ICMP编号,打包报文*/
int pack(int pack_no)
{
	INT32 packsize;
	struct icmp *icmp;
	pid_t pid;//进程pid
	pid=getpid();

	icmp=(struct icmp*)g_SendPacket;//与发送换成你过去关联
        icmp->icmp_type=ICMP_ECHO;//类型
        icmp->icmp_code=0;
       	icmp->icmp_cksum=0;//这一步需要,否则下列校验码出错
        icmp->icmp_seq=pack_no;
       	icmp->icmp_id=pid;

 	packsize=8+g_datalen;//头部8个字节
	icmp->icmp_cksum=cal_chksum( (unsigned short *)icmp,packsize); /*校验算法*/
	
	return packsize;
}

/*发送数据包,count为发送的个数*/
void send_packet(void *nNum)
{
	/*创建线程需要,把void型转成其他类型*/
	INT32 *p = (INT32 *)nNum;
	INT32 count=*p;
	

	INT32 i,nPackSize;
	struct timeval tval;
	
	for(i=0;i<count;i++)
	{
		nPackSize=pack(i);//设置icmp报头

		/*发送前计算发送时间,否则有概率出现接收时间早于发送时间*/
		gettimeofday(&tval,NULL); 
		g_start[i]=tval;//记录发送时间

		int ret=sendto(g_sockfd,g_SendPacket,nPackSize,0,(struct sockaddr *)&g_dest_addr,sizeof(g_dest_addr));//发送数据

		//printf("第%d个发送时间%ld-%ld\n",i,tval.tv_sec,tval.tv_usec);
		
		if(ret<0)
		{
			perror("数据发送错误");
			continue;
		}
		g_nSend++;//发送数据包个数加一
		
		Sleep(300);//延时300毫秒
			
	}
	
	//printf("数据发送完毕\n");
}

//把主机名转成ip,存到dest_addr里面
int GetIpByName(char *szHost)
{
	INT32 test;
	struct hostent *host;
	/*初始化,dest全局变量*/
	bzero(&g_dest_addr,sizeof(g_dest_addr));
        g_dest_addr.sin_family=AF_INET;//设置为ipv4

	if(inet_addr(szHost)==INADDR_NONE)//是主机名
	{
		host=gethostbyname(szHost);
	
		if(host==NULL)
		{
			perror("获取主机名错误");
			exit(1);
		}
		//printf("hostname:%s\n",inet_ntoa(host->h_addr));
		memcpy( (char *)&g_dest_addr.sin_addr,host->h_addr,host->h_length);
	}
	else//是ip
	{
		g_dest_addr.sin_addr.s_addr = inet_addr(szHost);
	}
	printf("IP为:%s\n",inet_ntoa(g_dest_addr.sin_addr));
	return 0;
}
