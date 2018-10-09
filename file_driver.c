#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

char *int_ascii ( unsigned long int ) ;

static unsigned char arr[4] = {} ;

unsigned long int ascii_int(char*) ;

unsigned long int htonl_u ( unsigned long int ) ;


typedef struct _ip {

		unsigned long int l_addr ;

		unsigned char name[16] ;

}_ip ;


int main ( void ) {

		FILE *fp = fopen ( "file" , "r" ) ;

		if ( fp == NULL ) { 

				perror("file open");

				exit ( EXIT_FAILURE ) ;

		}

		FILE *fp2 = fopen ( "file2" , "w" ) ;

		if ( fp2 == NULL ) { 

				perror("file open");

				exit ( EXIT_FAILURE ) ;

		}

		int i = 0 ;

		_ip **ip = NULL ;

		unsigned long int ip_addr = 0 ;

		char *str = malloc(20) ;

		if ( str == NULL ) { 

				perror("memalloc");

				exit ( EXIT_FAILURE ) ;

		}


		char *str2 = malloc(20) ;

		if ( str2 == NULL ) { 

				perror("memalloc");

				exit ( EXIT_FAILURE ) ;

		}


		int status = 0 ;

		do {

				status = fscanf(fp , "%s %s" , str , str2 );

				if ( status != EOF && status != 0 && str[0] != '#') {

						ip = realloc ( ip , sizeof( _ip*  ) * i + sizeof( _ip*  ) ) ;

						if ( ip == NULL ) { 

								perror("memalloc");

								exit ( EXIT_FAILURE ) ;

						}

						ip[i] = malloc(sizeof( _ip ) ) ;

						if ( ip[i] == NULL ) {

								perror("memalloc");

								exit ( EXIT_FAILURE ) ;

						}

						ip[i]-> l_addr = ascii_int ( str ) ;

						strcpy ( ip[i] -> name , str2 ) ;

						fprintf ( fp2 , "%s %s\n" , int_ascii (ip[i]-> l_addr) , ip[i]-> name) ;

						i++ ;

				}

		}while( status != 0 && status != EOF ) ;

		free(ip[i]);

		while ( i-- )

				free(ip[i]) ;

		free(ip) ;

		free(str) ;

		free(str2);

		fclose(fp);

		fclose(fp2);

}

