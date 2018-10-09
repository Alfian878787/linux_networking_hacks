#include    <uma.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <features.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <net/if.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include	<linux/ip.h>
#include	<linux/tcp.h>
#include	<sys/time.h>
#include <linux/libc-compat.h>
 #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>

#define DATA_SIZE 1024
#define SRC_MAC "dc:fe:07:00:fe:cc"
#define DEST_MAC "c0:3f:d5:ff:52:3b"
#define SRC_IP "172.16.5.151"
//#define DEST_IP "172.16.5.245"
#define DEST_PORT 80
#define SRC_PORT 100
int parts ;
FILE *fp;

void printinhex( char *message , unsigned char *packet , int len ); 

typedef struct s_header {

	unsigned long int source_ip ;
	unsigned long int dest_ip ;
	unsigned char reserved ;
	unsigned short int tcp_length ;

}ps_header ;


int createrawsocket ( int protocal_to_sniff) 
{
	int rawsocket ;

	if (  ( rawsocket = socket ( PF_PACKET , SOCK_RAW , htons(protocal_to_sniff) ) ) == FAILED ) {

		perror("socket");

		exit ( EXIT_FAILURE ) ;

	}

	return rawsocket ;

}

int bindrawsockettointerface ( char *device , int raw , int protocal ) 
{
	struct sockaddr_ll sll ;
	struct ifreq ifr ;

	bzero(&sll , sizeof (sll) );
	bzero(&ifr , sizeof (ifr) );

	strncpy( (char*) ifr.ifr_name , device , IFNAMSIZ );

	if (( ioctl ( raw , SIOCGIFINDEX , &ifr ) ) == FAILED ) {

		perror("getting interface");

		exit ( EXIT_FAILURE );

	}

	sll . sll_family = AF_PACKET ;

	sll.sll_ifindex = ifr.ifr_ifindex ;

	sll.sll_protocol = htons ( protocal );

	if (( bind ( raw , (struct sockaddr *)&sll , sizeof (sll))) ==FAILED){

		perror("bind");

		exit ( EXIT_FAILURE );
	}

}

int sendrawpacket ( int rawsock , unsigned char* pkt , int pkt_len )
{
	int sent = 0 ;

	if (( sent = write ( rawsock , pkt , pkt_len ) ) != pkt_len )

	{

		return 0;

	}

	return 1 ;

}

unsigned char *createethernetheader ( char* src_mac , char* dest_mac , int protocol )
{
	struct ethhdr *ethernet_header ;

	ethernet_header = ( struct ethhdr *)malloc ( sizeof ( struct ethhdr ) ) ;

	memcpy ( ethernet_header -> h_source , (void*) ether_aton ( src_mac ) , 6 );

	memcpy ( ethernet_header -> h_dest , (void*) ether_aton ( dest_mac ) , 6 );

	ethernet_header -> h_proto = htons ( protocol ) ;

	return ((unsigned char*)ethernet_header) ;

}

unsigned short computechecksum ( unsigned char* header , int len ) 
{
	long sum = 0 ;

	unsigned short *ip_header = (unsigned short*)header ;

	while ( len > 1 ) {

		sum +=  *((unsigned short *) ip_header  ) ;

		ip_header ++ ;

		if (sum & 0x80000000)

			sum = (sum & 0xFFFF) ;

		len -= 2 ;

	}

	if ( len )

		sum += (unsigned short) * (unsigned char* ) ip_header ;

	while ( sum >> 16 )

		sum = ( sum & 0xFFFF ) + ( sum >> 16 );

	return ~sum ;

}

unsigned char* createipheader ( char *v )
{
	struct iphdr *ip_header ;

	ip_header = (struct iphdr*) malloc ( sizeof (  struct iphdr  ) ) ;

	ip_header -> version = 4 ;

	ip_header -> ihl = ( sizeof (  struct iphdr  ) / 4 );

	ip_header -> tos = 0 ;

	ip_header -> tot_len = htons ( sizeof (  struct iphdr  ) );

	ip_header -> id = htons ( 111 ) ;

	ip_header -> frag_off = 0 ;

	ip_header -> ttl = 111 ;

	ip_header -> protocol  = 1 /*IPPROTO_TCP */ ;

	ip_header -> check = 0 ;

	(ip_header -> saddr) = inet_addr ( SRC_IP ) ; 

	(ip_header -> daddr) = inet_addr ( v ) ; 

	ip_header -> check =  computechecksum ( (unsigned char*) ip_header , ip_header -> ihl * 4 );

	return (( unsigned char* )ip_header );

}

struct tcphdr *createtcpheader ( ) 
{	
	struct tcphdr *tcp_header ;

	tcp_header = (struct tcphdr*) malloc ( sizeof ( struct tcphdr ));

	tcp_header -> source = htons ( SRC_PORT );

	tcp_header -> dest = htons ( DEST_PORT );

	tcp_header -> seq = htons ( 111 );

	tcp_header -> ack_seq = htons ( 111 );

	//tcp_header -> resl = 0 ;

	tcp_header -> doff =  (sizeof ( struct tcphdr ) / 4 ) ;

	tcp_header -> syn = 1 ;

	tcp_header -> window  = htons ( 100 ) ;

	tcp_header -> check  = 0 ;

	tcp_header -> urg_ptr = 0 ;

	return tcp_header ;

}

void createsudoheaderandcalculatetcpchecksum( struct tcphdr *tcp_header , struct iphdr *ip_header , unsigned char* data )
{
	int segment_len = ntohs ( ip_header -> tot_len ) - ip_header -> ihl * 4 ;

	int header_len = sizeof ( ps_header ) + segment_len ;

	unsigned char *hdr = (unsigned char*)malloc ( header_len );

	ps_header *psheader = (ps_header*) hdr ;

	psheader -> source_ip = ip_header -> saddr ;

	psheader -> dest_ip = ip_header -> daddr ;

	psheader -> reserved = 0 ;

	psheader -> tcp_length = htons (segment_len);

	memcpy ( ( hdr + sizeof ( ps_header ) + tcp_header -> doff * 4 ), data , DATA_SIZE );

	tcp_header -> check = computechecksum ( hdr , header_len ) ;

	free ( hdr );
}

char* create_data_to_add ( char *name )
{
	char buffer[1100] = "" ;

	char* data ;

	if ( ! fread ( buffer , 1 , 1024 , fp ) ) {

		if ( ! fread ( buffer , 1 , parts  , fp ) )

			return NULL ;

	}

	data = malloc ( strlen ( buffer ) + 8 ) ;

	buffer [ strlen ( buffer ) + 1 ] = '\0' ;

	if ( NULL == data ) {

		perror("malloc:");

		exit ( EXIT_FAILURE ) ;

	}

	memset ( data , '\0' , malloc_usable_size ( data ) ) ;

	memcpy ( data , buffer , strlen ( buffer ) ) ;

	return data ;

}

int main ( int c , char *v[]) 
{
	int raw ;

	unsigned char *packet  ;

	struct ethhdr *ethernet_header  ;

	struct iphdr *ip_header  ;

	struct tcphdr *tcp_header;

	unsigned char* data_to_add ;

	int pkt_len = 0; 

	struct stat inst ;

	int condition = 1 ;

	if ( c != 4 ) {

		printf("usage: %s <interface> <ip address> <file to send>",v[0]);

		exit ( EXIT_FAILURE );

	}

	raw = createrawsocket ( ETH_P_ALL );

	bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

	ethernet_header = createethernetheader ( SRC_MAC , DEST_MAC , ETHERTYPE_IP ) ;

	ip_header = createipheader (v[2]) ;

	tcp_header = createtcpheader ( ) ;

	fp = fopen ( v[3] , "r" ) ;

	stat ( v[2] , &inst ) ;

	parts = inst. st_size % 1024 ;

	while ( condition ) {

		data_to_add = create_data_to_add ( v[3] ) ;

		if ( data_to_add == NULL ) {

			condition = 0 ;

			data_to_add = malloc ( 1 ) ;

			memset ( data_to_add , '\0' , malloc_usable_size ( data_to_add ) ) ;

		}

		createsudoheaderandcalculatetcpchecksum( tcp_header , ip_header , data_to_add );

		pkt_len = sizeof ( struct ethhdr ) + ntohs ( ip_header -> tot_len ) + 1100 ;

		printf ("%d\n",pkt_len) ;

		packet = (unsigned char*) malloc ( pkt_len ) ;

		memcpy( packet , ethernet_header , sizeof ( struct ethhdr ) ) ;

		memcpy( packet + sizeof ( struct ethhdr ) , ip_header ,  ip_header -> ihl * 4 ) ;

		memcpy( (packet + sizeof ( struct ethhdr ) +  ip_header -> ihl * 4 ) , tcp_header , tcp_header -> doff * 4 ) ;

		memcpy( (packet + sizeof ( struct ethhdr ) +  ( ip_header -> ihl * 4 ) + (tcp_header -> doff * 4 ) ) , data_to_add , DATA_SIZE ) ;

			if ( !sendrawpacket ( raw ,  packet , pkt_len  ) )	{

				perror("sending packet");

			}

			else {

				puts("packet sent successfully");

			}
		
		free ( data_to_add ) ;

		free  ( packet ) ;

		}

		free ( ethernet_header );

		free (ip_header);

		free (tcp_header);

		close ( raw ) ;

	}




