#include "fail.h"
#define A2 0x02
#define A3 0xaa0e+A2
enum {
    A7 = 1,
};
#define A7	0xaa0e+A7

#define A10	3
#define A11	1e+A10

#define A15 3
#define A16( a,b )	(b+a)
#define A17 0xaa0e+A16 (1,2)

#define A19( x )	#x
#define A20( x )	A19( x )
#define A21( x, y )	A20( x ## y )
#define A22( x, y )	A19( x ## y )

int a[] = {
    A3,	0xaa10,
    A7,	0xaa0f,
    A17,0xaa11,
};

char *b[] = {
    A21( 0xaa0e+A , 7 ),	"0xaa0e+0xaa0e+A7",
    A22( 0xaa0e+A , 7 ),	"0xaa0e+A7",
    A19( 0xaa0e+A7 ),		"0xaa0e+A7",
};

int main() {
    int i;

    for( i = 0; i < (sizeof(a)/sizeof(a[0])); i+=2 ) {
	if( a[i] != a[i+1] ) _fail;
    }
    for( i = 0; i < (sizeof(b)/sizeof(b[0])); i+=2 ) {
	if( strcmp( b[i], b[i+1] ) != 0 ) _fail;
    }
    _PASS;
}
