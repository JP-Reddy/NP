#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

#define MAX 65536
void print_raw_header(unsigned short []);
void parse_TCP_header(unsigned char *,int );
void parse_IP_header(unsigned char *,int );
void packet_type(unsigned char *,int );
void process_packet(unsigned char *,int );
unsigned short checksum(unsigned short [],int );
FILE *logger;
int count=0;

int main(){
	char *buf;
	int sockfd;
	socklen_t size;
	int n,i;
	int fd;
	struct sockaddr_in addr;
	buf=(char *)malloc(MAX);
	logger=fopen("receiver_logs.txt","w");
	// addr.sin_family=AF_INET;
	// addr.sin_port=htons(10000);
	// addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
	printf("Created socket\n");

	// if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr))<0){
	// 	perror("bind");
	// 	exit(0);
	// }
	// printf("Binded to port 10000");
	n=1;
	while(n){
		size=sizeof(addr);
		n=recvfrom(sockfd,buf,MAX,0,(struct sockaddr *)&addr,&size);
		if(n<0){
			perror("recvfrom");
			exit(0);
		}
		if(n>0){
			process_packet(buf,n);
		}
		// sleep(1000);
	}
	close(sockfd);
}

void process_packet(unsigned char *buf,int n){
	struct ip* header=(struct ip *)buf;
	if(header->ip_p==6){
	if(strcmp(inet_ntoa(header->ip_dst),inet_ntoa(header->ip_src))==0){
		printf("=======================\n");
		printf("Source IP: %s\n",inet_ntoa(header->ip_src));
		printf("Destination IP: %s\n",inet_ntoa(header->ip_dst));
		// if((n=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr)))<0){
		// 	perror("sendto");
		// 	exit(0);
		// }
		// verify=(struct ip *)buf;
		printf("No. of packets received: %d\n",++count);
		// printf("Protocol used: %d\n",verify->ip_p);
		// printf("No. of packets sent: %d\n",++count);
		
	}

	// 	parse_IP_header(buf,n);
	// 	printf("*****************\n");
	}
}


void parse_IP_header(unsigned char *buf,int size){
	struct ip *header=(struct ip *)buf;
	// uint32_t ip;
	struct in_addr ip;
	struct sockaddr_in source,destn;
	memset(&ip,0,sizeof(ip));
	source.sin_addr=header->ip_src;
	destn.sin_addr=header->ip_dst;

	printf("Version: %d\n",header->ip_v);
	printf("Header length: %d\n",header->ip_hl);
	printf("Type of service: %d\n",header->ip_tos);
	printf("Total length: %X\n",header->ip_len);
	printf("Identification number: %X\n",header->ip_id);
	printf("Sequence number: %X\n",header->ip_off);
	printf("Time to live: %d\n",header->ip_ttl);
	printf("Protocol: %X\n",header->ip_p);
	printf("Checksum: %X\n",header->ip_sum);
	printf("Source IP: %s\n",inet_ntoa(source.sin_addr));
	printf("Destination IP: %s\n",inet_ntoa(destn.sin_addr));


	// inet_pton(AF_INET,"192.168.0.6",&((header->ip_src).s_addr));

	fprintf(logger,"Version: %d\n",header->ip_v);
	fprintf(logger,"Header length: %d\n",header->ip_hl);
	fprintf(logger,"Type of service: %d\n",header->ip_tos);
	fprintf(logger,"Total length: %X\n",header->ip_len);
	fprintf(logger,"Identification number: %X\n",header->ip_id);
	fprintf(logger,"Sequence number: %X\n",header->ip_off);
	fprintf(logger,"Time to live: %d\n",header->ip_ttl);
	fprintf(logger,"Protocol: %X\n",header->ip_p);
	fprintf(logger,"Checksum: %X\n",header->ip_sum);
	fprintf(logger,"Source IP: %s\n",inet_ntoa(source.sin_addr));
	fprintf(logger,"Destination IP: %s\n",inet_ntoa(destn.sin_addr));
	// int i=0;
	// while(i<size)
	// 	printf("%X",buf[i++]);
	// unsigned short sum;
	// sum=checksum((unsigned short *)buf,16);
	// printf("Sum of IP header: %X\n",sum);
}


