#define FAILED -1
/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include    <uma.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include <linux/libc-compat.h>

#define BUF_SIZE 256


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

	/* ripped for net */

	if (( ioctl ( raw , SIOCGIFINDEX , &ifr ) ) == FAILED ) {
		
		perror("getting interface");

		exit ( EXIT_FAILURE );

	}

	sll . sll_family = AF_PACKET ;

	sll.sll_ifindex = ifr.ifr_ifindex ;

	printf ("****************8%d\n" , ifr.ifr_ifindex) ;

	sll.sll_protocol = htons ( protocal );

	if (( bind ( raw , (struct sockaddr *)&sll , sizeof (sll))) ==FAILED){
		
		perror("bind");

		exit ( EXIT_FAILURE );
}

}

void printinhex( unsigned char *packet , int len ) 
{
	unsigned char * p = packet ;

	printf("packet starts \n");

	while ( len--) {

		printf("%.2x ",*p);

		p++;

	}

	printf("\npacket ends \n");

}
int main ( int c , char *v[]) 
{
	int raw ;
	
	unsigned char packet_buffer[2048];

	if ( c!=3 ) {

		printf("usage: <%s> <interface> <packets to sniff>" , v[0]);
	
		exit ( EXIT_FAILURE ) ;
	
	}

	int len ;

	int packets_to_sniff ;

	struct sockaddr_ll packet_info ;
	
	int packet_info_size = sizeof ( packet_info ) ;

	raw = createrawsocket ( ETH_P_IP ) ;

	bindrawsockettointerface ( v[1] , raw , ETH_P_IP );

	packets_to_sniff = atoi ( v[2] );

	while ( packets_to_sniff -- ) {

		if ( ( len = recvfrom ( raw ,packet_buffer , 2048 , 0 , (struct sockaddr*) &packet_info , &packet_info_size ) ) == FAILED ) {

			perror("recvfrom");

			exit ( EXIT_FAILURE );

		}
		
		else {

			
		printinhex( packet_buffer , len );
	
		
		}

	}

	return 0 ;

}

