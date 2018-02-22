#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
	int sockfd,n;
	char buf[100];
	struct sockaddr_in addr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9515);
	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);
	connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
	memset(buf,0,sizeof(buf));
	n=read(sockfd,buf,sizeof(buf));
	printf("%d, %s\n",n,buf);
}