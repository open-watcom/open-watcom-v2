#include "fail.h"

#ifdef __WATCOM_INT64__

__int64 a = 1;
__int64 b = 2;
__int64 c = a + b;
__int64 d = -a;

int main()
{
    if( a != 1 ) fail(__LINE__);
    if( b != 2 ) fail(__LINE__);
    if( c != 3 ) fail(__LINE__);
    if( d != -1 ) fail(__LINE__);
    _PASS;
}

#else
ALWAYS_PASS
#endif
