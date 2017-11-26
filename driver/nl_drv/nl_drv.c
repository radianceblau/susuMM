#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#define NETLINK_TEST 25
#define MAX_MSGSIZE 1024
int stringlength(char *s);
int err;
struct sock *nl_sk = NULL;
int flag = 0;
//���û�̬���̻ط���Ϣ
void sendnlmsg(char *message, int pid)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    int slen = 0;
    if(!message || !nl_sk)
    {
        return ;
    }
    printk(KERN_ERR "pid:%d\n",pid);
    skb_1 = alloc_skb(len,GFP_KERNEL);
    if(!skb_1)
    {
        printk(KERN_ERR "my_net_link:alloc_skb error\n");
    }
    slen = stringlength(message);
    nlh = nlmsg_put(skb_1,0,0,0,MAX_MSGSIZE,0);
    NETLINK_CB(skb_1).pid = 0;
    NETLINK_CB(skb_1).dst_group = 0;
    message[slen]= '\0';
    memcpy(NLMSG_DATA(nlh),message,slen+1);
    printk("my_net_link:send message '%s'.\n",(char *)NLMSG_DATA(nlh));
    netlink_unicast(nl_sk,skb_1,pid,MSG_DONTWAIT);
}
int stringlength(char *s)
{
    int slen = 0;
    for(; *s; s++)
    {
        slen++;
    }
    return slen;
}
//�����û�̬��������Ϣ
/*void nl_data_ready(struct sk_buff *__skb)
 {
     struct sk_buff *skb;
     struct nlmsghdr *nlh;
     char str[100];
     struct completion cmpl;
     printk("begin data_ready\n");
     int i=10;
     int pid;
     skb = skb_get (__skb);
	 printk("radia 1 skb->len:%d NLMSG_SPACE(0):%d\n", skb->len, NLMSG_SPACE(0));
     if(skb->len >= NLMSG_SPACE(0))
     {
	 printk("radia 2\n");
         nlh = nlmsg_hdr(skb);
	 printk("radia 3\n");
         memcpy(str, NLMSG_DATA(nlh), sizeof(str));
         printk("Message received:%s\n",str) ;
         pid = nlh->nlmsg_pid;
         while(i--)
        {//����ʹ��completion����ʱ��ÿ3�������û�̬�ط�һ����Ϣ
	 printk("radia 4\n");
            init_completion(&cmpl);
	 printk("radia 5\n");
            wait_for_completion_timeout(&cmpl,3 * HZ);
	 printk("radia 6\n");
            sendnlmsg("I am from kernel!",pid);
        }
         flag = 1;
         kfree_skb(skb);
    }
 }*/
static void nl_data_handler(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	char str[100];
	struct completion cmpl;
	printk("begin data_ready\n");
	int i=10;
	int pid;
	skb = skb_get (__skb);
	printk("radia 1 skb->len:%d NLMSG_SPACE(0):%d\n", skb->len, NLMSG_SPACE(0));

	while (skb->len >= NLMSG_SPACE(0)) {
	 printk("radia 2\n");
         nlh = nlmsg_hdr(skb);
	 printk("radia 3\n");
         memcpy(str, NLMSG_DATA(nlh), sizeof(str));
         printk("Message received:%s\n",str) ;
         pid = nlh->nlmsg_pid;
         while(i--)
        {//����ʹ��completion����ʱ��ÿ3�������û�̬�ط�һ����Ϣ
	 printk("radia 4\n");
            init_completion(&cmpl);
	 printk("radia 5\n");
            wait_for_completion_timeout(&cmpl,3 * HZ);
	 printk("radia 6\n");
            sendnlmsg("I am from kernel!",pid);
        }
         flag = 1;
         kfree_skb(skb);
    }
}

static void nl_data_recv(struct sock *sk, int len)
{
	struct sk_buff *skb;

	while ((skb = skb_dequeue(&sk->sk_receive_queue))) {
		nl_data_handler(skb);
		kfree_skb(skb);
	}
}

// Initialize netlink
int netlink_init(void)
{
    nl_sk = netlink_kernel_create(NETLINK_TEST, 1,
                                 nl_data_recv, NULL, THIS_MODULE);
    if(!nl_sk){
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");
        return 1;
    }
    printk("my_net_link_4: create netlink socket ok.\n");
    return 0;
}
static void netlink_exit(void)
{
    if(nl_sk != NULL){
        sock_release(nl_sk->sk_socket);
    }
    printk("my_net_link: self module exited\n");
}
module_init(netlink_init);
module_exit(netlink_exit);
MODULE_AUTHOR("yilong");
MODULE_LICENSE("GPL");
