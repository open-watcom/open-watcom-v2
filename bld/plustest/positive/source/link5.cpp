#include "fail.h"

#if defined( _M_IX86 )

#include <i86.h>
#include "link5.h"

int main()
{
    if( a_seg != FP_SEG( X::a ) ) fail(__LINE__);
    if( a_off != (void __based(void) *) FP_OFF( X::a ) ) fail(__LINE__);
    if( i_seg != FP_SEG( &X::i ) ) fail(__LINE__);
    if( i_off != (void __based(void) *) FP_OFF( &X::i ) ) fail(__LINE__);
    _PASS;
}

#else

ALWAYS_PASS

#endif
