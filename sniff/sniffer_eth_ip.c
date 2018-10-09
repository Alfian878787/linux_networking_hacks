/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#define FAILED -1
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/ip.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include <linux/libc-compat.h>

#define BUF_SIZE 256

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

parse_ether_header ( unsigned char *packet , int len ) 
{

		struct ethhdr *ethernet_header ;

		printf("size of ethhdr = %d\n",sizeof ( struct ethhdr));

		if ( len > sizeof ( struct ethhdr ) ) {

				ethernet_header = (struct ethhdr *) packet ;

				/* first six bytes : destination MAC address */

				/* next six bytes : source MAC address */

				/* last two are protocol bytes */

				printinhex ( "dest mac: " , ethernet_header -> h_dest , 6 ) ;

				printf("\n");

				printinhex ( "source mac: " , ethernet_header -> h_source , 6 ) ;

				printf("\n");

				printinhex ("protocol :" , (void*) &ethernet_header -> h_proto , 2) ;

		}
}

void parse_ip_header ( unsigned char* packet , int len ) 
{
	struct ethhdr *ethernet_header ;

	struct iphdr *ip_header ;
	
	ethernet_header = (struct ethhdr*)packet ;

	if ( (ntohs ( ethernet_header -> h_proto ) ) == ETH_P_IP ) {
	
		if ( len >=  (sizeof ( struct ethhdr ) + sizeof ( struct iphdr ) ) ) {

			ip_header = (struct iphdr*) ( packet + sizeof (struct ethhdr ) ) ;
			
			printf("dest ip address : %s\n",(char*)inet_ntoa ( ip_header -> daddr ));
		
			printf("souce ip address : %s\n",(char*)inet_ntoa ( ip_header -> saddr));
	
			printf("TTL is : %d\n" , ip_header -> ttl) ;
			
			printf("ip header length is : %d bytes\n" , ip_header -> ihl) ;

		}

	}

}
			

void printinhex( char *message , unsigned char *packet , int len ) 
{
		unsigned char * p = packet ;

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

		int packets_to_sniff ;

		struct sockaddr_ll packet_info ;

		int packet_info_size = sizeof ( packet_info ) ;

		if ( c != 3 ) {
		
			printf("usage : %s < interface > <number of packets to sniff>\n",v[0]);

			exit ( EXIT_FAILURE );

		}

		raw = createrawsocket ( ETH_P_IP ) ;

		bindrawsockettointerface ( v[1] , raw , ETH_P_IP );

		packets_to_sniff = atoi ( v[2] );

		while ( packets_to_sniff -- ) {

				if ( ( len = recvfrom ( raw ,packet_buffer , 2048 , 0 , (struct sockaddr*) &packet_info , &packet_info_size ) ) == FAILED ) {

						perror("recvfrom");

						exit ( EXIT_FAILURE );

				}

				else {


						printinhex( "\0" , packet_buffer , len );

						parse_ether_header(packet_buffer , len );

						parse_ip_header ( packet_buffer , len );


				}

		}

		return 0 ;
}
