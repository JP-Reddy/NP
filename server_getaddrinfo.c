#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <limits.h>

int main(int argc,char *argv[]) 
{
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	int err,sockfd,connfd,ret,n,file;
	struct sockaddr_in client;
	struct addrinfo hints,*res;
	socklen_t len;
	char buf[65536];
	if(argc!=2)
	{
		fprintf(stderr, "Usage: %s outputfilename\n",argv[0] );
		return 1;
	}
	file=open(argv[1],O_WRONLY | O_CREAT, 0600);

	if(file<0)
	{
		perror("Open");
		return 1;
	}

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	err=getaddrinfo(NULL,"12345",&hints, &res);

	if(err!=0)
	{
		printf("gettaddrinfo: %s\n",gai_strerror(err));
		return 1;
	}

	sockfd=socket(res->ai_family,res->ai_socktype,0);

	if(sockfd<0)
	{
		perror("Socket");
		return 1;
	}

	if(bind(sockfd,res->ai_addr,res->ai_addrlen)!=0)
	{
		perror("Bind");
		return 1;
	}
	freeaddrinfo(res);
	listen(sockfd,5);

	// for(;;)
	{
		len=sizeof(client);
		connfd=accept(sockfd,(struct sockaddr *)&client,&len);
		if(connfd<0)
		{
			perror("accept");
			return 1;
		}

		while((n=read(connfd,buf,sizeof(buf)))>0)
		{
			ret=write(file,buf,n);
			if(ret<1)
			{
				perror("write");
				break;
			}
		}
		close(connfd);
	}
	close(sockfd);
	return 0;
}
