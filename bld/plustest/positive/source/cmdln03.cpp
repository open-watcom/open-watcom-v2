#include "fail.h"

#if __WATCOM_REVISION__ >= 7

#ifdef X
int x = -5;
#else
int x;
#endif

#ifndef XX
int xx = -55;
#else
int xx;
#endif

#ifdef XXX
int xxx = -555;
#else
int xxx;
#endif

#ifndef XXXX
int xxxx = -5555;
#else
int xxxx;
#endif

int main() {
    if( x != -5 ) fail(__LINE__);
    if( xx != -55 ) fail(__LINE__);
    if( xxx != -555 ) fail(__LINE__);
    if( xxxx != -5555 ) fail(__LINE__);
    _PASS;
}

#else

ALWAYS_PASS

#endif
