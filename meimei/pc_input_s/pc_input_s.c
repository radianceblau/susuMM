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


#define INPUT_PORT_NUM	9991
#define MAX_CLIENT_NUM	100			//此server能够接受的client最大数量
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数
#define MAX_MSG_LEN	100
struct st_radia_msg  
{  
    long int msg_type;  
    char text[MAX_MSG_LEN];  
};

char client_name_table[MAX_CLIENT_NUM][MAX_CLIENT_NAME_LEN] = {0};//1，每个client需要上报一个name，用于标示自身。2，client_neme_table中为NULL表示该位置的连接资源未被使用。3，client_name_table中的位置确定connect_th表中的位置
pthread_t connect_th, read_event_th, client_th[MAX_CLIENT_NUM];//每个client对用一个线程
char thread_send_flag[MAX_CLIENT_NUM] = {0};
int client_socket_descriptor[MAX_CLIENT_NUM];
int service_socket_descriptor;
int key_event = 0, need_send_flag = 0;

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

void set_thread_flag()
{
	int i;
	for(i = 0; i < MAX_CLIENT_NUM; i++)//当前有多少个client处于连接状态，则相应设置thread_send_flag，以确保所有线程都已经将msg发送出去
	{
		if(client_name_table[i][0] == 0)
			thread_send_flag[i] = 0;
		else
			thread_send_flag[i] = 1;
	}
}

int check_thread_flag()
{
	int i;
	for(i = 0; i < MAX_CLIENT_NUM; i++)
	{
		printf("i:%d,flag:%d\n", i, thread_send_flag[i]);
		if(thread_send_flag[i] != 0)
			return -1;
	}
	return 1;
}

int checksock(int s)
{
	fd_set   fds;
	char buf[2];
	int nbread;
	FD_ZERO(&fds);
	FD_SET(s,&fds);
	if ( select(s+1, &fds, (fd_set *)0, (fd_set *)0, NULL) == -1 ) {
			//log(LOG_ERR,"select(): %s\n",strerror(errno)) ;
			return -1;
	}
	if (!FD_ISSET(s,&fds)) {
			//log(LOG_ERR,"select() returns OK but FD_ISSET not\n") ;
			return -1;
	}       
	/* read one byte from socket */
	nbread = recv(s, buf, 1, MSG_PEEK);
	if (nbread <= 0)
			return -1;
	return 0;
}

void *client_handler(void *pcii)
{
	struct st_radia_msg recv_msg, send_msg;
	int ret, cii = *((int *)pcii);
	int csd = client_socket_descriptor[cii];
	while(1)
	{
		memset(&recv_msg, 0, sizeof(struct st_radia_msg));
		memset(&send_msg, 0, sizeof(struct st_radia_msg));
		if(checksock(csd) == -1)//检测client是否断开
		{
			break;
		}
		else if(need_send_flag)
		{
			printf("client msg :%s\n", recv_msg.text);
			send_msg.msg_type = 1;
			//整理输入事件消息，发送给关心此变化的进程
			//.............
			sprintf(send_msg.text, "1");
			send(csd, (void *)(&recv_msg), strlen(recv_msg.text) + sizeof(long int), 0);
			thread_send_flag[cii] = 0;			
		}
		//ret = read(csd, (void *)(&recv_msg), sizeof(struct st_radia_msg));
		////printf("ret :%d\n", ret);
		//if(ret <= 0)//管理client id，检测client是否断开
		//{
		//	break;
		//}
		//memcpy(client_name_table[]
		usleep(500000);
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

void *read_event_handler(void *arg)
{
	while(1)
	{
		if(1)//read("dev/event0", buf) > 0)
		{
			key_event = 16;
			need_send_flag = 1;
			printf("set need_send_flag 1\n");
		}
		set_thread_flag();
		while(check_thread_flag() == -1);//等待，直至所有thread发送完成
		need_send_flag = 0;
		usleep(1000000);
	}
}

void init_socket_server()
{
	struct sockaddr_in s_addr;
	unsigned short port_num=INPUT_PORT_NUM;
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
	pthread_create(&read_event_th, NULL, read_event_handler, (void *)NULL);//创建线程，扫描相应按键、tp节点
	while(1)
	{
		//nothing to do
	}

	close(service_socket_descriptor);
	return 0;
}
