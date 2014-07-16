#include "fail.h"

int main()
{
    char dud = 5;
#if __WATCOM_REVISION__ >= 8
    dud %= 256;		// This gives divide by zero error (wcc386 and wpp386)
#endif
    if( dud != 5 ) fail(__LINE__);
    _PASS;
}
