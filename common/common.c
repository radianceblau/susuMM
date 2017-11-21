#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "../include/radia_comm.h"


//返回制定字符c在字符串str中的个数
int get_nums_char(char *str, char c)
{
	int i, num = 0;
	for(i = 0; i < strlen(str); i++)
		if(str[i] == c)
			num++;
	return num;
}

//返回指定字符c在字符串str中第num次出现的位置，如查找失败则返回-1
int get_cursor_char(char *str, char c, int num)
{
	int i, n = 0;
	if(num <= 0)
		return -1;
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == c)
			n++;
		if(n == num)
			return i;
	}
	return 0;
}

int open_client_socket(int *csd, char *ip, int port_num)
{
	struct sockaddr_in s_addr;
	
	*csd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == *csd)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");

	bzero(&s_addr,sizeof(struct sockaddr_in));
	s_addr.sin_family=AF_INET;
	s_addr.sin_addr.s_addr= inet_addr(ip);//"192.168.31.200");//("127.0.0.1");
	s_addr.sin_port=htons(port_num);
	printf("s_addr = %#x ,port : %#x\r\n",s_addr.sin_addr.s_addr,s_addr.sin_port);

	if(-1 == connect(*csd,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok !\r\n");
	return 1;
}

int creat_server_socket(int *ssd, int port_num)
{
	struct sockaddr_in s_addr;
	*ssd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == *ssd)
	{
			printf("socket fail ! \r\n");
			return -1;
	}
	printf("socket ok !\r\n");

	bzero(&s_addr,sizeof(struct sockaddr_in));
	s_addr.sin_family=AF_INET;
	s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	s_addr.sin_port=htons(port_num);

	if(-1 == bind(*ssd,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
			printf("bind fail !\r\n");
			return -1;
	}
	printf("bind ok !\r\n");

	if(-1 == listen(*ssd,5))
	{
			printf("listen fail !\r\n");
			return -1;
	}
	printf("listen ok\r\n");
	return 1;
}