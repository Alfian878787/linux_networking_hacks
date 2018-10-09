#include	<string.h>

#include	<uma.h>

static unsigned char arr[4] = {} ;

unsigned long int _conv(char*) ;

unsigned long int htonl_u ( unsigned long int ) ;
	
int i = 3 ;

int main ( void ) 
{
	
	char str[] = "172.16.5.151" ;

	unsigned char* ptr = NULL ;

	unsigned long int integer = _conv(str) ;

	unsigned long int endianess = 0 ;

	ptr = (char*)&integer ;

	i = 0 ;

	puts(B"little endian format"Z) ;

	while ( i++ < 4 ) 
	
	printf (BO G"%u "Z , *ptr++ ) ;

	puts("");
	
/*	printf ("%lu\n" , integer & 0x000000ff) ;
	
	printf ("%lu\n" , (integer & 0x0000ff00) >> 8) ;
	
	printf ("%lu\n" , (integer & 0x00ff0000) >> 16 ) ;
	
	printf ("%lu\n" , (integer & 0xff000000)>>24) ;*/

	endianess = htonl_u ( integer ) ;

	ptr = (char*) &endianess ;

	i = 0 ;
	
	puts(B"big endian format"Z) ;

    while ( i++ < 4 )

    printf (BO G"%u "Z , *ptr++ ) ;
	
	puts("");

	/*printf ("%lu big endian\n" , endianess) ;

    printf ("%lu\n" , endianess & 0x000000ff) ;

    printf ("%lu\n" , (endianess & 0x0000ff00) >> 8) ;

    printf ("%lu\n" , (endianess & 0x00ff0000) >> 16 ) ;

    printf ("%lu\n" , (endianess & 0xff000000)>>24) ;*/

	return 0 ;

}

unsigned long int _conv ( char *str ) 
{
	char *temp = str ;

	char *token = NULL ;


	token = strtok( str , ".") ;

	i = 3 ;

	while ( token != NULL ) {

			arr[ i-- ] = atoi ( token ) ;
	
			token = strtok ( NULL , ".") ;

		}
	return *(int*)arr ;

}

unsigned long int htonl_u ( unsigned long int integer )
{
	
	memset (arr , 0 , 4) ;

	i = 0 ;

	int some = 24 ;
	
	unsigned int mask = 0xff000000 ;

	while ( i < 4 ) {

		arr[i++] =  (integer & mask) >> some  ;

		mask >>= 8 ;

		some-=8 ;

		}

		return *(int*)arr ;

}

