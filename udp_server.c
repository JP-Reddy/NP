#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
// #include<arpa/inet.h>

int main(int argc,char *argv[]){
	socklen_t len;
	struct sockaddr_in client,addr;
	int sockfd,n;
	char buf[2048];

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0){
		perror("socket");
		return 1;
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(12345);
	addr.sin_addr.s_addr=INADDR_ANY;

	bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));

	len=sizeof(client);
	// printf("Receiving\n");
	n=recvfrom(sockfd,buf,sizeof(buf)-1,0,(struct sockaddr *)&client,&len);
	// printf("Received\n");
	write(fileno(stdout),buf,n);

	close(sockfd);
	return 1;
}