#include	<stdio.h>

#include	<unistd.h>

#include	<uma.h>

#define NETID 2886731008

int main ( void ) 
{
	long i = 2 ;

	unsigned long netid = NETID ;

	while ( i < 256 ) {
	
		if ( !fork() ) { 

			printf("%s %s\n",inet_ntoa ( ntohl ( netid + i  )) , inet_ntoa ( ntohl ( netid + 7  )));
		
			execl ( "./icmp_rain" , "./icmp_rain" , inet_ntoa ( ntohl ( NETID + i  )),  inet_ntoa ( ntohl ( NETID + 7  )) , NULL ) ; 

			perror("exec");

		}
			else  { i++ ; }
	
		}
	return 0 ; 

}
