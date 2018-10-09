/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include    <stdlib.h>
#define FAILED -1
#include    <string.h>
#include    <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/libc-compat.h>

#define BUF_SIZE 256

unsigned short int global_variable_to_check_tcp_and_ip = 0;

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

		printf("\n");

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

			printf("ip header length is : %d bytes\n\n" , ip_header -> ihl) ;

		}

	}

}

void parse_tcp_header ( unsigned char* packet , int len ) 
{
	struct ethhdr *ethernet_header ;

	struct iphdr *ip_header ;

	struct tcphdr *tcp_header ;

	puts("in tcp parser");

	if ( ( len >= ( sizeof (  struct ethhdr ) + sizeof ( struct iphdr ) + sizeof ( struct tcphdr ) ) ) ) {

		ethernet_header = (struct ethhdr*)packet ;

		if ( (ntohs ( ethernet_header -> h_proto ) ) == ETH_P_IP ) {

			ip_header = (struct iphdr*) ( packet + sizeof (struct ethhdr ) ) ;

			printf("IPPROTO_TCP is %d\n" , ip_header -> protocol);

			global_variable_to_check_tcp_and_ip = ip_header -> protocol  ;

			/* its all udp so no tcp details will be printed */

			if ( ip_header -> protocol ==IPPROTO_TCP /* 6 */) {

				tcp_header = (struct tcphdr*) ( packet + sizeof (  struct ethhdr ) + (ip_header -> ihl * 4 ) ) ;

				printf("source port : %d\n" , ntohs(tcp_header -> source)); 

				printf("dest port : %d\n" , ntohs(tcp_header -> dest)); 

				printf("sequence number : %d\n" , ntohs(tcp_header -> seq)); 

				printf("acknowledge number : %d\n" , ntohs(tcp_header -> ack_seq)); 

			}

			else puts("not a tcp packet\n");	
		}	

	}			

}

void parse_udp_header ( unsigned char* packet , int len )
{
	struct ethhdr *ethernet_header ;

	struct iphdr *ip_header ;

	struct udphdr *udp_header ;

	puts("in udp parser");

	if ( ( len >= ( sizeof (  struct ethhdr ) + sizeof ( struct iphdr ) + sizeof ( struct udphdr ) ) ) ) {

		ethernet_header = (struct ethhdr*)packet ;

		if ( (ntohs ( ethernet_header -> h_proto ) ) == ETH_P_IP ) {

			ip_header = (struct iphdr*) ( packet + sizeof (struct ethhdr ) ) ;

			//printf("IPPROTO_TCP is %d\n" , ip_header -> protocol);

			/* its all udp so no tcp details will be printed */


			if ( ip_header -> protocol ==IPPROTO_UDP /* 17 */) {

				udp_header = (struct udphdr*) ( packet + sizeof (  struct ethhdr ) + (ip_header -> ihl * 4 ) ) ;

				printf("source port : %d\n" , ntohs(udp_header -> source));

				printf("dest port : %d\n" , ntohs(udp_header -> dest));

			}

			else puts("not a udp packet");
		}

	}

}

int parse_data ( unsigned char *packet , int len ) 
{

	struct ethhdr *ethernet_header ;

	struct iphdr *ip_header ;

	struct tcphdr *tcp_header ;

	unsigned char *data ;

	int data_len ;

	if ( ( len > ( sizeof (  struct ethhdr ) + sizeof ( struct iphdr ) + sizeof ( struct tcphdr ) ) ) ) {

		ip_header = (struct iphdr*) ( packet + sizeof (struct ethhdr ) ) ;

		data =  (packet + sizeof (  struct ethhdr ) + (ip_header -> ihl * 4 ) + sizeof ( struct tcphdr ) ) ;

		data_len = ntohs ( ip_header -> tot_len ) -  (ip_header -> ihl * 4 ) - sizeof ( struct tcphdr ) ;

		if ( data_len ) {

			printinhex ( "data : " , data , data_len ) ;

			puts("\n");

			return 1;

		}
		else
		{
			puts("no data in packet");

			return 0 ;

		}

		puts("no data in packet");
	}
}


void printinhex( char *message , unsigned char *packet , int len ) 
{

	unsigned char * p = packet ;

	//	system("clear");

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

	/* PF_PACKET is a way to create RAW sockets */

	/* PF_PACKET is software interface to send/receive packets over layer 2 */

	/* if PF_PACKET is set kernel will not modify any packet so full headers will be there */

	/* protocal is ETH_P_IP for ip networks*/
	
	/* if you wish to receive all types of packets ( not only ip packets ) you have to use ETH_P_ALL */

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

			parse_tcp_header ( packet_buffer , len ) ;

			parse_udp_header ( packet_buffer , len ) ;

			if (  global_variable_to_check_tcp_and_ip == IPPROTO_TCP ) {

				if ( !parse_data ( packet_buffer , len ) )

					packets_to_sniff ++ ;

			}

			if ( global_variable_to_check_tcp_and_ip == 1  /* icmp */ ) 

				puts("packet is an ICMP packet");


		}

	}

	return 0 ;
}
