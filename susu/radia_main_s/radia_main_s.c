#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "../../include/radia_comm.h"

#define SCREEN_COLOR_RED	0xF800
#define SCREEN_COLOR_GREEN	0x07E0
#define SCREEN_COLOR_BLUE	0x001F

int display_csd;//display_csd;
int ntp_ok_flag = 1;//0代表成功，整数代表失败的次数

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

void show_ntp_state()
{
	short ntp_color;
	if(ntp_ok_flag == 0)
		ntp_color = SCREEN_COLOR_GREEN;
	else
		ntp_color = SCREEN_COLOR_RED;

	send_display_msg(0x11, 200, 0, 4, 46, 0, ntp_color);//N
	send_display_msg(0x11, 200+32, 0, 4, 52, 0, ntp_color);//T
	send_display_msg(0x11, 200+32*2, 0, 4, 48, 0, ntp_color);//P
	send_display_msg(0x11, 200+32*3, 0, 4, 26, 0, ntp_color);//:
	send_display_msg(0x11, 200+32*4, 0, 4, 16+ntp_ok_flag/100, 0, ntp_color);//9
	send_display_msg(0x11, 200+32*5, 0, 4, 16+((ntp_ok_flag/10)%10), 0, ntp_color);//9
	send_display_msg(0x11, 200+32*6, 0, 4, 16+(ntp_ok_flag%10), 0, ntp_color);//9
}
void update_time(struct tm *timenow)
{
	int ret, update_flag;
	if(timenow->tm_hour == 0 && timenow->tm_min == 0 && timenow->tm_sec == 0)
		update_flag = 1;
	else 
		update_flag = 0;
	if(ntp_ok_flag != 0 || update_flag)//每天0点更新，或者失败后每分钟尝试一次
	{
		if(timenow->tm_sec == 0)
		{
			ret = system("ntpdate 1.cn.pool.ntp.org");
			if(ret == 0)
			{
				ntp_ok_flag = 0;//更新成功
			}
			else
			{
				ntp_ok_flag++;//更新失败
				if(ntp_ok_flag >= 999)
					ntp_ok_flag = 999;
			}
			show_ntp_state();
		}
	}
	
}

struct tm *getNowTime()
{
	time_t now;		  //实例化time_t结构
	struct tm *timenow;		  //实例化tm结构指针
	time(&now);
	//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
	timenow = localtime(&now);
	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
	printf("Local time is %s/n",asctime(timenow));
	//上句中asctime函数把时间转换成字符，通过printf()函数输出
	return timenow;
}

int main()
{
	int recbyte;
	struct tm *timenow;

	printf("radia main running!\r\n");
	if(system("/radia/radia_display_s > /dev/null &") == 0)
		printf("start radia_display_s success\n");
	else
		printf("start radia_display_s failed!\n");
	sleep(1);
	if(system("/radia/radia_web_s > /dev/null &") == 0)
		printf("start radia_web_s success\n");
	else
		printf("start radia_web_s failed!\n");
	sleep(1);
	if(system("/radia/radia_spider_s > /dev/null &") == 0)
		printf("start radia_spider_s success\n");
	else
		printf("start radia_spider_s failed!\n");
	sleep(1);
	
	if(open_client_socket(&display_csd, "127.0.0.1", DISPLAY_PORT_NUM) != 1)		
	{
		printf("open_client_socket error!\n");
	}
	send_display_msg(0x12, 0, 0, 3, 0, 0, 0xFF00);//mei
	while(1)
	{
		show_ntp_state();
		if(system("ntpdate 1.cn.pool.ntp.org") == 0)
		{
			ntp_ok_flag = 0;
			show_ntp_state();
			break;
		}
		else
		{
			ntp_ok_flag++;
		}
	}
	while(1)
	{
		timenow = getNowTime();
		update_time(timenow);
		sleep(1);
	}

	close(display_csd);

	return 0;

}