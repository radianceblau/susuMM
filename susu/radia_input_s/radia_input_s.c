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
#include <sys/ioctl.h>
#include <errno.h>
#include "../../include/radia_comm.h"


char client_name_table[MAX_CLIENT_NUM][MAX_CLIENT_NAME_LEN] = {0};//1，每个client需要上报一个name，用于标示自身。2，client_neme_table中为NULL表示该位置的连接资源未被使用。3，client_name_table中的位置确定connect_th表中的位置
pthread_t connect_th, read_event_th, client_th[MAX_CLIENT_NUM];//每个client对用一个线程
char thread_send_flag[MAX_CLIENT_NUM] = {0};
int client_socket_descriptor[MAX_CLIENT_NUM];
int input_service_socket_descriptor;
int key_event = 0, need_send_flag = 0;

//return :返回当前已经连接成功的client数量
int set_thread_flag()
{
	int i, num = 0;
	for(i = 0; i < MAX_CLIENT_NUM; i++)//当前有多少个client处于连接状态，则相应设置thread_send_flag，以确保所有线程都已经将msg发送出去
	{
		if(client_name_table[i][0] == 0)
			thread_send_flag[i] = 0;
		else
		{
			thread_send_flag[i] = 1;
			num++;
		}
	}
	return num;
}

int check_thread_flag()
{
	int i;
	for(i = 0; i < MAX_CLIENT_NUM; i++)
	{
		//printf("i:%d,flag:%d\n", i, thread_send_flag[i]);
		if(thread_send_flag[i] != 0)
			return -1;
	}
	return 1;
}


void *client_handler(void *pcii)
{
	//struct st_input_msg send_msg;
	char send_msg[10];
	int ret, cii = *((int *)pcii);
	int csd = client_socket_descriptor[cii];
	while(1)
	{
		memset(&send_msg, 0, 10);
		printf("enter client_handler!\n");
		if(check_socket_connect(csd) == -1)//检测client是否断开
		{
			printf("wiil goout this thread!\n");
			break;
		}
		else if(1)//need_send_flag)
		{
			//整理输入事件消息，发送给关心此变化的进程
			//.............
			sprintf(send_msg, "112");
			printf("radia send_msg:%s\n", send_msg);
			send(csd, (void *)(send_msg), strlen(send_msg), 0);
			thread_send_flag[cii] = 0;			
		}
		usleep(500000);
	}
	printf("disconnect one, internal id %d\n", cii);
	free_client_internal_id(client_name_table, cii);
	close(csd);
}

void *connect_handler(void *arg)
{
	int addr_len = sizeof(struct sockaddr_in), client_internal_id;	
	struct sockaddr_in c_addr;
	int csd;
	while(1)
	{
		if(get_free_client_internal_id(client_name_table) != 0)
		{
			//printf("will client internal id:%d\n", client_internal_id);
			csd = accept(input_service_socket_descriptor, (struct sockaddr *)(&c_addr), &addr_len);
			//printf("csd is %d\n", csd);
			if(-1 == csd)
			{
				printf("accept fail !\r\n");
				//free_client_internal_id(client_name_table, client_internal_id);
				continue;
			}
			printf("accept success!!!!!!!csd is %d\n", csd);
			client_internal_id = alloc_client_internal_id(client_name_table);
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

void *read_event_handler(void *arg)
{
	while(1)
	{
		if(1)//read("dev/event0", buf) > 0)
		{
			key_event = 16;
			need_send_flag = 1;
			//printf("set need_send_flag 1\n");
		}
		if(set_thread_flag() == 0)
			continue;
		while(check_thread_flag() == -1);//等待，直至所有thread发送完成
		need_send_flag = 0;
		usleep(1000000);
	}
}

int main()
{

	printf("run display service!\r\n");
	if(creat_server_socket(&input_service_socket_descriptor, INPUT_PORT_NUM) != 1)
	{
		printf("creat_server_socket error!\n");
	}
	pthread_create(&connect_th, NULL, connect_handler, (void *)NULL);//创建线程，等待client连接请求
	pthread_create(&read_event_th, NULL, read_event_handler, (void *)NULL);//创建线程，扫描相应按键、tp节点
	while(1)
	{
		//nothing to do
	}

	close(input_service_socket_descriptor);
	return 0;
}
