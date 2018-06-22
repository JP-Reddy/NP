#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<pcap.h>
#include<string.h>
#include<ctype.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<arpa/inet.h>
#include<time.h>
#include<unistd.h>

#define MAX_PACKET_SIZE 65535

void packet_view (u_char * args, const struct pcap_pkthdr *h,
		  const u_char * data);
void forward (u_char *, int);
uint16_t udp_checksum (const void *buff, size_t len, in_addr_t src_addr,
		       in_addr_t dest_addr);

FILE *f;
int pckt_cnt = 1, packets = 0;
char source_ip[15], dest_ip[15], mess[15];


int
main (int argc, char *argv[])
{
  strcpy (source_ip, argv[1]);
  strcpy (dest_ip, argv[2]);
  strcpy (mess, argv[3]);
  f = fopen ("logfile", "w+");
  if (f == NULL)
    puts ("Error opening logfile");
  int i, count = 1000;
  char *dev, errmsg[PCAP_ERRBUF_SIZE];
  const u_char *data;
  pcap_t *pcap_handler;
  struct pcap_pkthdr h;
  struct bpf_program bpf;
  dev = pcap_lookupdev (errmsg);
  if (strlen (dev) == 0)
    {
      puts ("pcap_lookupdev error!");
      fprintf (f, "Error: %s", errmsg);
      return -1;
    }
  memset (errmsg, 0, sizeof (errmsg));
  if ((pcap_handler =
       pcap_open_live (dev, MAX_PACKET_SIZE, 0, 512, errmsg)) == NULL)
    {
      fprintf (f, "pcap_open_live error: %s\n", errmsg);
      return -1;
    }
  char *filter = "udp";
  if (pcap_compile (pcap_handler, &bpf, filter, 0, PCAP_NETMASK_UNKNOWN) < -1)
    {
      pcap_perror (pcap_handler, "Compile");
      exit (0);
    }
  if (pcap_setfilter (pcap_handler, &bpf) == -1)
    pcap_perror (pcap_handler, "Setfilter");

  puts ("Writing to logfile...");

  while (count--)
    pcap_dispatch (pcap_handler, 5, packet_view, NULL);

  printf ("%d udp packets captured and forwarded\n", pckt_cnt);

  return 0;
}

void
packet_view (u_char * args, const struct pcap_pkthdr *h, const u_char * data)
{
  //FILE *f = fopen("./logfile","w+");
  //puts("SD");
  int i, j;
  struct ip *ip = (struct ip *) (data + 14);
  struct udphdr *udp = (struct udphdr *) (ip + 1);
  u_char source[15], dest[15];
  u_int32_t *src = (u_int32_t *) (data + 26), *dst =
    (u_int32_t *) (data + 30);
  inet_ntop (AF_INET, src, source, sizeof (source));
  inet_ntop (AF_INET, dst, dest, sizeof (dest));
  fprintf (f, "Source -> ip: %s port: %d\t\tDestination -> ip: %s port: %d\n",
	   source, htons (udp->uh_sport), dest, htons (udp->uh_dport));
  fprintf (f, "Packet receicved at timestamp:%lu with length %d\n\n",
	   h->ts.tv_sec, h->caplen);
  for (i = 0; i < h->len; i++)
    {
      fprintf (f, "%02x", data[i]);
      if (i % 2 == 1)
	fprintf (f, " ");
    }
  fprintf (f, "\n***************Data************\n");
  for (i = 0; i < h->len; i++)
    {
      if (isprint (data[i]))
	fprintf (f, "%c", data[i]);
      else
	fprintf (f, ".");
      if (i % 16 == 0 || i == h->len - 1);
      // fprintf(f,"\n");
    }
  forward (data + 14, h->len - 14);
  fprintf (f, "\n\n");
  return;
}

void
forward (u_char * data, int len)
{

  char buf[1500], add[15];
  strcpy (add, mess);
  memcpy (buf, data, len);
  memcpy (buf + len, add, strlen (add));
  struct sockaddr_in addr;

  //FILE *f = fopen("./logfile","w+");
  u_int32_t *src = (u_int32_t *) (buf + 12), *dst = (u_int32_t *) (buf + 16);
  struct ip *ip = (struct ip *) buf;
  struct udphdr *udp = (struct udphdr *) (ip + 1);
  int sockfd, on = 1;
  int *val = &on;
  char dest[15];
  sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sockfd < 1)
    {
      fputs ("Socket error!", f);
      return;
    }
  if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0)
    {
      fputs ("setsockopt IP_HDRINCL error!", f);
      return;
    }
  addr.sin_family = AF_INET;
  //puts(dest_ip);
  addr.sin_addr.s_addr = inet_addr (dest_ip);
  if (*dst == inet_addr (source_ip))
    {
      //puts("Am i there?");
      inet_ntop (AF_INET, dst, dest, sizeof (dest));
      printf ("Forwarded %d packets\r", packets++);
      //ip->ip_src.s_addr = *src;
      ip->ip_src.s_addr = ip->ip_dst.s_addr;
      ip->ip_dst.s_addr = inet_addr (dest_ip);	//*dst;
      ip->ip_sum = 0;		/*kernel fills this */
      udp->check =
	udp_checksum (buf + (ip->ip_hl << 2), len - (ip->ip_hl << 2),
		      ip->ip_src.s_addr, ip->ip_dst.s_addr);
      char *extra = data + len;
      extra = "Preetham";
      if (sendto
	  (sockfd, buf, len + strlen (add), 0, (struct sockaddr *) &addr,
	   sizeof (addr)) < 0)
	{
	  fputs ("sendto error!", f);
	}
      else
	fprintf (f, "\n\nPacket forwarded succesfully at timestamp %lu!\n",(unsigned long) time (0));
      fprintf (f,"\n------------Packet %d------------------",pckt_cnt++);
    }
  close (sockfd);

  return;
}

uint16_t
udp_checksum (const void *buff, size_t len, in_addr_t src_addr,
	      in_addr_t dest_addr)
{
  const uint16_t *buf = buff;
  uint16_t *ip_src = (void *) &src_addr, *ip_dst = (void *) &dest_addr;
  uint32_t sum;
  int length = len;
  sum = 0;
  while (len > 1)
    {
      sum += *buf++;
      if (sum & 0x80000000)
	sum = (sum & 0xFFFF) + (sum >> 16);
      len -= 2;
    }
  if (len & 1)
    sum += *((uint8_t *) buf);
  sum += *(ip_src++);
  sum += *ip_src;
  sum += *(ip_dst++);
  sum += *ip_dst;
  sum += htons (IPPROTO_UDP);
  sum += htons (length);
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return ((uint16_t) (~sum));
}
