#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../../include/radia_comm.h"
#include "radia_display_s.h"

//--------------------------------------------------------display------------------------------------------------------------------------
struct fb_var_screeninfo fb_var;
struct fb_fix_screeninfo fb_fix;
char * fb_base_addr = NULL;
int ntp_ok_flag = 0;//0代表成功，整数代表失败的次数

void show_pure_color(unsigned short bg_color)
{
	int i;
    unsigned short *pwAddr = (unsigned short *)(fb_base_addr);
	for(i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		pwAddr[i]= bg_color;
	}
}

/*****************************************
dwOffset_x:x
dwOffset_y:y
multiple:multiple
num:num is the is of word_ascii_code
 bg_color
 word_color
 *****************************************/

void show_ascii(unsigned int dwOffset_x, unsigned int dwOffset_y, int multiple, int num, short bg_color, short word_color)
{
    int i;
	char n_of_short = 0;
	short pix = 0x0000;
	int n_of_pix = 0;
	int multiple_x, multiple_y;
	//printf("radia show num:%d!\n", num);
    unsigned short *pwAddr = (unsigned short *)(fb_base_addr);
	pwAddr += dwOffset_x + dwOffset_y * SCREEN_WIDTH;
	for(i = 0; i < 8; i++)//字摸元素个数
	{
		for(multiple_y = 0; multiple_y < multiple; multiple_y++)
		{
			for(n_of_short = 0 ; n_of_short < 8; n_of_short++)
			{
				if(word_ascii_code[num][i] & (1 << n_of_short))
					pix = word_color;
				else
					pix = bg_color;
				for(multiple_x = 0; multiple_x < multiple; multiple_x++)
				{
					pwAddr[n_of_pix] = pix;
					n_of_pix++;
				}
			}
			n_of_pix += SCREEN_WIDTH-8*multiple;
		}
	}
}
void show_word(unsigned int dwOffset_x, unsigned int dwOffset_y, int multiple, int num, short bg_color, short word_color)
{

    int i;
	short mei_code_16[16];
	char n_of_short = 0;
	short pix = 0x0000;
	int n_of_pix = 0;
	int multiple_x, multiple_y;
	printf("radia show mei!\n");
    unsigned short *pwAddr = (unsigned short *)(fb_base_addr);
	pwAddr += dwOffset_x + dwOffset_y * SCREEN_WIDTH;
	for(i = 0; i < 32/2; i++)//拼接成16位
	{
		mei_code_16[i] = word_mei_code[2*i];
		mei_code_16[i] += word_mei_code[2*i+1] << 8;
	}
	for(i = 0; i < 32/2; i++)//字摸元素个数
	{
		for(multiple_y = 0; multiple_y < multiple; multiple_y++)
		{
			for(n_of_short = 0 ; n_of_short < 16; n_of_short++)
			{
				if(mei_code_16[i] & (1 << n_of_short))
					pix = word_color;
				else
					pix = bg_color;
				for(multiple_x = 0; multiple_x < multiple; multiple_x++)
				{
					pwAddr[n_of_pix] = pix;
					n_of_pix++;
				}
			}
			n_of_pix += SCREEN_WIDTH-16*multiple;
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
	//printf("Local time is %s/n",asctime(timenow));
	//上句中asctime函数把时间转换成字符，通过printf()函数输出
	return timenow;
}

void show_time(struct tm *timenow)
{	
	show_ascii(0, 100, 7, 16+timenow->tm_hour/10, 0, 0xFFFF);//时，十位
	show_ascii(56, 100, 7, 16+timenow->tm_hour%10, 0, 0xFFFF);//时，个位
	show_ascii(56*2, 100, 7, 26, 0, 0xFFFF);//冒号
	show_ascii(56*3, 100, 7, 16+timenow->tm_min/10, 0, 0xFFFF);//分，十位
	show_ascii(56*4, 100, 7, 16+timenow->tm_min%10, 0, 0xFFFF);//分，个位
	show_ascii(56*5, 100, 7, 26, 0, 0xFFFF);//冒号
	//color = (((timenow->tm_sec/2) & 0x1F)<<10);// + (((60-timenow->tm_sec/2) & 0x1F)<<4) + (timenow->tm_sec/2) & 0x1F;
	//printf("color:%d\n", color);
	show_ascii(56*6, 100, 7, 16+timenow->tm_sec/10, 0, 0xFFFF);//秒，十位
	show_ascii(56*7, 100, 7, 16+timenow->tm_sec%10, 0, 0xFFFF);//秒，个位

	

	show_ascii(0, 200, 2, 16+(timenow->tm_year+1900)/1000, 0, 0xFFFF);//年，千位
	show_ascii(16, 200, 2, 16+((timenow->tm_year+1900)/100)%10, 0, 0xFFFF);//年，百位
	show_ascii(16*2, 200, 2, 16+((timenow->tm_year+1900)/10)%10, 0, 0xFFFF);//年，十位
	show_ascii(16*3, 200, 2, 16+(timenow->tm_year+1900)%10, 0, 0xFFFF);//年，个位
	show_ascii(16*4, 200, 2, 14, 0, 0xFFFF);//.
	show_ascii(16*5, 200, 2, 16+(timenow->tm_mon+1)/10, 0, 0xFFFF);//月，十位
	show_ascii(16*6, 200, 2, 16+(timenow->tm_mon+1)%10, 0, 0xFFFF);//月，个位
	show_ascii(16*7, 200, 2, 14, 0, 0xFFFF);//.
	show_ascii(16*8, 200, 2, 16+timenow->tm_mday/10, 0, 0xFFFF);//日，十位
	show_ascii(16*9, 200, 2, 16+timenow->tm_mday%10, 0, 0xFFFF);//日，个位
	
	show_ascii(16*11, 192, 4, 16+timenow->tm_wday, 0, 0xF12F);//星期

}

void init_display()
{
	int fd = 0;
	long int screensize = 0;
	char *path = "/dev/fb0";

	fd = open(path,O_RDWR);
	if (fd <0){
		printf("error opening %s\n", path);
		exit(1);
	}
	/* Get fixed screen information */
	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix)) {
		printf("Error reading fb fixed information.\n");
		exit(1);
	}
	/* Get variable screen information 	*/
	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)) {
		printf("Error reading fb variable information.\n");
		exit(1);
	}
    
	screensize = fb_var.xres * fb_var.yres * fb_var.bits_per_pixel / 8;	
#if (EM86XX_MODE == EM86XX_MODEID_WITHHOST)
	fb_base_addr = (char *)mmap(NULL , screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
#else
	/*uclinux doesn't support MAP_SHARED or MAP_PRIVATE with PROT_WRITE, so no mmap at all is simpler*/
	fb_base_addr = (char *)fb_fix.smem_start;
#endif
	if ((int)fb_base_addr == -1) {
		printf("error mapping fb\n");
		exit(1);
	}
	show_pure_color(0);
}
//--------------------------------------------------------end display------------------------------------------------------------------------

//------------------------------------------------------socket--------------------------------------------------------------------------

char client_name_table[MAX_CLIENT_NUM][MAX_CLIENT_NAME_LEN] = {0};//1，每个client需要上报一个name，用于标示自身。2，client_neme_table中为NULL表示该位置的连接资源未被使用。3，client_name_table中的位置确定connect_th表中的位置
pthread_t connect_th, client_th[MAX_CLIENT_NUM];//每个client对用一个线程
int client_socket_descriptor[MAX_CLIENT_NUM];
int service_socket_descriptor;

void *client_handler(void *pcii)
{
	struct st_display_msg recv_msg;
	char send_msg[100];
	int ret, cii = *((int *)pcii);
	int csd = client_socket_descriptor[cii];
	while(1)
	{
		memset(&recv_msg, 0, sizeof(recv_msg));
		memset(&send_msg, 0, sizeof(send_msg));
		ret = read(csd, (void *)(&recv_msg), sizeof(struct st_display_msg));
		//printf("ret :%d\n", ret);
		if(ret <= 0)//管理client id，检测client是否断开
		{
			break;
		}
		//memcpy(client_name_table[]
		printf("client msg type:0x%x,x:%d,y:%d,multiple:%d,num:%d,bg_color:0x%x,word_color:0x%x\n", 
							recv_msg.type, recv_msg.x, recv_msg.y, recv_msg.multiple, recv_msg.num, recv_msg.bg_color, recv_msg.word_color);
		//根据msg 完成display处理
		//.........
		switch(recv_msg.type)
		{
			case 0x10://pure
				show_pure_color(recv_msg.bg_color);
				break;
			case 0x11://ascii
				show_ascii(recv_msg.x, recv_msg.y, recv_msg.multiple, recv_msg.num, recv_msg.bg_color, recv_msg.word_color);
				break;
			case 0x12://word
				show_word(recv_msg.x, recv_msg.y, recv_msg.multiple, recv_msg.num, recv_msg.bg_color, recv_msg.word_color);
				break;
			case 0x00://led
				break;
			default:
				break;
		}
		sprintf(send_msg, "display!");
		send(csd, (void *)(&send_msg), strlen(send_msg), 0);
		usleep(500000);
	}
	printf("disconnect one, internal id %d\n", cii);
	free_client_internal_id(client_name_table, cii);
	close(csd);
}

void *connect_handler(void *arg)
{
	int addr_len = sizeof(struct sockaddr_in), client_internal_id;	
	struct sockaddr_in c_addr;
	int csd;
	while(1)
	{
		if(get_free_client_internal_id(client_name_table) != 0)
		{
			//printf("will client internal id:%d\n", client_internal_id);
			csd = accept(service_socket_descriptor, (struct sockaddr *)(&c_addr), &addr_len);
			//printf("csd is %d\n", csd);
			if(-1 == csd)
			{
				printf("accept fail !\r\n");
				//free_client_internal_id(client_name_table, client_internal_id);
				continue;
			}
			printf("accept success!!!!!!!csd is %d\n", csd);
			client_internal_id = alloc_client_internal_id(client_name_table);
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
//--------------------------------------------------------end socket------------------------------------------------------------------------

int main()
{
	struct tm *timenow;
	printf("run display service!\r\n");
	init_display();
	if(creat_server_socket(&service_socket_descriptor, DISPLAY_PORT_NUM) != 1)
	{
		printf("creat_server_socket error!\n");
	}
	pthread_create(&connect_th, NULL, connect_handler, (void *)NULL);//创建线程，等待client连接请求
	while(1)
	{
		//clock
		timenow = getNowTime();
		show_time(timenow);
		sleep(1);
	}

	close(service_socket_descriptor);
#if (EM86XX_MODE == EM86XX_MODEID_WITHHOST)
	//munmap(fb_base_addr, screensize);
#endif
	//close(fd);
	return 0;
}
