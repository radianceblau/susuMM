#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "../../include/radia_comm.h"


int client_socket_descriptor;

int main()
{
	int recbyte;
	char recv_msg[100];

	printf("radia main running!\r\n");
	if(open_client_socket(&client_socket_descriptor, "192.168.31.200", INPUT_PORT_NUM) != 1)		
	{
		printf("open_client_socket error!\n");
	}
	while(1)
	{
		/*test for display*/
		// struct st_display_msg send_msg;
		// memset(&send_msg, 0, sizeof(struct st_display_msg));
		// send_msg.type = 0x11;
		// send_msg.x = 100;
		// send_msg.y = 10;
		// send_msg.multiple = 5;
		// send_msg.num = 44;
		// send_msg.bg_color = 0;
		// send_msg.word_color = 0xFF00;		
		// send(client_socket_descriptor, (void *)(&send_msg), sizeof(struct st_display_msg), 0);
		// printf("send ok!\n");
		
		memset(&recv_msg, 0, 100);
		if(-1 == (recbyte = read(client_socket_descriptor, (void *)(&recv_msg), 100)))
		{
				printf("read data fail !\r\n");
				return -1;
		}
		printf("recv:%s\n",recv_msg);
		//break;
	}

	close(client_socket_descriptor);

	return 0;

}