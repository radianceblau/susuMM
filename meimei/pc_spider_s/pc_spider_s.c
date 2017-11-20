#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#define MAX_RECV_MSG_LEN	1024*10
#define MAX_SEND_MSG_LEN	1024

int client_socket_descriptor;
char recv_msg[MAX_RECV_MSG_LEN], send_msg[MAX_SEND_MSG_LEN];

struct addrinfo {
   int              ai_flags;
   int              ai_family;
   int              ai_socktype;
   int              ai_protocol;
   size_t           ai_addrlen;
   struct sockaddr *ai_addr;
   char            *ai_canonname;
   struct addrinfo *ai_next;
};

void init_socket_client()
{
	struct addrinfo *answer, hint, *curr;
	char ipstr[16];
	struct sockaddr_in s_addr;
	unsigned short portnum = 8080;
	
	client_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == client_socket_descriptor)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");
////////////////////getaddrinfo//////////////////////////
#if 0
	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo("www.baidu.com", NULL, &hint, &answer);
	if (ret != 0) {
	fprintf(stderr,"getaddrinfo: &s\n",
	gai_strerror(ret));
	exit(1);
	}
	for (curr = answer; curr != NULL; curr = curr->ai_next) {
	inet_ntop(AF_INET,&(((struct sockaddr_in *)(curr->ai_addr))->sin_addr),ipstr, 16);
	printf("%s\n", ipstr);
	}
	freeaddrinfo(answer);
#endif
////////////////////getaddrinfo//////////////////////////

	bzero(&s_addr,sizeof(struct sockaddr_in));
	s_addr.sin_family=AF_INET;
	s_addr.sin_addr.s_addr= inet_addr("172.16.16.121");//(ipstr);//("172.16.16.121");
	s_addr.sin_port=htons(portnum);
	printf("s_addr = %#x ,port : %#x\r\n",s_addr.sin_addr.s_addr,s_addr.sin_port);

	if(-1 == connect(client_socket_descriptor,(struct sockaddr *)(&s_addr), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok !\r\n");
}

void parse_html(char *msg)
{
   char *str1 = "Borland International", *str2 = "nation", *ptr; 
   ptr = strstr(str1, str2); 
   printf("The substring is: %s", ptr); 
}

int main()
{
	int recbyte;

	printf("radia main running!\r\n");
	parse_html(recv_msg);return;
	init_socket_client();
	while(1)
	{
		memset(recv_msg, 0, MAX_RECV_MSG_LEN);
		memset(send_msg, 0, MAX_SEND_MSG_LEN);
		sprintf(send_msg, "GET / HTTP/1.1\r\n");
		strcat(send_msg, "Host:");
		strcat(send_msg, "fds");
		strcat(send_msg, "\r\n");
		strcat(send_msg, "Accept: */*\r\n");
		strcat(send_msg, "User-Agent: Mozilla/4.0(compatible)\r\n");
		strcat(send_msg, "connection:Keep-Alive\r\n");
		strcat(send_msg, "\r\n\r\n");
		
		send(client_socket_descriptor, (void *)(&send_msg), strlen(send_msg), 0);
		
		struct timeval timeout = {1,0};  //设置超时时间1秒，0代表秒后面的微秒数，左边这个就是1秒0微秒
		//设置接收超时    
		setsockopt(client_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
		if(-1 == (recbyte = read(client_socket_descriptor, (void *)(&recv_msg), MAX_RECV_MSG_LEN)))
		{
				printf("read data fail !\r\n");
				return -1;
		}
		printf("recv:%s\n",recv_msg);
		break;
	}
	close(client_socket_descriptor);
	parse_html(recv_msg);
	return 0;

}