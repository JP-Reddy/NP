#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
int main(int argc,char *argv[])
{
	char *ptr,**pptr;
	struct hostent *hptr;
	char str[INET_ADDRSTRLEN];
	struct sockaddr_in addr;
	ptr=argv[1];
	// memset(hptr,0,sizeof(hptr));
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, ptr, &addr.sin_addr.s_addr);
	hptr=gethostbyaddr((const char *)&addr.sin_addr,sizeof(addr),AF_INET);
	printf("Host name: %s",hptr->h_name);

}