#ifndef _RADIA_COMM_H
#define _RADIA_COMM_H

#define DISPLAY_PORT_NUM	9990
#define INPUT_PORT_NUM		9991
#define WEB_PORT_NUM		9992
#define MAX_CLIENT_NUM	100			//��server�ܹ����ܵ�client�������
#define MAX_CLIENT_NAME_LEN 100		//ÿ��client�ϱ�name������ֽ���
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
//--------------------�ַ����������--------------------------
//�����ƶ��ַ�c���ַ���str�еĸ���
int get_nums_char(char *str, char c);

//����ָ���ַ�c���ַ���str�е�num�γ��ֵ�λ�ã������ʧ���򷵻�-1
int get_cursor_char(char *str, char c, int num);

//---------------------socket�������---------------------------------
//����һ��socket��client�˿�
int open_client_socket(int *csd, char *ip, int port_num);

//��ȡsocket server���е�������Դ��
int get_free_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN]);

//����һ�����е�socket server
int alloc_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN]);

//�ͷ�һ���ѱ�ռ�õ�socket server������Դ
int free_client_internal_id(char (*cnt)[MAX_CLIENT_NAME_LEN], int id);

//����һ��socket��server�˿�
int creat_server_socket(int *ssd, int port_num);

//�ж�socket��client����û�жϿ���ʧ�ܷ���-1
int check_socket_connect(int csd);
#endif /* _RADIA_COMM_H */