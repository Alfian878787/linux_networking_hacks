/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <net/if.h>
#include	<string.h>
#include	<stdio.h>

int main(int c, char *v[]) 
{
	struct ifreq ifr;

	int s;

	if ( c < 2 ) {

		printf("usage: <%s> < aa:bb:cc:dd:ee:ff >\n",v[0]);

		return 1 ;

	}

	sscanf(v[1], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&ifr.ifr_hwaddr.sa_data[0],
			&ifr.ifr_hwaddr.sa_data[1],
			&ifr.ifr_hwaddr.sa_data[2],
			&ifr.ifr_hwaddr.sa_data[3],
			&ifr.ifr_hwaddr.sa_data[4],
			&ifr.ifr_hwaddr.sa_data[5]
	      );

	s = socket(AF_INET, SOCK_DGRAM, 0);

	strcpy(ifr.ifr_name, "enp2s0");

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER; //ethernet 10 Mbps

	printf("%x:%x:%x:%x:%x:%x\n",ifr.ifr_hwaddr.sa_data[0],ifr.ifr_hwaddr.sa_data[1],ifr.ifr_hwaddr.sa_data[2],ifr.ifr_hwaddr.sa_data[3],ifr.ifr_hwaddr.sa_data[4],ifr.ifr_hwaddr.sa_data[5]);

	ioctl(s, SIOCSIFHWADDR, &ifr);

	perror("");	

	return 0 ;
}
