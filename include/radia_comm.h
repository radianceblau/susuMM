#ifndef _RADIA_COMM_H
#define _RADIA_COMM_H

#define DISPLAY_PORT_NUM	9990
#define INPUT_PORT_NUM		9991
#define WEB_PORT_NUM		9992
#define MAX_CLIENT_NUM	100			//此server能够接受的client最大数量
#define MAX_CLIENT_NAME_LEN 100		//每个client上报name的最大字节数
#define MAX_MSG_LEN	10240

//void show_ascii(unsigned int dwOffset_x, unsigned int dwOffset_y, int multiple, int num, short bg_color, short word_color)
struct st_display_msg  
{  
    int type;			//0x00 led; 0x10 pure,0x11 ascii,0x12 word
	int x;
	int y;
	int multiple;
	int num;
	int bg_color;
	int word_color;
};
//--------------------字符串操作相关--------------------------
//返回制定字符c在字符串str中的个数
int get_nums_char(char *str, char c);

//返回指定字符c在字符串str中第num次出现的位置，如查找失败则返回-1
int get_cursor_char(char *str, char c, int num);

//---------------------socket操作相关---------------------------------
//开启一个socket的client端口
int open_client_socket(int *csd, char *ip, int port_num);

//获取socket server空闲的连接资源数
int get_free_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN]);

//分配一个空闲的socket server
int alloc_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN]);

//释放一个已被占用的socket server连接资源
int free_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN], int id);

//建立一个socket的server端口
int creat_server_socket(int *ssd, int port_num);

//判断socket的client端有没有断开，失败返回-1
int check_socket_connect(int csd);
#endif /* _RADIA_COMM_H */