#include "fail.h"
#include <string.h>

// ISO C interpretation 6.8.3.3 page 90

#define hash_hash # ## #
#define mkstr( a )	#a
#define in_between( a ) mkstr( a )
#define join( c, d )	in_between( c hash_hash d )

char *p = join( x, y );

int main() {
    if( strcmp( p, "x ## y" ) ) fail(__LINE__);
    _PASS;
}
