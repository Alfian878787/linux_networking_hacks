#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/ioctl.h>
#include	<linux/if.h>
#include	<string.h>

int main ( int c , char *v[] ) 
{
	int sock ;

	struct ifreq ifr ;

	strcpy ( ifr . ifr_name , "eth0" ) ;

	ifr.ifr_netmask.sa_family = AF_INET ;

	sscanf(v[1], "%hhx:%hhx:%hhx:%hhx",
                        &ifr.ifr_netmask.sa_data[0],
                        &ifr.ifr_netmask.sa_data[1],
                        &ifr.ifr_netmask.sa_data[2],
                        &ifr.ifr_netmask.sa_data[3]
              );


	sock = socket ( AF_INET , SOCK_DGRAM , 0 ) ;
	
	perror("socket system call");

	ioctl ( sock , SIOCSIFNETMASK , &ifr ) ;

	perror("ioctl call");
	
//	printf("%x %x %x %x\n", ifr . ifr_netmask .sa_data[0] , ifr . ifr_netmask .sa_data[1] ,ifr . ifr_netmask .sa_data[2] ,ifr . ifr_netmask .sa_data[3] );

	return  0 ;

}

	
