#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>

int main(int argc, char *argv[]){
	int sockfd,n;
	struct sockaddr_in addr;

	if(argc!=2){
		fprintf(stderr,"Usage: %s destination_ip",argv[0]);
		return 1;
	}

	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	if(sockfd<0)
	{
		perror("socket");
		return 1;
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(12345);

	inet_pton(AF_INET,argv[1],&addr.sin_addr.s_addr);

	n=sendto(sockfd,"EN MACHA SAMACHARA",18,0,(struct sockaddr *)&addr,sizeof(addr));
	// printf("Sent");
	if(n<1){
		perror("sendto");
		return 1;
	}
	close(sockfd);
	return 1;
}