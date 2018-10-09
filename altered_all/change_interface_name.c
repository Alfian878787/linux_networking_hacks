/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>             /* offsetof */
#include <net/if.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#define IFNAME "eth0"

int main ( int c , char *v[] ) 
{
	if ( c < 3 ) {

		printf("usage: <%s> < old interface name> <new interface name>\n",v[0]);

		return 1 ;

	}

	struct ifreq ifr ;

	int sock ;

	strcpy ( ifr . ifr_name , v[1] ) ;
	
	strcpy ( ifr . ifr_newname , v[2] ) ;

	sock = socket ( AF_INET , SOCK_DGRAM , 0 ) ;

	ioctl  ( sock , SIOCSIFNAME , &ifr ) ;

	perror("ioctl call");

	return 0 ;

}

	
