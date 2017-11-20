#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define DISPLAY_PORT_NUM	9990
#define INPUT_PORT_NUM		9991
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数

//void show_ascii(unsigned int dwOffset_x, unsigned int dwOffset_y, int multiple, int num, short bg_color, short word_color)
struct st_radia_msg  
{  
    int type;			//0x00 led; 0x10 pure,0x11 ascii,0x12 word
	int x;
	int y;
	int multiple;
	int num;
	int bg_color;
	int word_color;
};

int client_socket_descriptor;

int init_socket_client()
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
	s_addr.sin_addr.s_addr= inet_addr("192.168.31.200");//("127.0.0.1");
	s_addr.sin_port=htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n",s_addr.sin_addr.s_addr,s_addr.sin_port);

	if(-1 == connect(client_socket_descriptor,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok !\r\n");
}

int main()
{
	int recbyte;
	struct st_radia_msg send_msg;
	char recv_msg[100];

	printf("radia main running!\r\n");
	init_socket_client();
	while(1)
	{
		memset(&send_msg, 0, sizeof(struct st_radia_msg));
		// send_msg.type = 0x10;
		// send_msg.bg_color = 0x001F;
		send_msg.type = 0x11;
		send_msg.x = 100;
		send_msg.y = 10;
		send_msg.multiple = 5;
		send_msg.num = 44;
		send_msg.bg_color = 0;
		send_msg.word_color = 0xFF00;
		
		send(client_socket_descriptor, (void *)(&send_msg), sizeof(struct st_radia_msg), 0);
		printf("send ok!\n");
		
		memset(&recv_msg, 0, 100);
		if(-1 == (recbyte = read(client_socket_descriptor, (void *)(&recv_msg), 100)))
		{
				printf("read data fail !\r\n");
				return -1;
		}
		printf("recv:%s\n",recv_msg);
		break;
	}

	close(client_socket_descriptor);

	return 0;

}