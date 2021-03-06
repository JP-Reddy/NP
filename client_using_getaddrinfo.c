#include<stdio.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<limits.h>
#include<fcntl.h>
#include<netdb.h>
#include<string.h>

int main(int argc,char *argv[])
{
	// addrinfo *res;
	struct addrinfo *res,*res0,hints;
	int err,sock,file,n,ret;
	char buf[65536];
	if(argc!=4)
	{
		fprintf(stderr,"Usage: %s hostname servicname filename",argv[0]);
		return 1;
	}
	
	file=open(argv[3],O_RDONLY);
	if(file<0)
	{
		perror("open");
		return 1;
	}

	memset(&hints,0,sizeof(hints));
	hints.ai_family=PF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	if((err=getaddrinfo(argv[1],argv[2],&hints,&res0))!=0)
	{
		printf("error %d: %s\n",err,gai_strerror(err));
		return 1;
	}

	for(res=res0;res!=NULL;res=res->ai_next)
	{
		sock=socket(res->ai_family,res->ai_socktype,res->ai_protocol);

		if(sock<0)
		{
			continue;
		}
		if(connect(sock,res->ai_addr,res->ai_addrlen)!=0)
		{
			close(sock);
			continue;
		}
		break;
	}

	freeaddrinfo(res0);

	if(res==NULL)
	{
		printf("Failed\n");
		return 1;
	}

	while((n=read(file,buf,sizeof(buf)))>0)
	{
		ret=write(sock,buf,n);
		if(ret <1)
		{
			perror("Write");
			break;
		}
	}
	close(sock);
	return 1;
}