#include "fail.h"
#include <stdlib.h>

// can't believe this stupid thing was actually a bug for n releases (n > 4)

#if __WATCOMC__ > 1060

int main()
{
    int a, b, c, d, e, f;
    
    a = 1; b = 5; c = 2;
    
    if( ( a = b ) || 1 ) c = 6;
    if( ( a != 5 ) || ( b != 5 ) || ( c != 6 ) ) _fail;
    
    d = 1; e = 5; f = 2;
    
    if( ( d = e ) && 0 ) f = 6;
    if( ( d != 5 ) || ( e != 5 ) || ( f == 6 ) ) _fail;

    _PASS;
}

#else

ALWAYS_PASS

#endif
