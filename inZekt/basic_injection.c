/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include    <uma.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <features.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h> #include <netinet/in.h>
#include <net/if.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include <linux/libc-compat.h>

#define PACKET_LENGTH 1024

void printinhex( char *message , unsigned char *packet , int len ); 

/* create a raw socket */

/* set interface you wanna sniff to promiscus mode */

/* bind raw socket to this interface - bind() */

/* recv packets on network recvfrom() */

/* process data n close raw socket */

/* interface is eth0 */ 

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


int main ( int c , char *v[]) 
{
		int raw ;

		unsigned char packet [ PACKET_LENGTH ] = "dc:fe:07:00:fe:cc" ;

		int num_of_pkts = 0 ;

		//memset ( packet , "dc:fe:07:00:fe:cc " , PACKET_LENGTH ) ;

		puts(packet);

		raw = createrawsocket ( ETH_P_ALL );

		bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

		num_of_pkts = atoi ( v[2] ) ;

		while ( ( num_of_pkts -- ) > 0 ) {

			if ( !sendrawpacket ( raw , packet , PACKET_LENGTH ) ) {

				perror("sending packet");

			}

			else {

			puts("sent packets successfully");

			}

		}

	close ( raw ) ;

}


		

