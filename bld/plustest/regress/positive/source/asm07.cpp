#include "fail.h"

// preprocesor substitution and directive processing test for _asm

#define MOV_X_CONST(a) mov x,a

#define NUMBER_0A1BH 0a1bh

#define mmx  .MMX
#define sse3 .XMM3

struct S {
    void f() {
#ifdef __X86__
        unsigned int    x;

        _asm    .586 ;
        _asm    mmx

        _asm    mov x, 0a1bh
        if( x != 0x0a1b ) _fail;
        _asm    MOV_X_CONST( 0a1bh );
        if( x != 0x0a1b ) _fail;
#endif
    }
};

int main() {
#ifdef __X86__
    unsigned int    x = 0;
    S               y;

    _asm    .8086   ; comment 1
    _asm    .386    // comment 2
    _asm    .586    /* comment 3 */
    _asm    mmx
    _asm    .K3D
    _asm    .686
    _asm    .XMM
    _asm    .XMM2  ;
    _asm    sse3

    _asm {
        .686
        .XMM3
        jmp l1
        addsubpd xmm1,xmm3
        l1:
    }
    _asm mov x,1 /* comment 3 */ _asm add x,2 /* comment 3 */ _asm add x,3 /* comment 3 */
    if( x != 6 ) _fail;
    _asm    mov x,1   ; comment 1
    _asm    add x,2   // comment 2
    _asm    add x,3   /* comment 3 */
    if( x != 6 ) _fail;
    y.f();
    _asm    MOV_X_CONST( 0a1bh );
    if( x != 0x0a1b ) _fail;
    _asm    MOV_X_CONST( NUMBER_0A1BH );
    if( x != 0x0a1b ) _fail;
#endif
    _PASS;
}
