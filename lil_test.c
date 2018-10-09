#include	<uma.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SWAP( one , two)  if ( one!=two ) { ( two = ( one + two - ( one = two ) ) ) ;}


void inline SWAP1(char *p, char *q)
{
	*p = *p + *q - (*q = *p);
}

int main ( void ) {

	char buf[] = "172.16.5.151" ;

	unsigned long int x = 0 ;

	unsigned long int y = 0 ;

	x = ascii_int ( buf ) ;

	SWAP1(((char*)&x+0) , ((char*)&x+3));

	SWAP1(((char*)&x+1) , ((char*)&x+2));

	printf ("%u\n" , x ) ;

	printf ("%u\n" , htonl_u ( x ) ) ;

}
