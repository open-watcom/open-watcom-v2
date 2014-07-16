#include "fail.h"

static int test(long op, long r1, long r2, long m, long mask)
{
    switch (op) {
    case 0x5200:
    case 0x5000:
    case 0x5400:
	return mask & ((1 << r1) | (1 << r2) | (1 << m));
    case 0x4000:
    case 0x4300:
    case 0x4040:
    case 0x4380:
    case 0x4340:
    case 0x4080:
    case 0x40c0:
    case 0x4100:
    case 0x41c0:
    case 0x4140:
    case 0x4180:
	return mask & ((1 << r2) | (1 << m));
    case 0x9000:
	return mask & ((1 << r1) | (1 << 13));
    case 0xa800:
    case 0xb000:
    case 0xb080:
    case 0x9800:
    case 0xbc00:
	return mask & (1 << 13);
    case 0xc800:
	return mask & (1 << r1);
    default:
	fail(__LINE__);
    }
    return 0;
}

int main(void) {
    if( test(0x5200L,0,0,0,-1) != 1 ) fail(__LINE__);
    if( test(0x5000L,0,0,0,-1) != 1 ) fail(__LINE__);
    if( test(0x5400L,0,0,0,-1) != 1 ) fail(__LINE__);
    _PASS;
}
