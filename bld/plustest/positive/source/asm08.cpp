#include "fail.h"

int x;

int main() {
#if defined( __X86__ )
    unsigned short sh1;

    __asm mov sh1,ax
    x = 0;
#if defined( __386__ )
    __asm mov eax,x
#else
    __asm mov ax,x
#endif
    __asm _emit 0x83 ;
    __asm _emit 0xc0 ; add [e]ax,3
    __asm _emit 3    ;
    __asm _emit 83h  ;
    __asm _emit 0c0h ; add [e]ax,2
    __asm _emit 2    ;
#if defined( __386__ )
    __asm mov x,eax
#else
    __asm mov x,ax
#endif
    if( x != 5 ) _fail;
#elif defined( __AXP__ )
#elif defined( __PPC__ )
#endif
    _PASS;
}
