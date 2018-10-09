#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/ip.h>
#include "gep.h"
#include <linux/udp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/libc-compat.h>

#define BUF_SIZE 256
#define FAILED -1
void printinhex( char *message , unsigned char *packet , int len );
int packets_to_sniff ;
FILE *fp ;

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

parse_ether_header ( unsigned char *packet , int len ) 
{

	struct ethhdr *ethernet_header ;

	printf("size of ethhdr = %d\n",sizeof ( struct ethhdr));

	if ( len > sizeof ( struct ethhdr ) ) {

		ethernet_header = (struct ethhdr *) packet ;

		/* first six bytes : destination MAC address */

		/* next six bytes : source MAC address */

		/* last two are protocol bytes */

/*		printinhex ( "dest mac: " , ethernet_header -> h_dest , 6 ) ;

		printf("\n");

		printinhex ( "source mac: " , ethernet_header -> h_source , 6 ) ;

		printf("\n");

		printinhex ("protocol :" , (void*) &ethernet_header -> h_proto , 2) ;

		printf("\n");*/

	}
}

int parse_ge_header ( unsigned char* packet , int len )
{
	struct ethhdr *ethernet_header ;

	struct gehdr *ge_header ;

	int i = 0 ;

	if ( ( len >= ( sizeof (  struct ethhdr ) + sizeof ( struct gehdr ) ) ) ) {

		ethernet_header = (struct ethhdr*)packet ;

		ge_header = (struct gehdr*) ( packet + sizeof (struct ethhdr ) ) ;	

		if ( (ge_header -> src_port) == 222 ) {
			
			puts("recv");

			/*printf("source port : %d\n" , (ge_header -> src_port));

			printf("dest port : %d\n" , (ge_header -> dest_port));

			printf("len is :%d\n",ntohs(ge_header -> len) ) ;*/

			fprintf(fp ,"%s",(packet + sizeof ( struct ethhdr ) + sizeof (struct gehdr) )) ;
//			printf("%s",(packet + sizeof ( struct ethhdr ) + sizeof (struct gehdr) )) ;

			//printf("packets to sniff %d\n",packets_to_sniff);

			return 0 ;

		}

		else {

			return 1 ;
		}

	}

}
void printinhex( char *message , unsigned char *packet , int len )
{

	unsigned char * p = packet ;

	//      system("clear");

	puts(message);

	while ( len--) {

		printf("%.2x ",*p);

		p++;

	}

	printf("\n");

}


int main ( int c , char *v[]) 
{
	int raw ;

	unsigned char packet_buffer[2048] = "";

	int len ;


	struct sockaddr_ll packet_info ;

	int variable = 0 ;

	int packet_info_size = sizeof ( packet_info ) ;

	fp = fopen ( "filerecv" , "w" ) ;

	if ( c != 3 ) {

		printf("usage : %s < interface > <number of packets to sniff>\n",v[0]);

		exit ( EXIT_FAILURE );

	}

	/* PF_PACKET is a way to create RAW sockets */

	/* PF_PACKET is software interface to send/receive packets over layer 2 */

	/* if PF_PACKET is set kernel will not modify any packet so full headers will be there */

	/* protocal is ETH_P_IP for ip networks*/

	/* if you wish to receive all types of packets ( not only ip packets ) you have to use ETH_P_ALL */

	raw = createrawsocket ( ETH_P_IP ) ;

	bindrawsockettointerface ( v[1] , raw , ETH_P_IP );

	packets_to_sniff = -0 ;

	while ( packets_to_sniff-- != -24 ) {

		if ( ( len = recvfrom ( raw ,packet_buffer , 2048 , 0 , (struct sockaddr*) &packet_info , &packet_info_size ) ) == FAILED ) {

			perror("recvfrom");

			exit ( EXIT_FAILURE );

		}

		else {


			if ( ( variable = parse_ge_header ( packet_buffer , len ) ) == 1) {

				packets_to_sniff ++ ;
				
			}
		}
	}
	return 0 ;
}
