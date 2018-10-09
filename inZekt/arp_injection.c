/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include    <uma.h>
#include    "header.h"
#define FAILED -1
#define SRC_MAC "c0:3f:d5:ff:58:eb"
#define DEST_MAC "c0:3f:d5:ff:53:f7"
#define SRC_IP "172.16.5.151"
#define DEST_IP "172.16.9.1"

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

	sll . sll_family = AF_PACKET ;  /* always AF_PACKET */

	sll.sll_ifindex = ifr.ifr_ifindex ; /* interface number */

	sll.sll_protocol = htons ( protocal ); /* physicalm layer protocol */

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

_ether_header *createethernetheader ( char* src_mac , char* dest_mac , int protocol )
{
	_ether_header *ethernet_header ;

	ethernet_header = ( _ether_header *)malloc ( sizeof ( _ether_header ) ) ;

	memcpy ( ethernet_header -> source , (void*) ether_aton ( src_mac ) , 6 ); /* asci to network order */

	memcpy ( ethernet_header -> destination , (void*) ether_aton ( dest_mac ) , 6 );

	ethernet_header -> protocol = htons ( 0x0806 ) ;

	return (ethernet_header) ;

}

arpheader * createarpheader ( void )
{
	arpheader *arp_header ;

	in_addr_t temp ;

	arp_header = ( arpheader *)malloc ( sizeof ( arpheader ) ) ;

	arp_header -> hardware_type = htons ( ARPHRD_ETHER ) ;

	arp_header -> protocol_type = htons ( ETHERTYPE_IP ) ;

	arp_header -> hard_addr_len = 6 ;

	arp_header -> prot_addr_len = 4 ;

	arp_header -> opcode = htons ( /*ARPOP_REQUEST*/ 1 ) ;

	memcpy ( arp_header -> source_hardware , (void*) ether_aton ( SRC_MAC ) , 6 ); /* asci to network order */
	
	memcpy ( arp_header -> dest_hardware , (void*) ether_aton ( DEST_MAC ) , 6 ); /* asci to network order */

	temp = inet_addr ( SRC_IP ) ;
	
	memcpy ( &(arp_header -> source_ip) , &temp , 4 ); /* asci to network order */
	
	temp = inet_addr ( DEST_IP ) ;
	
	memcpy ( &(arp_header -> dest_ip) , &temp , 4 ); /* asci to network order */

	return arp_header ;

}

int main ( int c , char *v[]) 
{
	int raw ;

	unsigned char *packet  ;

	_ether_header *ethernet_header  ;

	arpheader *arp_header  ;

	int pkt_len = 0; 

	if ( c != 3 ) {

		printf("usage: %s <interface> <destination ip>",v[0]);

		exit ( EXIT_FAILURE );

	}

	raw = createrawsocket ( ETH_P_ALL );

	bindrawsockettointerface ( v[1] , raw , ETH_P_ALL );

	ethernet_header = createethernetheader ( SRC_MAC , DEST_MAC , ETHERTYPE_IP /* IP family*/) ;

	arp_header = createarpheader () ;

	pkt_len = sizeof ( _ether_header ) + sizeof ( arpheader ) ;

	packet = (unsigned char*) malloc ( pkt_len ) ;

	memcpy( packet , ethernet_header , sizeof ( _ether_header  ) ) ;

	memcpy( packet + sizeof ( _ether_header ) , arp_header ,  sizeof ( arpheader ) ) ;

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

	free (arp_header);

	free ( packet );

	close ( raw ) ;

}




