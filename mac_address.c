/*propreitary of umaraZ Inc written for GLOBALEDGESOFT p LTD */

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
	struct ifreq s;

	int fd = socket(AF_LOCAL, SOCK_DGRAM, IPPROTO_IP);


		int i;
		for (i = 0; i < 6; ++i)

		&s.ifr_addr.sa_data[i] = 0xaa ;

	strcpy(s.ifr_name, "eth0");

	ioctl(fd, SIOCSIFHWADDR, &s);


		perror("");

		return 0;


	return 1;

}
