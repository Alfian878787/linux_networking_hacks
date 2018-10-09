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
//#define HOST "172.16.5.245"
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

int main(int c, char **v) {

		struct ifreq ifr;
		struct sockaddr_in inst;
		int sockfd;                     /* socket fd we use to manipulate stuff with */
		int selector;
		unsigned char mask;

		char *p;

		if ( c < 2 ) {

				printf("usage: <%s> <new ip address>\n",v[0]);

				exit ( EXIT_FAILURE ) ;

		}

		sockfd = socket(AF_INET, SOCK_DGRAM, 0);

		strncpy(ifr.ifr_name, IFNAME, IFNAMSIZ);

		memset(&inst, 0, sizeof(struct sockaddr));

		inst.sin_family = AF_INET;

		inst.sin_port = 0;

		inst.sin_addr.s_addr = inet_addr(v[1]);

		p = (char *) &inst;

		memcpy( (((char *)&ifr + ifreq_offsetof(ifr_addr) )), p , sizeof(struct sockaddr));

		ioctl(sockfd, SIOCSIFADDR, &ifr);//set PA address

		perror("setting ip address");

		ioctl(sockfd, SIOCGIFFLAGS, &ifr); //get flags

		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

		ioctl(sockfd, SIOCSIFFLAGS, &ifr);

		close(sockfd);

		return 0;
}
