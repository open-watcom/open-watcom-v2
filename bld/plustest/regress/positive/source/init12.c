#include "fail.h"

#if defined( _M_IX86 )

#define MK_FP(seg,off) (((__segment)(seg)):>((void __based(void) *)(off)))

// #pragma on (dump_init)
char __based(__segname("foo_TEXT")) bar[] = "abc";

/* Now const-fold the :> operator. */
/* NB: This doesn't work in 32-bit mode because the data initializer
 * can't handle static initialization of 16:32 far pointers!
 */
#ifdef _M_I86
void __far *lp = MK_FP( 0x1234, 0x5678 );
#endif

int __based(__segname("foo_TEXT")) main()
{
    if( bar[0] == 'a'
     && bar[1] == 'b'
     && bar[2] == 'c' ) {
    } else {
        fail( __LINE__ );
    }
#ifdef _M_I86
    if( (unsigned long)lp != 0x12345678 ) fail( __LINE__ );
#endif
    _PASS;
}

#else

ALWAYS_PASS

#endif
