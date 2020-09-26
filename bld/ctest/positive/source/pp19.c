#include "fail.h"

enum {
    A7 = 1,
};
#define A7	0xaa0e+A7
#define A 0xaa0e

#define A10	3
#define A11	1e+A10

#define A19( x )	#x
#define A20( x )	A19( x )
#define A21( x, y )	A20( x ## y )
#define A22( x, y )	A19( x ## y )

char *b[] = {
    A21( 0xaa0e+A , 7 ),	"0xaa0e+A7",
    A22( 0xaa0e+A , 7 ),	"0xaa0e+A7",
    A19( 0xaa0e+A7 ),		"0xaa0e+A7",
};

int main() {
    int i;

    for( i = 0; i < (sizeof(b)/sizeof(b[0])); i+=2 ) {
	if( strcmp( b[i], b[i+1] ) != 0 ) _fail;
    }
    _PASS;
}
