// wpp -mc -3 -d1 -zff -s
#include "fail.h"

#if __WATCOM_REVISION__ >= 8
unsigned failed;

#if defined(__I86__)

void __far *c;

void _linget( unsigned long __far * );
#pragma aux _linget = \
	"pop ax" \
	"pop dx" \
	"push dx" \
	"push ax" \
	"cmp ax,word ptr c" \
	"jne L1" \
	"cmp dx,word ptr c+2" \
	"je L2" \
	"L1: inc word ptr failed" \
	"jmp L0" \
	"L2: push si" \
	"push es" \
	"mov si,ax" \
	"mov es,dx" \
	"cmp word ptr es:[si],5" \
	"jne L3" \
	"cmp word ptr es:2[si],0" \
	"je L4" \
	"L3: inc word ptr failed" \
	"L4: pop es" \
	"pop si" \
	"L0: xor ax,ax" /* CG doesn't push enough so we make sure things are OK */ \
	"push ax" \
	"push ax" \
	parm caller [] \
	modify [ax dx sp];

#elif defined(__386__)

void __far *c;

void _linget( unsigned long __far * );
#pragma aux _linget = \
	"pop eax" \
	"pop edx" \
	"push edx" \
	"push eax" \
	"cmp eax,dword ptr c" \
	"jne L1" \
	"cmp dx,word ptr c+4" \
	"je L2" \
	"L1: inc dword ptr failed" \
	"jmp L0" \
	"L2: push esi" \
	"push es" \
	"mov esi,eax" \
	"mov es,dx" \
	"cmp dword ptr es:[esi],5" \
	"je L4" \
	"L3: inc dword ptr failed" \
	"L4: pop es" \
	"pop esi" \
	"L0: xor eax,eax" /* CG doesn't push enough so we make sure things are OK */ \
	"push eax" \
	"push eax" \
	parm caller [] \
	modify [eax edx esp];


#else

void *c;

void _linget( unsigned long *p )
{
    if( p != c ) ++failed;
    if( *p != 5 ) ++failed;
}

#endif

typedef struct {
    unsigned long	ul;
} TestStr, *PTestStr;

void __saveregs test( PTestStr pOut )
{
    pOut->ul = 5;
    _linget( &pOut->ul );
    _linget( &pOut->ul );
}

int main() {
    TestStr v;

    c = &(v.ul);
    test( &v );
    if( failed ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
