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

#define MAX 65536
void print_raw_header(unsigned short []);
void parse_TCP_header(unsigned char *,int );
void parse_IP_header(unsigned char *,int );
void packet_type(unsigned char *,int );
void process_packet(unsigned char *,int );
void PrintData (unsigned char* , int );
unsigned short checksum(unsigned short [],int );
FILE *logger;
int sockfd;
int count=0;

int main(){
	char *buf;
	socklen_t size;
	int n,i;
	int fd;
	struct sockaddr_in addr;
	buf=(char *)malloc(MAX);

	sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
	printf("Created socket\n");
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
	}
	close(sockfd);
}

void process_packet(unsigned char *buf,int size){
	struct ip *header=(struct ip *) buf,*verify;
	int iph_len=header->ip_hl*4;
	struct tcphdr *tcp_header=(struct tcphdr *)(buf+iph_len);
	struct sockaddr_in addr;
	if(strcmp("127.0.0.1",inet_ntoa(header->ip_src))==0 || strcmp("127.0.0.1",inet_ntoa(header->ip_dst))==0)
		return;
	int n;
	int cnt=0;

	int one = 1;
	const int *val = &one;

	// if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	// {
	//    perror("Error setting IP_HDRINCL");
	//    exit(0);
	// }

	switch(header->ip_p){
		case 1: //ICMP

				break;
		case 2: //IGMP
				break;
		case 6: //TCP
				// parse_TCP_header(buf,size);
				// printf("***********************\n");
				// parse_IP_header(buf,size);
				memset(&addr,0,sizeof(addr));
				addr.sin_family=AF_INET;
				addr.sin_port=tcp_header->dest;
				// addr.sin_addr=
				inet_pton(AF_INET,"192.168.0.5",&addr.sin_addr.s_addr);
				// inet_pton
				header->ip_p=200;
				// header->ip_src=header->ip_dst;
				header->ip_dst=addr.sin_addr;
				header->ip_sum=checksum((unsigned short *)buf,16);
				// header->ip_sum=0;
				// if(strcmp(inet_ntoa(header->ip_dst),inet_ntoa(header->ip_src))==0){
					printf("=======================\n");
					printf("Checksum: %X\n",header->ip_sum);
					printf("Source IP: %s\n",inet_ntoa(header->ip_src));
					printf("Destination IP: %s\n",inet_ntoa(header->ip_dst));
					printf("Identification number: %d\n",ntohs(header->ip_id));
					*(buf + iph_len + tcp_header->doff*4)="Hello";
					if((n=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&addr,sizeof(addr)))<0){
						perror("sendto");
						exit(0);
					}
					// verify=(struct ip *)buf;
					// printf("Protocol used: %d\n",verify->ip_p);
					// printf("Checksum: %X\n",verify->ip_sum);
					PrintData(buf + iph_len + tcp_header->doff*4 , (size - tcp_header->doff*4-iph_len) );
					printf("No. of packets sent: %d\n",++count);
					
				// }
				// printf("******************\n");

				break;
		case 17: //UDP
				break;
		// case 200: printf("No. of packets received: %d\n",++cnt);
				  // break;
		default: break;
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

	// inet_pton(AF_INET,"192.168.0.6",&((header->ip_src).s_addr));


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
	// int i=0;
	// while(i<size)
	// 	printf("%X",buf[i++]);
	header->ip_p=htons(200);
	header->ip_sum=checksum((unsigned short *)buf,16);
	// unsigned short sum;
	// sum=checksum((unsigned short *)buf,16);
	// printf("Sum of IP header: %X\n",sum);
}


unsigned short checksum(unsigned short header[],int type){
	int sum=0,i;
	int num;
	unsigned short x;
	num=160/type;
	switch(type){
		case 8: x=0x00FF;
				break;
		case 12: x=0x0FFF;
				break;
		case 16: x=0xFFFF;
				break;

	}
	for(i=0;i<num;i++){
		// printf("Word %d: %X\n",i,header[i]);
		if(i!=5)
			sum+=(unsigned short)header[i];
	}

	while(sum>>type){
		sum=(sum & x) + (sum>>type);
	}
	sum=~(unsigned short) sum;
	return sum;
}

void parse_TCP_header(unsigned char *buf,int size){
	struct ip *ip_hdr=(struct ip *)buf;
	int iph_len=ip_hdr->ip_hl*4;
	struct tcphdr *header=(struct tcphdr *)(buf+iph_len);

	print_raw_header((unsigned short *)header);
	printf("TCP HEADER\n");
	printf("Source port no: %X\n",ntohs(header->source));
	printf("Destination port no: %X\n",ntohs(header->dest));
	printf("Sequence no: %X\n",header->seq);
	printf("Acknowledgement no: %X\n",header->ack_seq);
	printf("Header length: %X\n",header->doff);
	// printf("Reserved: %X\n",header->res);
	printf("URGENT FLAG: %d\n",header->urg);
	printf("ACKNOWLEDGEMENT FLAG: %d\n",header->ack);
	printf("PUSH FLAG: %d\n",header->psh);
	printf("RESET FLAG: %d\n",header->rst);
	printf("SYN FLAG: %d\n",header->syn);
	printf("FIN FLAG: %d\n",header->fin);
	printf("Window: %X\n",header->window);
	printf("Checksum: %X\n",header->check);
	printf("Urgent pointer: %X\n",header->urg_ptr);

	// header->check = header->check - header->dest + heade
	// header->dest=htons(10000);
}


void print_raw_header(unsigned short header[]){
	int i;
	for(i=0;i<10;i++){
		printf("Word %d: %X\n",i,header[i]);
	}
}



void PrintData (unsigned char* data , int Size)
{
    int i,j; 
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            printf("         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    printf("%c",(unsigned char)data[j]); //if its a number or alphabet
                 
                else printf("."); //otherwise print a dot
            }
            printf("\n");
        } 
         
        if(i%16==0) printf("   ");
            printf(" %02X",(unsigned int)data[i]);
                 
        if( i==Size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++) printf("   "); //extra spaces
             
            printf("         ");
             
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) printf("%c",(unsigned char)data[j]);
                else printf(".");
            }
            printf("\n");
        }
    }
}