#include "fail.h"

#if __WATCOM_REVISION__ <= 5
#pragma inline_depth(0)
#endif

int testfunc(int,int,int,int,int,int,int,int,int,int,int,int,int shadow)
{
#ifdef __X86__
#ifdef __386__
    extern int testfuncASM( void );
    #pragma aux testfuncASM = \
    "mov eax,shadow"\
    value [eax];
#else
    extern int testfuncASM( void );
    #pragma aux testfuncASM = \
    "mov ax,shadow"\
    value [ax];
#endif
#else
#ifdef __AXP__
    int _axp;
    _asm {
	ldl $v0,shadow
	stl $v0,_axp
    }
#define testfuncASM()	_axp
#endif
#endif
    return testfuncASM();
}

int main()
{
    if( testfunc(1,2,3,4,5,6,1,2,3,4,5,6,0) != 0 ) fail(__LINE__);
    _PASS;
}
