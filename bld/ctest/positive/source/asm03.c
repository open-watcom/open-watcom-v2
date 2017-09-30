#include "fail.h"

int testfunc(int a,int b,int c,int d,int e,int f,int g,int h,int i,int j,int k,int l,int shadow)
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
#define testfuncASM()   _axp
#endif
#ifdef __MIPS__
    int _mips;
    _asm {
        lw  $v0,shadow
        sw  $v0,_mips
    }
#define testfuncASM()   _mips
#endif
#endif
    return testfuncASM();
}

int main()
{
    if( testfunc(1,2,3,4,5,6,1,2,3,4,5,6,0) != 0 ) fail(__LINE__);
    _PASS;
}
