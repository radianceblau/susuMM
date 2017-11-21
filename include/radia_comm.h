#ifndef _RADIA_COMM_H
#define _RADIA_COMM_H

#define DISPLAY_PORT_NUM	9990
#define DISPLAY_INPUT_NUM	9991
#define WEB_PORT_NUM		9992
#define MAX_CLIENT_NUM	100			//此server能够接受的client最大数量
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数
#define MAX_MSG_LEN	10240

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

int get_nums_char(char *str, char c);
int get_cursor_char(char *str, char c, int num);
int open_client_socket(int *csd, char *ip, int port_num);
int creat_server_socket(int *ssd, int port_num);

#endif /* _RADIA_COMM_H */