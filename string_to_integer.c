#include	<string.h>

#include	<uma.h>

char *int_ascii ( unsigned long int ) ;

//static unsigned char arr[4] = {} ;

unsigned long int ascii_int(char*) ;

unsigned long int htonl_u ( unsigned long int ) ;

unsigned long int ascii_int ( char *str ) 
{
		char *temp = str ;

		char *token = NULL ;

		int i = 0 ;

		static unsigned char arr[4] = {} ;

		token = strtok( str , ".") ;

		i = 3 ;

		while ( token != NULL ) {

				printf ("%s\n" , token ) ;

				arr[ i-- ] = atoi ( token ) ;

				token = strtok ( NULL , ".") ;

		}
		return *(int*)arr ;

}

unsigned long int htonl_u ( unsigned long int integer )
{


		int i = 0 ;

		static unsigned char arr[4] = {} ;
		
		memset (arr , 0 , 4) ;

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

		static char array [15] = {} ;

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

		return array ;

}


