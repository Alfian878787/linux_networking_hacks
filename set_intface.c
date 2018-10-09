/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
	struct ifreq ifr;
	in_addr_t in_addr;
	struct sockaddr_in sin;

	int sockfd ;

	int io = 0 ;

	memset(&ifr, 0, sizeof(struct ifreq));
	memset(&sin, 0, sizeof(struct sockaddr_in));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sprintf(ifr.ifr_name, "eth0");
	in_addr = inet_addr("172.16.5.129");
	sin.sin_addr.s_addr = in_addr;
	memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
	io = ioctl(sockfd, SIOCSIFADDR, &ifr);
	perror(""); 

}
