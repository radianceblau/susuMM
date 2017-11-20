#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

//------------------------------------------------------socket--------------------------------------------------------------------------
#define DISPLAY_PORT_NUM	9990
#define INPUT_PORT_NUM		9991
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数
#define MAX_MSG_LEN	100

struct st_radia_msg  
{  
    char msg_type;  
    char text[MAX_MSG_LEN];  
};

int client_socket_descriptor;
void init_socket_client()
{
	struct sockaddr_in s_addr;
	unsigned short portnum = DISPLAY_PORT_NUM;
	
	client_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == client_socket_descriptor)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");

	bzero(&s_addr,sizeof(struct sockaddr_in));
	s_addr.sin_family=AF_INET;
	s_addr.sin_addr.s_addr= inet_addr("192.168.31.200");
	s_addr.sin_port=htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n",s_addr.sin_addr.s_addr,s_addr.sin_port);

	if(-1 == connect(client_socket_descriptor,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok !\r\n");
}

//--------------------------------------------------------end socket------------------------------------------------------------------------
int main()
{
	int recbyte;
	struct st_radia_msg recv_msg, send_msg;

	printf("radia main running!\r\n");
	init_socket_client();
	while(1)
	{
		memset(&recv_msg.text, 0, MAX_MSG_LEN);
		memset(&send_msg.text, 0, MAX_MSG_LEN);
		//输入数据  
		printf("Enter some text: ");
		fgets(send_msg.text, MAX_MSG_LEN, stdin);
		send_msg.msg_type = 1;
		send(client_socket_descriptor, (void *)(&send_msg), strlen(send_msg.text) + sizeof(char), 0);
		
		if(-1 == (recbyte = read(client_socket_descriptor, (void *)(&recv_msg), sizeof(struct st_radia_msg))))
		{
				printf("read data fail !\r\n");
				return -1;
		}
		printf("recv:%s\n",recv_msg.text);
	}

	close(client_socket_descriptor);

	return 0;

}