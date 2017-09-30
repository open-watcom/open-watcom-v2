#include "failex.h"

#if defined( _M_IX86 )

#include <i86.h>
#include "link5.h"

int X::i;
char X::a[] = { 'a', 'b', 'c' };

__segment i_seg = FP_SEG( &X::i );
void __based(void) *i_off = FP_OFF( &X::i );
__segment a_seg = FP_SEG( X::a );
void __based(void) *a_off = FP_OFF( X::a );

#else

int never_used = 956;

#endif
