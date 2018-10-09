#include	<stdio.h>
#include	<strings.h>
#include	<string.h>
#include	<stdlib.h>
#include	<sys/socket.h>
#include	<features.h>
#include	<net/if.h>
#include	<linux/if_packet.h>
#include	<linux/if_ether.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include	<net/ethernet.h>
#include	<net/if_arp.h>
#include	<arpa/inet.h>
#include	<netinet/in.h>

typedef struct _ether_header {	

	unsigned char destination[6] ;

	unsigned char source[6] ;

	unsigned short protocol ;

} _ether_header ;

typedef struct arp_header {

	unsigned short hardware_type ;
	
	unsigned short protocol_type ;

	unsigned char hard_addr_len ;

	unsigned char prot_addr_len ;

	unsigned short opcode ;

	unsigned char source_hardware[6] ;

	unsigned char source_ip [4] ;

	unsigned char dest_hardware[6] ;

	unsigned char dest_ip [4] ;

}arpheader ;
