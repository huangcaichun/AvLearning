//客户端实现思路：
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

void handle(int num)
{
	printf("recv num:%d\n",num);
	exit(0);
}

int main(void)
{
	int	sockfd;
	signal(SIGUSR1, handle);
	
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	//listenfd = socket(AF_INET,  IPPROTO_TCP, 0);
	if (sockfd < 0)
	{
		perror("socket:");
		exit(0);
	}
	
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.114"); //绑定服务器任意一个i
	//inet_aton("127.0.0.1", &servaddr.sin_addr);*/
	
	if ( connect(sockfd, (struct sockaddr *)&servaddr,  sizeof(servaddr)) < 0)
	{
		perror("connect:");
		exit(0);
	} 

	int pid = 0;
	pid = fork();
	
	if (pid > 0) //父进程从键盘接收数据，向父进程写数据
	{
		char sendbuf[1024] = {0};
		while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
		{
			//向服务器写数据
			write(sockfd, sendbuf, strlen(sendbuf));
			memset(sendbuf, 0, sizeof(sendbuf));
			
		}
	}
	else //子进程接收服务器的数据，显示屏幕
	{
		char recvbuf[1024] = {0};
		while (1)
		{
			//从服务器读数据
			int ret = read(sockfd, recvbuf, sizeof(recvbuf));
			if (ret < 0)
			{
				printf("read err\n");
				break;
			}
			if (ret == 0)
			{
				printf("read err\n");
				break;
			}

			//
			fputs(recvbuf, stdout); //从服务端收到数据，打印屏幕

			memset(recvbuf, 0, sizeof(recvbuf));
		}
		close(sockfd);
		kill(getppid(), SIGUSR1);
		exit(0);
	}

 	close(sockfd);
	return 0;
}