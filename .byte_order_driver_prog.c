#include	<string.h>

#include	<uma.h>

char *int_ascii ( unsigned long int ) ;

static unsigned char arr[4] = {} ;

unsigned long int _conv(char*) ;

unsigned long int htonl_u ( unsigned long int ) ;

int i = 3 ;

int main ( void ) 
{

		char str[] = "172.16.0.0" ;

		unsigned char* ptr = NULL ;

		unsigned long int integer = _conv(str) ;

		unsigned long int endianess = 0 ;

		static unsigned char arr[4] = {} ;

		ptr = (char*)&integer ;

		i = 0 ;

		puts(B"little endian format"Z) ;

		while ( i++ < 4 ) 

				printf (BO G"%u "Z , *ptr++ ) ;

		puts("");

		endianess = htonl_u ( integer ) ;

		ptr = (char*) &endianess ;

		i = 0 ;

		puts(B"big endian format"Z) ;

		while ( i++ < 4 )

				printf (BO G"%u "Z , *ptr++ ) ;

		puts("");

		/* now in dotted string format */

		//	endianess = integer ;

		puts(B"little endian format"Z) ;

		int_ascii ( endianess ) ;

		puts(B"big endian format"Z) ;

		int_ascii ( integer ) ;

		return 0 ;

}

unsigned long int _conv ( char *str ) 
{
		char *temp = str ;

		char *token = NULL ;

		static unsigned char arr[4] = {} ;

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

		static unsigned char arr[4] = {} ;

		int some = 24 ;

		unsigned int mask = 0xff000000 ;

		while ( i < 4 ) {

				arr[i++] =  (integer & mask) >> some  ;

				mask >>= 8 ;

				some-=8 ;

		}

		return *(int*)arr ;

}

char *int_ascii ( unsigned long integer )
{

		int some = 24 ;

		unsigned int mask = 0xff000000 ;

		char array [15] = {} ;

		int index = 0 ;

		int temp = 0 ;

		int rem = 0 ;

		int i = 0 ;

		int var = 100 ;

		static unsigned char arr[4] = {} ;

		while ( i < 4 ) {

				arr[i++] =  (integer & mask) >> some  ;

				temp = arr [ i-1 ] ;

				var = 100 ;

				while ( var) {

						rem = temp / var ;

						rem = rem + 48 ;

						if ( rem != 48 || var == 1 )	

								array[index++] = rem ; 

						//				if ( !(temp == (temp % var)) ) temp = 0 ;

						//				else

						temp = temp%var ;

						if ( var == 1 ) var = 0 ; ;

						var= var / 10 ;


				}	

				array[index++] = '.' ;

				mask >>= 8 ;

				some-=8 ;

		}

		array[--index] = '\0' ;

		puts(array);
}


