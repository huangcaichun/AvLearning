//服务器端实现思路：

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
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while (0)

/* Read "n" bytes from a descriptor. */
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ((nread = read(fd, ptr, nleft)) < 0) 
		{
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return -1;
		}
		else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

/* Write "n" bytes to a descriptor. */
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;

	while (nleft > 0) 
	{
		if ((nwritten = write(fd, ptr, nleft)) <= 0) 
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return -1;			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return n;
}

/**
 * recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
 * @sockfd: 套接字
 * @buf: 接收缓冲区
 * @len: 长度
 * 成功返回>=0，失败返回-1
 */
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK); // 设置标志位后读取后不清除缓冲区
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

/* 读到'\n'就返回，加上'\n' 一行最多为maxline个字符 */
ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline;
    int count = 0;

    while (1)
    {
        ret = recv_peek(sockfd, bufp, nleft);
        if (ret < 0)
            return ret; // 返回小于0表示失败
        else if (ret == 0)
            return ret; //返回0表示对方关闭连接了

        nread = ret;
        int i;
        for (i = 0; i < nread; i++)
        {
            if (bufp[i] == '\n')
            {
                ret = readn(sockfd, bufp, i + 1);
                if (ret != i + 1)
                    exit(EXIT_FAILURE);
                
                 return ret + count;
            }
        }
        if (nread > nleft)
            exit(EXIT_FAILURE);
        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if (ret != nread)
            exit(EXIT_FAILURE);

        bufp += nread;
        count += nread;
    }

    return -1;
}

void do_echoser(int conn)
{
    char recvbuf[1024];
    while (1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = readline(conn, recvbuf, 1024);
        if (ret == -1)
            ERR_EXIT("readline error");
        else if (ret  == 0)   //客户端关闭
        {
            printf("client close\n");
            break;
        }

        fputs(recvbuf, stdout);
        writen(conn, recvbuf, strlen(recvbuf));
    }
}

void handle_sigchld(int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
	signal(SIGCHLD,SIG_IGN);
	int	listenfd;
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
		ERR_EXIT("socket");
	
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);
	//servaddr.sin_addr.s_addr = inet_addr(INADDR_ANY); //绑定服务器任意一个ip
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.114"); //绑定服务器任意一个ip。 为什么127.0.0.1拒绝连接？
	//inet_aton("127.0.0.1", &servaddr.sin_addr);*/
	
	int  optval = 1;
	//每个级别SOL_SOCKET下，也有很多选项 不同的选择会有不同的结构
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		ERR_EXIT("setsockopt");

 	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
 		ERR_EXIT("bind");

 	if (listen(listenfd, SOMAXCONN) < 0) 
 		ERR_EXIT("listen");
 	
 	int conn;
 	struct sockaddr_in peeraddr;
 	socklen_t peerlen = sizeof(peeraddr);
	memset(&peeraddr, 0, sizeof(struct sockaddr_in ));
    
	pid_t pid;
    while (1)
    {
		if ((conn = accept(listenfd, (struct sockaddr *)&peeraddr, &peerlen)) < 0)
			ERR_EXIT("accept");

		printf("perradd:%s peerport:%d \n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
		
		//每来一个连接 启动一个子进程
		pid = fork();
		if (pid == -1)
			ERR_EXIT("fork");
		if (pid == 0)
		{
			close(listenfd); //子进程不需要监听
			do_echoser(conn);
			exit(EXIT_SUCCESS); //子进程死，父进程没有调waitpid收尸
		}
		else 
			close(conn); //父进程不需要conn
    }

	return 0;
}