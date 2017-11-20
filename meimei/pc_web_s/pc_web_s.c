#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>


#define WEB_PORT_NUM	9992
#define MAX_CLIENT_NUM	100			//此server能够接受的client最大数量
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数
#define MAX_MSG_LEN	10240

char client_name_table[MAX_CLIENT_NUM][MAX_CLIENT_NAME_LEN] = {0};//1，每个client需要上报一个name，用于标示自身。2，client_neme_table中为NULL表示该位置的连接资源未被使用。3，client_name_table中的位置确定connect_th表中的位置
pthread_t connect_th, read_event_th, client_th[MAX_CLIENT_NUM];//每个client对用一个线程
char thread_send_flag[MAX_CLIENT_NUM] = {0};
int client_socket_descriptor[MAX_CLIENT_NUM];
int service_socket_descriptor;

char recv_msg[MAX_MSG_LEN], send_msg[MAX_MSG_LEN];

int get_free_client_internal_id()
{
	int i, free_num = 0;
	for(i = 0; i < MAX_CLIENT_NUM; i++)
	{
		if(client_name_table[i][0] == 0)
			free_num++;
	}
	return free_num;
}

int alloc_client_internal_id()//为当前client分配一个内部资源id
{
	int i;
	for(i = 0; i < MAX_CLIENT_NUM; i++)
	{
		if(client_name_table[i][0] == 0)
		{
			memcpy(client_name_table[i], "noname", strlen("noname"));
			return i;
		}
	}
	return -1;
}

int free_client_internal_id(int id)
{
	memset(client_name_table[id], 0, MAX_CLIENT_NAME_LEN);
	return 0;
}

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

//http://172.16.12.135:9992/?x=10&y=45&m=3&bg_color=0xFF00&wd_color=0x0000
//GET /?x=10&y=45&m=3&bg_color=0xFF00&wd_color=0x0000 HTTP/1.1
int parse_requestline(char* req_line, int * para_x, int *para_y, int *para_m, int *para_bg_color, int* para_wd_color)
{
	int n, next_n;
	char num_string[10];
	if(strncmp(req_line, "GET", 3) != 0)//本web server只处理GET请求
	{//请求命令格式是否正确
		printf("error request! not GET method\n");
		return -1;
	}
	if(get_nums_char(req_line, '&') != 4)
	{
		printf("error request! error nums of &\n");
		return -1;
	}
	else
	{
		n = get_cursor_char(req_line, '?', 1);
		next_n = get_cursor_char(req_line, '&', 1);
		strncpy(num_string, req_line[n+1], next_n - n);//will do
		//*para_x = atoi();
	}
	return 1;
}

void *client_handler(void *pcii)
{
	int ret, cii = *((int *)pcii), n;
	int csd = client_socket_descriptor[cii];
	char buffer[1024];
	int para_x, para_y, para_m, para_bg_color, para_wd_color;
	//此处模拟http协议，因为http协议是无状态的，因此不保持连接，通信结束立即断开
	memset(&recv_msg, 0, MAX_MSG_LEN);
	ret = read(csd, (void *)(&recv_msg), MAX_MSG_LEN);
	printf("recv:%s\n", recv_msg);
	n = get_cursor_char(recv_msg, '\r', 1);//获取字符串第一行即requestline的位置
	memset(buffer, 0, 1024);
	strncpy(buffer, recv_msg, n+2);
	if(1 == parse_requestline(buffer, &para_x, &para_y, &para_m, &para_bg_color, &para_wd_color))//解析http请求行中参数成功
	{
		memset(send_msg, 0, MAX_MSG_LEN);
		memset(buffer, 0, 1024);
		sprintf(buffer, "<html><body><h1>It works!</h1><p>This is the default web page for this server.</p><p>The web server software is running but no content has been added, yet.</p></body></html>");
		sprintf(send_msg, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContend-Length:%d\r\n\r\n", strlen(buffer));
		strcat(send_msg, buffer);
		send(csd, send_msg, strlen(send_msg), 0);
		printf("send ok!\n");
	}
	printf("disconnect one, internal id %d\n", cii);
	free_client_internal_id(cii);
	close(csd);
}

void *connect_handler(void *arg)
{
	int addr_len = sizeof(struct sockaddr_in), client_internal_id;	
	struct sockaddr_in c_addr;
	int csd;
	while(1)
	{
		if(get_free_client_internal_id != 0)
		{
			//printf("will client internal id:%d\n", client_internal_id);
			csd = accept(service_socket_descriptor, (struct sockaddr *)(&c_addr), &addr_len);
			if(-1 == csd)
			{
					printf("accept fail !\r\n");
					free_client_internal_id(client_internal_id);
					continue;
			}
			client_internal_id = alloc_client_internal_id();
			client_socket_descriptor[client_internal_id] = csd;
			printf("accept one, internal id:%d\n", client_internal_id);
			pthread_create(&(client_th[client_internal_id]),NULL,client_handler,(void *)(&client_internal_id));//为每一个成功连接的client创建一个独立线程，处理client数据请求	
			//pthread_join(th[client_internal_id],NULL);
		}
		else
		{
			printf("client num limit 10!\n");
			usleep(1000000);
		}
		usleep(500000);		
	}	
}

void init_socket_server()
{
	struct sockaddr_in s_addr;
	unsigned short port_num=WEB_PORT_NUM;
	service_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == service_socket_descriptor)
	{
			printf("socket fail ! \r\n");
			return -1;
	}
	printf("socket ok !\r\n");

	bzero(&s_addr,sizeof(struct sockaddr_in));
	s_addr.sin_family=AF_INET;
	s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	s_addr.sin_port=htons(port_num);

	if(-1 == bind(service_socket_descriptor,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
			printf("bind fail !\r\n");
			return -1;
	}
	printf("bind ok !\r\n");

	if(-1 == listen(service_socket_descriptor,5))
	{
			printf("listen fail !\r\n");
			return -1;
	}
	printf("listen ok\r\n");	
}

int main()
{

	printf("run display service!\r\n");
	init_socket_server();
	pthread_create(&connect_th, NULL, connect_handler, (void *)NULL);//创建线程，等待client连接请求
	while(1)
	{
		//nothing to do
	}

	close(service_socket_descriptor);
	return 0;
}
