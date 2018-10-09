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
#include "gep.h"
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include	<linux/ip.h>
#include	<linux/tcp.h>
#include <linux/libc-compat.h>

#define PACKET_LENGTH 1024
#define SRC_MAC "dc:fe:07:00:fe:cc"
#define DEST_MAC "74:27:ea:d2:ab:f7"
//#define DEST_MAC "dc:fe:07:00:f6:f8"
#define SRC_IP "172.16.5.151"
#define DEST_IP "172.16.5.151"
//#define DEST_IP "172.16.5.245"

int global_variable = 0;

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

unsigned char* creategeheader ( char *v )
{
	struct gehdr *ge_header ;
	
	char buffer[100] ;

	puts ("enter any data to include in header") ;

	fgets ( buffer , 100 , stdin ) ;

	global_variable = strlen ( buffer ) ;

	printf("%zu value\n" , sizeof (  struct gehdr  ) + strlen ( buffer));

	ge_header = (struct gehdr *) malloc ( sizeof (  struct gehdr  ) + strlen ( buffer) ) ;

	memset ( ge_header , '\0' , (sizeof (  struct gehdr  ) + strlen ( buffer) ) );
	
	ge_header -> src_port = 222 ;
	
	ge_header -> dest_port =(128);

	memcpy ( ge_header + sizeof (  struct gehdr  )  , buffer , strlen ( buffer)  ); 
	
	ge_header -> len = htons ( 4 + strlen ( buffer ));
	
	return (( unsigned char* )ge_header );

}


int main ( int c , char *v[]) 
{
	int raw ;
	
	int _while = 1 ;

	unsigned char *packet  ;

	unsigned char *ethernet_header  ;

	unsigned char *ge_header  ;

	int pkt_len = 0; 

	if ( c != 3 ) {

		printf("usage: %s <interface> <destination ip>",v[0]);

		exit ( EXIT_FAILURE );

	}

	raw = createrawsocket ( ETH_P_ALL );

	bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

	ethernet_header = createethernetheader ( SRC_MAC , DEST_MAC , ETHERTYPE_IP /* IP family*/) ;

	ge_header = creategeheader (v[2]) ;

	pkt_len = sizeof ( struct ethhdr ) + sizeof ( struct gehdr ) ;

	packet = (unsigned char*) malloc ( pkt_len ) ;

	memcpy( packet , ethernet_header , sizeof ( struct ethhdr ) ) ;

	memcpy( packet + sizeof ( struct ethhdr ) , ge_header ,  sizeof ( struct gehdr ) + global_variable ) ;

	printf ("%zu\n", sizeof ( struct gehdr ) + global_variable) ;

	while ( _while-- ) {

		if ( !sendrawpacket ( raw ,  packet , pkt_len  ) )	{

			perror("sending packet");
		}

		else {

			puts("packet sent successfully");

		}
	}


	free (ge_header);

	free ( packet );

	free ( ethernet_header );
	
	close ( raw ) ;

}




