#include "dump.h"
#include <stdio.h>
class A {
    public:
	char a;
    A();
    A( int );
};
static A *pp = 0;
A::A()
{
    a = 'a';
    if( pp == 0 ) {
	printf( "CTOR1\n" );
	pp = this;
    } else {
	printf( "CTOR1:%x\n", this - pp );
    }
}
A::A( int i )
{
    a = (char)i;
    if( pp == 0 ) {
	printf( "CTOR2\n" );
	pp = this;
    } else {
	printf( "CTOR2:%x\n", this - pp );
    }
}

#define INNER   2
#define OUTER_1 3
#define OUTER_2 5

void dump( char *name, A a[][INNER], int limit )
{
    for( int i = 0 ; i < limit ; i++ ) {
	for( int j = 0 ; j < INNER ; j++ ) {
	    printf( "%s[%d,%d] = %d\n", name, i, j, a[i][j].a );
	}
    }
}

int main( void )
{
    {
	int something = __LINE__;
	DUMP( something );
    }
    static A a;
    pp = 0;
    static A b[OUTER_1][INNER];
    pp = 0;
    static A c[OUTER_1][INNER] = { A(1) };
    pp = 0;
    static A d[OUTER_1][INNER] = { { A(1) }, { A(3) } };
    pp = 0;
    static A e[OUTER_2][INNER] = { { A(1) }, { A(3) }, { A(5) } };
    pp = 0;
    printf( "a: %d\n", a.a );
    dump( "b", b, OUTER_1 );
    dump( "c", c, OUTER_1 );
    dump( "d", d, OUTER_1 );
    dump( "e", e, OUTER_2 );
    return 0;
}

