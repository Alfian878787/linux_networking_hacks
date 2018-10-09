#include	<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void print ( char* buffer )
{
	FILE *fp = fopen ( "k" , "a+");

	fprintf ( fp , "%s" , buffer ) ;
}

int main ( void ) 
{
	FILE *fp = fopen ( "killer" , "r" ) ;

	struct stat buf ;

	char buffer[1024];

	stat ( "killer" , &buf ) ;

	printf("%d\n",buf . st_size % 2);

	int var = buf . st_size / 2 ; 

	while ( var-- ) {

		fread ( buffer , 2 , 1 , fp ) ;

		print(buffer);

	}
	/*	fread ( buffer , 1024 , 1 , fp ) ;

		print(buffer);*/

        
        fread ( buffer ,  buf . st_size % 2 , 1 , fp ) ;

	print(buffer);



}
