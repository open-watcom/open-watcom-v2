#include "fail.h"

#if defined( M_I86 ) || defined( M_I386 )
// #pragma on (dump_init)
char __based(__segname("foo_TEXT")) bar[] = "abc";

int __based(__segname("foo_TEXT")) main()
{
    if( bar[0] == 'a'
     && bar[1] == 'b'
     && bar[2] == 'c' ) {
    } else {
        fail( __LINE__ );
    }
    _PASS;
}

#else

ALWAYS_PASS

#endif
