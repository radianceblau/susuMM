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
#include "../../include/radia_comm.h"


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

//http://172.16.12.135:9992/?tp=0x11&xx=10&yy=45&mt=3&nu=44&bc=0xFF00&wc=0x1F80
int parse_requestline(char* req_line, struct st_radia_msg *para_msg)
{
	int n, next_n;
	char num_string[20];
	if(strncmp(req_line, "GET", 3) != 0)//本web server只处理GET请求
	{//请求命令格式是否正确
		printf("error request! not GET method\n");
		return -1;
	}
	if(get_nums_char(req_line, '&') != 6)
	{
		printf("error request! error nums of &\n");
		return -1;
	}
	else
	{
		//-----------------------type-----------------------------------
		n = get_cursor_char(req_line, '?', 1);
		//printf("n:%d\n", n);
		next_n = get_cursor_char(req_line, '&', 1);
		//printf("next_n:%d\n", next_n);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		//printf("num_string:%s\n", num_string);
		para_msg->type = strtol(num_string, NULL, 16);
		printf("para_msg->type:0x%x\n", para_msg->type);
		//-----------------------x-----------------------------------
		n = get_cursor_char(req_line, '&', 1);
		next_n = get_cursor_char(req_line, '&', 2);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->x = strtol(num_string, NULL, 10);
		printf("para_msg->x:%d\n", para_msg->x);
		//-----------------------y-----------------------------------
		n = get_cursor_char(req_line, '&', 2);
		next_n = get_cursor_char(req_line, '&', 3);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->y = strtol(num_string, NULL, 10);
		printf("para_msg->y:%d\n", para_msg->y);
		//-----------------------multiple-----------------------------------
		n = get_cursor_char(req_line, '&', 3);
		next_n = get_cursor_char(req_line, '&', 4);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->multiple = strtol(num_string, NULL, 10);
		printf("para_msg->multiple:%d\n", para_msg->multiple);
		//-----------------------num-----------------------------------
		n = get_cursor_char(req_line, '&', 4);
		next_n = get_cursor_char(req_line, '&', 5);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->num = strtol(num_string, NULL, 10);
		printf("para_msg->num:%d\n", para_msg->num);
		//-----------------------bg_color-----------------------------------
		n = get_cursor_char(req_line, '&', 5);
		next_n = get_cursor_char(req_line, '&', 6);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->bg_color = strtol(num_string, NULL, 16);
		printf("para_msg->bg_color:0x%x\n", para_msg->bg_color);
		//-----------------------word_color-----------------------------------
		n = get_cursor_char(req_line, '&', 6);
		next_n = get_cursor_char(req_line, ' ', 2);
		memset(num_string, 0, 20);
		strncpy(num_string, &req_line[n+1+3], next_n-n-1-3);//n+4因为将cursor从?指向0 ex:?tp=0x11
		para_msg->word_color = strtol(num_string, NULL, 16);
		printf("para_msg->word_color:0x%x\n", para_msg->word_color);
	}
	return 1;
}

void *client_handler(void *pcii)
{
	int ret, cii = *((int *)pcii), n;
	int csd = client_socket_descriptor[cii];
	char buffer[1024];
	struct st_radia_msg para_msg;
	//此处模拟http协议，因为http协议是无状态的，因此不保持连接，通信结束立即断开
	memset(&recv_msg, 0, MAX_MSG_LEN);
	ret = read(csd, (void *)(&recv_msg), MAX_MSG_LEN);
	printf("recv:%s\n", recv_msg);
	n = get_cursor_char(recv_msg, '\r', 1);//获取字符串第一行即requestline的位置
	memset(buffer, 0, 1024);
	strncpy(buffer, recv_msg, n+2);
	printf("requestline:%s", buffer);
	if(1 == parse_requestline(buffer, &para_msg))//解析http请求行中参数成功
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

int main()
{
	printf("run display service!\r\n");
	if(creat_server_socket(&service_socket_descriptor, WEB_PORT_NUM) != 1)
	{
		printf("creat_server_socket error!\n");
	}
	pthread_create(&connect_th, NULL, connect_handler, (void *)NULL);//创建线程，等待client连接请求
	while(1)
	{
		//nothing to do
	}

	close(service_socket_descriptor);
	return 0;
}
