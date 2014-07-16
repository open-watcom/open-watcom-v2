#include "fail.h"

int main()
{
#if    !(65535UL < 4294967295UL)
	fail(__LINE__);
#else
#endif
#if    ((0xFFFFFFFFUL >> 1) == 0x7FFFFFFFUL)
#else
	fail(__LINE__);
#endif
#if 0x1111 && 0x8888
#else
	fail(__LINE__);
#endif
#if 0x1111 || 0x8888
#else
	fail(__LINE__);
#endif
    _PASS;
}
