#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc,char *argv[])
{
	char *ptr,**pptr;
	char str[INET_ADDRSTRLEN];
	struct hostent *hptr;

	ptr=argv[1];
	if((hptr=gethostbyname(ptr))==NULL)
	{
		// err_msg("gethostbyname error for host %s: %s\n",ptr,hst_error(h_errno));
		return 1;
	}
	printf("Official hostname is %s\n",hptr->h_name);

	for(pptr=hptr->h_aliases;*pptr!=NULL;pptr++)
		printf("Alias %s\n",*pptr);

	for(pptr=hptr->h_addr_list;*pptr!=NULL;pptr++)
	{
		printf("Address: %s\n",inet_ntop(AF_INET,*pptr,str,sizeof(str)));
	}
	return 1;
}