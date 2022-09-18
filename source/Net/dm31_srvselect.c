
//标准C库
#include <stdio.h>
#include <string.h>

//系统调用
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)
#define MAXLINE 80                 /*宏定义 MAXLINE 为 80*/
#define SERV_PORT 8000           /*宏定义 端口号为 8000*/

int main(int argc, char **argv)
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;                                  /*定义fd_set(文件描述符的集合)型变量 rset allset*/
    char buf[MAXLINE];                               /*定义缓冲区数组buf[ ]*/
    char str[INET_ADDRSTRLEN];                /*定义字符数组str[ ]*/
    socklen_t cliaddr_len;
    struct sockaddr_in    cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);       /*打开一个网络通讯端口，地址类型IPV4，TCP协议*/


    bzero(&servaddr, sizeof(servaddr));                        /*servaddr 初始化*/
    servaddr.sin_family      = AF_INET;                        /*设置地址类型为 IPV4*/
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);         /*设置网络地址为本地任意IP地址*/

    servaddr.sin_port = htons(SERV_PORT); /*定义端口号 SERV_PORT(8000)*/


    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));                                /*调用bind将参数listenfd与servaddr绑定，使listenfd监听servaddr所描述的地址和端口*/

    listen(listenfd, 20);     /*listen()声明listenfd处于监听状态，并且最多允许有backlog个客户端处于连接待状态，0—成功，-1—失败*/

    maxfd = listenfd;        /*将maxfd初始化*/
    maxi = -1;              /*将maxi赋初值为-1*/
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;                             /*将client[i]中的每一个元素赋初值为-1*/
    FD_ZERO(&allset);            /* 将文件描述符集合allset置零*/
    FD_SET(listenfd, &allset);     /*将监听文件描述符listenfd放入集合allset中*/

    while (1)
    {         
        rset = allset;   
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);/* 调用select函数它可以同时监听多个阻塞的文件描述符,并且处理有数据到达的文件描述符,并返回更新的个数给nready,没有数据到达就阻塞*/
        if (nready == -1)
        {
            //perr_exit("select error");     /*当nready小于0时出错* /
            if (errno == EINTR)
                continue;
            ERR_EXIT("select");
        }
        if (nready == 0)
            continue;
        if (FD_ISSET(listenfd, &rset)) { /*检测文件描述符listenfd是否在rset集合中（检测rset中是否有新的连接出现）*/
            cliaddr_len = sizeof(cliaddr); /*对cliaddr_len赋初值，并将本地客户端地址的长度放入变量cliaddr_len中*/
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);  /*服务器调用accept()接受连接，若没有客户端的连接请求，就阻塞等待直到有客户端连接上来；若有客户端连接则解除阻塞状态,并返回一个服务器与客户端互相通信的文件描述符connfd*/

            printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                    ntohs(cliaddr.sin_port)); /*打印连接的客户端地址和端口号*/

            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0) {
                    client[i] = connfd; /*将已连接到服务器的客户端的文件描述符放入数组client[i]中*/
                    break;
                }
            if (i == FD_SETSIZE) {
                fputs("too many clients\n", stderr);/*如果i == FD_SETSIZE表示连接已满,则输出错误信息" too many clients "*/
                exit(1);     /*异常退出*/
            }

            FD_SET(connfd, &allset); /*将新的文件描述符connfdt添加到allset中 */
            if (connfd > maxfd)
                maxfd = connfd; /*如果connfd > maxfd,则将connfd的值赋给maxfd，使得maxfd存放的是最大的文件描述符*/
            if (i > maxi)
                maxi = i;    /*如果i > maxi,将i的值赋给maxi，使得maxi存放的是值最大的i*/

            if (--nready == 0)
                continue;                                    /*当nready为0,没有可读的文件描述符,则结束本次循环。并开始执行下一次循环*/

        }
        for (i = 0; i <= maxi; i++) 
        {
            /* check all clients for data */
            if ((sockfd = client[i]) < 0)
                continue;                                         /*如果没有连接,则结束本次循环，并开始执行下一次循环*/
            if (FD_ISSET(sockfd, &rset))         /*检测文件描述符listenfd是否在rset集中*/
            {
                if ( (n = read(sockfd, buf, MAXLINE)) == 0)                 /*判断客户端连接是否关闭*/
                {                           
                    close(sockfd);                                        /*如果客户端连接关闭,则服务器端连接关闭*/
                    FD_CLR(sockfd, &allset);                /*sockfd的将文件描述符从allset中清除*/
                    client[i] = -1;
                } else {                                /*若客户端连接没有关闭*/
                    int j;
                    for (j = 0; j < n; j++)
                        buf[j] = toupper(buf[j]);                /*将buf[j]中的小写字母全部转换为大写字母，并存入buf[j]中*/
                    write(sockfd, buf, n);                  /*调用write将处理结果发给客户端*/
                }
                if (--nready == 0)
                    break;    /*当nready为0,没有可读的文件描述符,结束本次循环*/
            }
        }
    }
}