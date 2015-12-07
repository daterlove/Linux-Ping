#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include<pthread.h>
#include"stdvar.h"

#define PACKET_SIZE 4096
#define MAX_COUNT 100

void Sleep(int ms);
/*两个发送接收时间的timeval结构相减*/
int time_sub(struct timeval *end,struct timeval *start);
/*分析判断程序参数,并设置相关全局变量*/
void GetArgments(int argc,char **argv);

/*校验和算法*/
unsigned short cal_chksum(unsigned short *addr,int len);
/*给定ICMP编号,打包报文*/
int pack(int pack_no);
/*发送数据包,count为发送的个数*/
void send_packet(void *nNum);
//把主机名转成ip,存到dest_addr里面
int GetIpByName(char *szHost);

/*解析数据包,传入需要解析的缓冲区和长度,正确返回0,错误返回-1*/
int unpack(char *buf,int len);
/*接收数据包*/
void recv_packet(int count);

/*统计*/
void Statistic();
/*打印所有参数的状态*/
void print_allArg();
/*打印帮助信息*/
void print_help();






