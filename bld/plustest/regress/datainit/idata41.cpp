#include "dump.h"
static char *p1 = 0;
static char *p2[] = { "abc", "def", "ghi" };
char *f1( void ) { return p1; }
char **f2( void ) { return p2; }
int main( void )
{
    char *a1;
    char **a2;

    DUMP( p1 );
    DUMP( *p2 );
    DUMP( *p2[0] );
    DUMP( *p2[1] );
    DUMP( *p2[2] );
    a1 = f1();
    a2 = f2();
    DUMP( a1 );
    if( a1 ) {
	DUMP( *a1 );
    }
    DUMP( *a2 );
    return 0;
}
