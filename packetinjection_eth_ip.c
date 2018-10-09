/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

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
#include <linux/libc-compat.h>

#define PACKET_LENGTH 1024
#define SRC_MAC "dc:fe:07:00:fe:cc"
#define DEST_MAC "dc:fe:07:00:f6:f8"
#define SRC_IP "172.16.5.151"
//#define DEST_IP "172.16.5.245"

void printinhex( char *message , unsigned char *packet , int len ); 

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

	memcpy ( ethernet_header -> h_source , (void*) ether_aton ( src_mac ) , 6 ); /* asci to network order */

	memcpy ( ethernet_header -> h_dest , (void*) ether_aton ( dest_mac ) , 6 );

	ethernet_header -> h_proto = htons ( protocol ) ;

	return ((unsigned char*)ethernet_header) ;

}

unsigned short computeipchecksum ( unsigned char* header , int len ) 
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

	ip_header = (struct iphdr *) malloc ( sizeof (  struct iphdr  ) ) ;

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

	ip_header -> check =  computeipchecksum ( (unsigned char*) ip_header , ip_header -> ihl * 4 );

	return (( unsigned char* )ip_header );

}


int main ( int c , char *v[]) 
{
	int raw ;

	unsigned char *packet  ;

	unsigned char *ethernet_header  ;

	unsigned char *ip_header  ;

	int pkt_len = 0; 

	if ( c != 3 ) {

		printf("usage: %s <interface> <destination ip>",v[0]);

		exit ( EXIT_FAILURE );

	}

	raw = createrawsocket ( ETH_P_ALL );

	bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

	ethernet_header = createethernetheader ( SRC_MAC , DEST_MAC , ETHERTYPE_IP /* IP family*/) ;

	ip_header = createipheader (v[2]) ;

	pkt_len = sizeof ( struct ethhdr ) + sizeof ( struct iphdr ) ;

	packet = (unsigned char*) malloc ( pkt_len ) ;

	memcpy( packet , ethernet_header , sizeof ( struct ethhdr ) ) ;

	memcpy( packet + sizeof ( struct ethhdr ) , ip_header ,  sizeof ( struct iphdr ) ) ;

	while ( 1 ) {

		if ( !sendrawpacket ( raw ,  packet , pkt_len  ) )	{

			perror("sending packet");

		}

		else {

			//	sleep(5);

			puts("packet sent successfully");

		}
	}

	free ( ethernet_header );

	free (ip_header);

	free ( packet );

	close ( raw ) ;

}




