#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include "sckutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
 do \
 { \
    perror(m); \
    exit(EXIT_FAILURE); \
 } while(0)
        
        
#include <signal.h>

void handle(int signum)
{
    int pid = 0;
    printf("recv signum:%d \n", signum);
    
    //避免僵尸进程
    while ((pid = waitpid(-1, NULL, WNOHANG) ) > 0)
    {
        printf("退出子进程pid%d \n", pid);
        fflush(stdout);
    } 
}

int main(void)
{
    int listenfd;
    
    signal(SIGCHLD, handle);
    
    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8001);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    if (listen(listenfd, SOMAXCONN) < 0)
        ERR_EXIT("listen");

    struct sockaddr_in peeraddr;
    socklen_t peerlen;    
    int conn;
    
    while(1)
    {
        int ret = 0;
        int wait_seconds = 5;
        if ((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
        ERR_EXIT("accept");

        printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        
        int pid = fork();
        if (pid == 0)
        {
            char     recvbuf[1024];
            int     recvbuflen = 1024;
            int        datalen;
            close(listenfd);
            while(1)
            {
                memset(recvbuf, 0, sizeof(recvbuf));
                ret = read_timeout(conn, wait_seconds);
                if (ret == 0)
                {
                    datalen = readline(conn, recvbuf, sizeof(recvbuf));
                    if (datalen < 0 )
                    {
                        ERR_EXIT("read func err\n");
                    }
                    if (datalen == 0)
                    {
                        printf("客户端对方已经关闭\n");
                        close(conn);
                        ERR_EXIT("readline return 0");
                    }
                }
                else if (ret == -1 && errno == ETIMEDOUT )
                {
                    printf("读超时\n");
                    continue;
                }
                else if (ret < 0)
                {
                    ERR_EXIT("select 失败\n");
                }
                fputs(recvbuf, stdout);
                
                int wtlen = 0;
                ret = write_timeout(conn,  wait_seconds);
                if (ret == 0)
                {
                    wtlen = writen(conn, recvbuf, datalen);
                    printf("writen : %d\n", wtlen);
                }
                else if (ret == -1 && errno == ETIMEDOUT )
                {
                    printf("写超时\n");
                    continue;
                }
                else if (ret < 0)
                {
                    ERR_EXIT("select 失败\n");
                }
            }
        
        }
        else if (pid > 0)
        {
            close(conn);
        }
    }
    
    return 0;
}