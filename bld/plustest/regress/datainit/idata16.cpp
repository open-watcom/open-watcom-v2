#include <stdio.h>
class C {
    public:
	int c;
	C( int );
};
C::C( int a ) { c = a; }

void g( int a )
{
    C c1( a );
    C c2 = c1;
    printf( "%d %d\n", c1.c, c2.c );
}

int main( void )
{
    g( 7 );
    return 0;
}
