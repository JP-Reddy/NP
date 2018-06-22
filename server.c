#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>

int main()
{
	int sock0,connfd;
	struct sockaddr_in addr,client;
	socklen_t len;
	sock0=socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9515);
	addr.sin_addr.s_addr=INADDR_ANY;
	bind(sock0, (struct sockaddr *) &addr,sizeof(addr));
	listen(sock0,5);
	for(;;)
	{
		len=sizeof(client);
		connfd=accept(sock0,(struct sockaddr *) &client,&len);
		write(connfd,"HELL",4);
		close(connfd);
	}
	close(sock0);
}