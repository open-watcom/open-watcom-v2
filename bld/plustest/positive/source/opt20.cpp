#include "fail.h"

unsigned fib( unsigned i, unsigned n,
    unsigned p01, unsigned p02, unsigned p03, unsigned p04, unsigned p05,
    unsigned p06, unsigned p07, unsigned p08, unsigned p09, unsigned p10,
    unsigned p11, unsigned p12, unsigned p13, unsigned p14, unsigned p15,
    unsigned p16, unsigned p17, unsigned p18, unsigned p19, unsigned p20
    )
{
    #if 0
    printf( "%u %u "
    "%u %u %u %u %u "
    "%u %u %u %u %u "
    "%u %u %u %u %u "
    "%u %u %u %u %u "
    "\n",
	i, n,
    p01, p02, p03, p04, p05,
    p06, p07, p08, p09, p10,
    p11, p12, p13, p14, p15,
    p16, p17, p18, p19, p20
    );
    #endif

    return i == n ? p20+p01 :
    	fib( i+1, n,
    p01+p02, p03, p04, p05,
    p06, p07, p08, p09, p10,
    p11, p12, p13, p14, p15,
    p16, p17, p18, p19, p20,
	p20 + p19 );

}

int main() {
    if( fib( 1,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 ) != 6766 ) fail(__LINE__);
    _PASS;
}
