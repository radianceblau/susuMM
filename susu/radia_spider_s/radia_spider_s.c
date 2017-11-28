#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "../../include/radia_comm.h"

#define MAX_RECV_MSG_LEN	1024*1024
#define MAX_SEND_MSG_LEN	1024*10
#define SCREEN_COLOR_RED	0xF800
#define SCREEN_COLOR_GREEN	0x07E0
#define SCREEN_COLOR_BLUE	0x001F

int client_socket_descriptor;
int display_csd;//display_csd;
char recv_msg[MAX_RECV_MSG_LEN], send_msg[MAX_SEND_MSG_LEN];
int now_pv = 0, start_pv = 0, now_sc = 0, start_sc = 0;

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

//0x10 pure
//0x11 ascii
//0x12 word mei
void send_display_msg(int tpye, int x, int y, int multiple, int num, int bg_color, int word_color)
{
	struct st_display_msg send_msg;
	memset(&send_msg, 0, sizeof(struct st_display_msg));
	send_msg.type = tpye;
	send_msg.x = x;
	send_msg.y = y;
	send_msg.multiple = multiple;
	send_msg.num = num;
	send_msg.bg_color = bg_color;
	send_msg.word_color = word_color;		
	send(display_csd, (void *)(&send_msg), sizeof(struct st_display_msg), 0);
}

int get_ip_from_URL(char *url, char *ip)
{
	struct addrinfo *answer, hint, *curr;
	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(url, NULL, &hint, &answer);
	if (ret != 0) {
		printf("getaddrinfo error!\n");
		return -1;
	}
	for (curr = answer; curr != NULL; curr = curr->ai_next)
	{
		inet_ntop(AF_INET,&(((struct sockaddr_in *)(curr->ai_addr))->sin_addr),ip, 16);
		printf("%s\n", ip);
		break;//选取第一个ip
	}
	freeaddrinfo(answer);
	return 1;
}

int get_html()
{
	int recv_byte, total = 0;
	memset(recv_msg, 0, MAX_RECV_MSG_LEN);
	memset(send_msg, 0, MAX_SEND_MSG_LEN);
	sprintf(send_msg, "GET /radianceblau HTTP/1.1\r\n");
	strcat(send_msg, "Host:");
	strcat(send_msg, "blog.csdn.net");
	strcat(send_msg, "\r\n");
	strcat(send_msg, "Connection: keep-alive\r\n");
	strcat(send_msg, "Cache-Control: max-age=0\r\n");
	strcat(send_msg, "Upgrade-Insecure-Requests: 1\r\n");
	strcat(send_msg, "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36\r\n");
	strcat(send_msg, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
	//strcat(send_msg, "Accept-Encoding: gzip, deflate, sdch\r\n");
	strcat(send_msg, "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n");
	//strcat(send_msg, " [truncated]Cookie: bdshare_firstime=1477841873484; uuid_tt_dd=4525944858232572553_20161030; _ga=GA1.2.1351182788.1487849345; __utma=29493984.1351182788.1487849345.1501076952.1501076952.1; __utmz=29493984.1501076952.1.1.utmcsr=(direct)|utm\r\n");
	strcat(send_msg, "\r\n\r\n");
	
	send(client_socket_descriptor, (void *)(&send_msg), strlen(send_msg), 0);
	
	struct timeval timeout = {5,0};  //设置超时时间1秒，0代表秒后面的微秒数，左边这个就是1秒0微秒
	//设置接收超时    
	setsockopt(client_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
	while(1)
	{
		if(-1 == (recv_byte = read(client_socket_descriptor, (void *)(&recv_msg[total]), MAX_RECV_MSG_LEN)))
		{
			printf("read data fail !\r\n");
			//return -1;
			break;
		}
		printf("recv byte:%d\n", recv_byte);
		total += recv_byte;
	}
	//printf("recv:%s\n",recv_msg);
	return 1;
}

//返回指定子字符串在字符串str中的个数
//限制str的长度，不读取至str结尾
int get_nums_str_specified_length(char *str, int len, char *t_str)
{
	char find_flag, find_num = 0;
	int i, j,t_len = strlen(t_str);
	if(len == 0)
		len = strlen(str);
	for(i = 0; i < len; i++)
	{
		find_flag = 0;
		for(j = 0; j < t_len; j++)
		{
			if(str[i + j] != t_str[j])
				break;
			else if(j == t_len -1)
				find_flag = 1;
		}
		if(find_flag == 1)
		{
			find_num++;
		}
	}
	return find_num;
}

//返回str中包含第num个t_str的位置
//限制str的长度，不读取至str结尾
int get_cursor_str_specified_length(char *str, int len, char *t_str, int num)
{
	char find_flag, find_num = 0;
	int i, j,t_len = strlen(t_str);
	if(len == 0)
		len = strlen(str);
	for(i = 0; i < len; i++)
	{
		find_flag = 0;
		for(j = 0; j < t_len; j++)
		{
			if(str[i + j] != t_str[j])
				break;
			else if(j == t_len -1)
				find_flag = 1;
		}
		if(find_flag == 1)
		{
			find_num++;
			if(find_num == num)
				return i;
		}
	}
	return -1;
}

//返回制定属性的html标签在字符串中的起始位置坐标和技术位置坐标
//tag="ul"		只针对有结束标签的tag
//prop="id="blog_rank""
int get_html_tag(char *page_buffer, char *tag, char *prop, int *start, int *end)
{
	int i, j, debug, cursor = 0;
	char start_tag_buffer[100], end_tag_buffer[100];
	int start_tag_start/*<ul*/, start_tag_end/*>*/, end_tag_start/*</ul*/, end_tag_end/*>*/;
	int num, page_buffer_len;

	printf("enter get_html_tag\n");
	page_buffer_len = strlen(page_buffer);
	printf("page_buffer_len:%d\n", page_buffer_len);
	memset(start_tag_buffer, 0 , sizeof(start_tag_buffer));
	memset(end_tag_buffer, 0 , sizeof(end_tag_buffer));
	memcpy(start_tag_buffer, "<", 1);
	memcpy(start_tag_buffer+1, tag, strlen(tag));
	memcpy(start_tag_buffer+1+strlen(tag), " ", 1);// "<ul "
	printf("start_tag_buffer:%s\n", start_tag_buffer);
	memcpy(end_tag_buffer, "</", 2);
	memcpy(end_tag_buffer+2, tag, strlen(tag));
	memcpy(end_tag_buffer+2+strlen(tag), ">", 1);// "</ul>"
	printf("end_tag_buffer:%s\n", end_tag_buffer);
	
	for(i = 0; i < page_buffer_len;)
	{
		printf("i loop :%d   ", i);
		start_tag_start = get_cursor_str_specified_length(&page_buffer[i], 0, start_tag_buffer, 1);//找到tag起始位置
		printf("start_tag_start:%d\n", start_tag_start);
		if(start_tag_start < 0)
			return -1;
		start_tag_end = get_cursor_str_specified_length(&page_buffer[i+start_tag_start] ,0, ">", 1);//找到tag的>位置
		printf("start_tag_end:%d\n", start_tag_end);
		if(start_tag_end <= 0)
			return -2;
		num = get_nums_str_specified_length(&page_buffer[i+start_tag_start+1], start_tag_end, "<");
		printf("< num:%d\n", num);
		if(num > 0)
		{
			debug = get_cursor_str_specified_length(&page_buffer[i+start_tag_start+1], start_tag_end, "<", 1);
			printf("-3:debug:%d %s\n", debug, &page_buffer[i+start_tag_start]);
			return -3;
		}
		num = get_nums_str_specified_length(&page_buffer[i+start_tag_start+1], start_tag_end, prop);//查看这个标签中从<ul  到  >  是否包含目标属性
		cursor = i+start_tag_start+start_tag_end;//光标位于开始标签的结尾
		printf("prop num :%d, cursor:%d:%c\n", num, cursor, page_buffer[cursor]);
		if(num > 0)//属性找到
		{
			int n, nest = 0;//在寻找结束标签时，找到了相同标签的嵌套，应该忽略<div><div></div></div>
			for(j = 0; j < page_buffer_len-cursor;)
			{
				printf("j loop %d\n", j);
				end_tag_start = get_cursor_str_specified_length(&page_buffer[cursor+1+j], 0, end_tag_buffer, 1);
				printf("end_tag_start:%d\n", end_tag_start);
				if(end_tag_start < 0)
					return -4;
				end_tag_end = end_tag_start + strlen(end_tag_buffer);
				printf("end_tag_end:%d\n", end_tag_end);
				n = get_nums_str_specified_length(&page_buffer[cursor+1+j], end_tag_start, start_tag_buffer);
				if(n >= 1)
				{
					nest++;
					j=j+end_tag_end;
				}
				else
					break;					
			}
			if(j >= page_buffer_len-cursor)
				return -5;
			printf("start_tag_start:%d,end_tag_end:%d\n", start_tag_start, end_tag_end);
			*start = start_tag_start + i;
			*end = end_tag_end + cursor;
			break;
		}
		i = i + start_tag_start + start_tag_end + 1;//指向tag结尾的下一个字符
	}
	return 1;
}

void display_pv_sc(int pv, int pv_up, int sc, int sc_up)
{
	int pv_x = 220, pv_y = 180, sc_x = 220, sc_y = 200, mt = 2, offset = 16;
	//send_display_msg(0x10, pv_x, pv_y, mt, 48, 0, 0xffff);//pure
	send_display_msg(0x11, pv_x, pv_y, mt, 48, 0, 0xffff);//P
	send_display_msg(0x11, pv_x+offset, pv_y, mt, 54, 0, 0xffff);//V
	send_display_msg(0x11, pv_x+offset*2, pv_y, mt, 26, 0, 0xffff);//:
	if(pv/100000)
		send_display_msg(0x11, pv_x+offset*3, pv_y, mt, 16+((pv/100000)%100000), 0, 0xffff);//十万
	send_display_msg(0x11, pv_x+offset*4, pv_y, mt, 16+((pv/10000)%10000), 0, 0xffff);//万
	send_display_msg(0x11, pv_x+offset*5, pv_y, mt, 16+((pv/1000)%10), 0, 0xffff);//千
	send_display_msg(0x11, pv_x+offset*6, pv_y, mt, 16+((pv/100)%10), 0, 0xffff);//百
	send_display_msg(0x11, pv_x+offset*7, pv_y, mt, 16+((pv/10)%10), 0, 0xffff);//十
	send_display_msg(0x11, pv_x+offset*8, pv_y, mt, 16+(pv%10), 0, 0xffff);//个
	send_display_msg(0x11, pv_x+offset*9, pv_y, mt, 11, 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, pv_x+offset*10, pv_y, mt, 16+((pv_up/1000)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, pv_x+offset*11, pv_y, mt, 16+((pv_up/100)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, pv_x+offset*12, pv_y, mt, 16+((pv_up/10)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, pv_x+offset*13, pv_y, mt, 16+(pv_up%10), 0, SCREEN_COLOR_RED);//+
	
	send_display_msg(0x11, sc_x, sc_y, mt, 51, 0, 0xffff);//S
	send_display_msg(0x11, sc_x+offset, sc_y, mt, 35, 0, 0xffff);//C
	send_display_msg(0x11, sc_x+offset*2, sc_y, mt, 26, 0, 0xffff);//:
	if(sc/10000)
	send_display_msg(0x11, sc_x+offset*3, sc_y, mt, 16+((sc/10000)%10000), 0, 0xffff);//万
	send_display_msg(0x11, sc_x+offset*4, sc_y, mt, 16+((sc/1000)%10), 0, 0xffff);//千
	send_display_msg(0x11, sc_x+offset*5, sc_y, mt, 16+((sc/100)%10), 0, 0xffff);//百
	send_display_msg(0x11, sc_x+offset*6, sc_y, mt, 16+((sc/10)%10), 0, 0xffff);//十
	send_display_msg(0x11, sc_x+offset*7, sc_y, mt, 16+(sc%10), 0, 0xffff);//个
	send_display_msg(0x11, sc_x+offset*9, sc_y, mt, 11, 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, sc_x+offset*10, sc_y, mt, 16+((sc_up/1000)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, sc_x+offset*11, sc_y, mt, 16+((sc_up/100)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, sc_x+offset*12, sc_y, mt, 16+((sc_up/10)%10), 0, SCREEN_COLOR_RED);//+
	send_display_msg(0x11, sc_x+offset*13, sc_y, mt, 16+(sc_up%10), 0, SCREEN_COLOR_RED);//+
		
	
}

int get_pv_sc(int *fangwen, int *jifen)
{
	int ul_start = 0, ul_end = 0, ret;
	int fangwen_start, fangwen_end, jifen_start, jifen_end;
	char ipstr[16];
	char ul_buffer[1024], fangwen_buffer[100], jifen_buffer[100];

	ret = get_ip_from_URL("www.csdn.net", ipstr);
	if(ret != 1)
	{
		printf("get_ip_from_URL error:%d\n", ret);
		return -1;
	}
	if(open_client_socket(&client_socket_descriptor, ipstr, 80) != 1)		
	{
		printf("open_client_socket error!\n");
	}
	get_html();
	ret = get_html_tag(recv_msg, "ul", "id=\"blog_rank\"", &ul_start, &ul_end);
	printf("get_html_tag ret:%d,ul_start:%d,ul_end:%d\n", ret, ul_start, ul_end);
	if(ret < 0)
	{
		return -1;
	}
	memset(ul_buffer, 0, sizeof(ul_buffer));
	memcpy(ul_buffer, &recv_msg[ul_start], ul_end - ul_start + 1);
	//printf("%s\n", ul_buffer);
	memset(fangwen_buffer, 0 ,sizeof(fangwen_buffer));
	fangwen_start = get_cursor_str_specified_length(ul_buffer, 0, "<span>", 1);
	fangwen_end = get_cursor_str_specified_length(ul_buffer, 0, "</span>", 1);
	memcpy(fangwen_buffer, &ul_buffer[fangwen_start+6], fangwen_end - fangwen_start + 1 - 6 -1 - 3);
	*fangwen = strtol(fangwen_buffer, NULL, 10);
	printf("%d-%d:%d,fangwen:%s\n",fangwen_start, fangwen_end, *fangwen, fangwen_buffer);
	
	memset(jifen_buffer, 0, sizeof(jifen_buffer));
	jifen_start = get_cursor_str_specified_length(ul_buffer, 0, "<span>", 2);
	jifen_end = get_cursor_str_specified_length(ul_buffer, 0, "</span>", 2);
	memcpy(jifen_buffer, &ul_buffer[jifen_start+6], jifen_end - jifen_start + 1 - 6 -1);
	*jifen = strtol(jifen_buffer, NULL, 10);
	printf("%d-%d:%d,jifen:%s\n", jifen_start, jifen_end, *jifen, jifen_buffer);
	return 1;
}

struct tm *getNowTime()
{
	time_t now;		  //实例化time_t结构
	struct tm *timenow;		  //实例化tm结构指针
	time(&now);
	//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow = localtime(&now);
	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
	//printf("Local time is %s/n",asctime(timenow));
	//上句中asctime函数把时间转换成字符，通过printf()函数输出
	return timenow;
}

int update_pv_sc(struct tm *timenow, int force_update_flag)
{
	int ret, update_start_pv_sc_flag, update_pv_start;
	if(timenow->tm_hour == 0 && timenow->tm_min == 0)
		update_start_pv_sc_flag = 1;
	else 
		update_start_pv_sc_flag = 0;
	if(/*timenow->tm_sec == 0*/(timenow->tm_min == 0 || timenow->tm_min == 30))
		update_pv_start = 1;
	else 
		update_pv_start = 0;
	
	if(update_start_pv_sc_flag)//每天0点记录一天的起始pv、sc
	{
		ret = get_pv_sc(&start_pv, &start_sc);
		if(ret < 0)
			return -1;
	}
	if(update_pv_start || force_update_flag)//每小时的0分、30分更新pv，更新显示
	{
		ret = get_pv_sc(&now_pv, &now_sc);
		if(ret < 0)
			return -1;
		display_pv_sc(now_pv, now_pv - start_pv, now_sc, now_sc - start_sc);
	}
	return 1;
}
int main()
{
	int ret, force_flag = 0;
	struct tm *timenow;
	printf("radia spider running!\r\n");
	if(open_client_socket(&display_csd, "127.0.0.1", DISPLAY_PORT_NUM) != 1)		
	{
		printf("open_client_socket error!\n");
	}
	while(1)
	{
		ret = get_pv_sc(&start_pv, &start_sc);
		now_pv = start_pv;
		now_sc = start_sc;
		if(ret > 0)
			break;
		sleep(1);
	}
	display_pv_sc(now_pv, now_pv - start_pv, now_sc, now_sc - start_sc);
	
	while(1)
	{
		timenow = getNowTime();
		ret = update_pv_sc(timenow, force_flag);
		if(ret < 0)
			force_flag = 1;
		else 
			force_flag = 0;
		sleep(43);
	}
	close(client_socket_descriptor);
	close(display_csd);
	return 0;

}