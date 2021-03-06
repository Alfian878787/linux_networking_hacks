/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	int n;

	struct ifreq ifr;

	char array[] = "eth0";

	n = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);

	ioctl(n, SIOCGIFADDR, &ifr);
	
	perror("ioctl");

	close(n);

	printf("IP adress for interface %s is %s\n" , array , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );

	return 0;
}
