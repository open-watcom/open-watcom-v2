#include <stdio.h>
#include <mmintrin.h>

long featureflags( void );

#pragma aux featureflags = \
    ".586"          \
    "mov eax,1"     \
    "cpuid"         \
    "mov eax,edx"   \
    modify [eax ebx ecx edx]

#define MM_EXTENSION 0x00800000

void main( void )
{
    if( featureflags() & MM_EXTENSION ) {
    /*
        sequence of code that uses Multimedia functions
        .
        .
        .
    */

        _m_empty();
    }

    /*
        sequence of code that uses floating-point
        .
        .
        .
    */
}
