#include	<uma.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



int main ( void ) {

		struct stat inst ;

		int fp  = open( "./file_size" , O_RDWR) ;

		fstat  ( fp , &inst ) ;

		printf("%d\n",(off_t)inst.st_size);
		
		printf("%u\n",inst.st_ino);

}
