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
#include <linux/gep.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include	<linux/ip.h>
#include	<linux/tcp.h>
#include <linux/libc-compat.h>

#define PACKET_LENGTH 1024
#define SRC_MAC "dc:fe:07:00:fe:cc"
#define DEST_MAC "dc:fe:07:00:f6:f8"

static FILE *fp ;

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
	
	ge_header = (struct gehdr *) malloc ( sizeof (  struct gehdr  ) ) ;

	memset ( ge_header , '\0' , sizeof (  struct gehdr  )  );
	
	ge_header -> src_port = 222 ;
	
	ge_header -> dest_port =(128);

	ge_header -> len = htons ( 4 );
	
	return (( unsigned char* )ge_header );

}

char* create_data_to_add ( char *name )
{
	char buffer[1024] = "" ;

	char* data ;

//	puts ("enter data");

//	fgets ( buffer , 1023 , stdin ) ;

	fp = fopen ( name , "r" ) ;

	fread ( buffer , 1020 , 1 , fp ) ; 

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
	
	int _while = 1 ;

	unsigned char* data_to_add ;

	unsigned char *packet  ;

	unsigned char *ethernet_header  ;

	unsigned char *ge_header  ;

	int pkt_len = 0; 

	int condition = -1 ;

	if ( c != 3 ) {

		printf("usage: %s <interface> < file to send >",v[0]);

		exit ( EXIT_FAILURE );

	}

	raw = createrawsocket ( ETH_P_ALL );

	bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

	ethernet_header = createethernetheader ( SRC_MAC , DEST_MAC , ETHERTYPE_IP /* IP family*/) ;

	ge_header = creategeheader (v[2]) ;

	while ( condition != -6 ) {

	data_to_add = create_data_to_add ( v[2] ) ;

	pkt_len = sizeof ( struct ethhdr ) + sizeof ( struct gehdr ) + strlen (data_to_add ) ;

	packet = (unsigned char*) malloc ( pkt_len ) ;

	memset ( packet , '\0' ,  pkt_len ) ;

	memcpy( packet , ethernet_header , sizeof ( struct ethhdr ) ) ;

	memcpy( packet + sizeof ( struct ethhdr ) , ge_header ,  sizeof ( struct gehdr )  ) ;
	
	memcpy( packet + sizeof ( struct ethhdr ) + sizeof ( struct gehdr ) , data_to_add , strlen(data_to_add) ) ;

	while ( _while-- ) {

		if ( !sendrawpacket ( raw ,  packet , pkt_len  ) )	{

			perror("sending packet");
		}

		else {

			puts("packet sent successfully");

			printf("%d\n",condition--);

		}
	}

	free ( data_to_add ) ;

	_while = 1 ;
	
	}

	free (ge_header);

	free ( packet );

	free ( ethernet_header );

	close ( raw ) ;

}
