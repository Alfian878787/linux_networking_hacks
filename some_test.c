#include	<stdio.h>
int main ( void ) {
#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1

	printf("yes");
#else
	printf("yes");
#endif


}
