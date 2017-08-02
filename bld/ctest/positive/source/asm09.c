#include "fail.h"

/* Verify that #pragma aux information is properly propagated; as
 * a side effect, verify that mmintrin.h more or less works.
 */

#if defined(__386__)

typedef union {
    unsigned __int64    _64[1];
    unsigned long       _32[2];
    unsigned short      _16[4];
    unsigned char        _8[8];
} __m64;

__m64 _m_paddb(__m64 *__m1, __m64 *__m2);

#define _m_paddb(__m1, __m2)            _m_paddb(&(__m1), &(__m2))

#pragma aux __m_binary = parm [eax] [edx] modify exact []

#pragma aux (__m_binary) _m_paddb =     \
                ".586"                  \
                "movq   mm0,[eax]"      \
                "movq   mm1,[edx]"      \
                "paddb  mm0,mm1"        \
                "movq   [esi],mm0"

__m64   a;
__m64   b = { 0x0123456789abcdef };
__m64   c = { 0xfedcba9876543210 };

int main( void )
{
    a = _m_paddb( b, c );
    if( a._64[0] != -1 ) _fail;
    _PASS;
}

#else

int main( void )
{
    _PASS;
}

#endif
