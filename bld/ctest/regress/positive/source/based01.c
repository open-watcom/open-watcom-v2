#include "fail.h"

#if defined( _M_IX86 )

#include <i86.h>

#define MY_MK_FP(seg,off) (((__segment)(seg)):>((void __based(void) *)(off)))

int main( void )
{
    char                    c0, c1, c2, c3;
    char __based( void )    *off;
    __segment               seg;
    void __far              *lp = MY_MK_FP( 0x1234, 0x5678 );

    c0 = 0xAA;

    off = (char __based( void ) *)FP_OFF( &c0 );
    seg = FP_SEG( &c0 );

    /* Test various ways of data access through based pointer */
    c1 = *(seg:>off);
    c2 = *(char __based( seg ) *)off;
    c3 = ((char __based( seg ) *)off)[0];

    if( c0 != c1 || c1 != c2 || c2 != c3 ) fail(__LINE__);

    /* Test pointer conversions */
#if defined( __386__ )
    if( (unsigned long long)lp != 0x123400005678 ) fail( __LINE__ );
#else
    if( (unsigned long)lp != 0x12345678 ) fail( __LINE__ );
#endif
    if( lp != MK_FP( 0x1234, 0x5678 ) ) fail( __LINE__ );
    _PASS;
}

#else

ALWAYS_PASS

#endif
