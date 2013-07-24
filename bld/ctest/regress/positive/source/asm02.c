#include "fail.h"

#define INSTR_NOP   0x90

extern int a;

extern void bar( int );

#if defined(__I86__)

#pragma aux bar = INSTR_NOP "mov a,ax" parm [ax];

#elif defined(__386__)

#pragma aux bar = INSTR_NOP "mov a,eax" parm [eax];

#else

void bar( int x ) {
    a = x;
}

#endif

int a = 1;

int main() {
    if( a != 1 ) fail(__LINE__);
    bar( __LINE__ ); if( a != __LINE__ ) fail(__LINE__);
    _PASS;
}
