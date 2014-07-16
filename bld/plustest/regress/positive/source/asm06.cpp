#include "fail.h"

// bug in parsing numeric constant tokens
int main() {
#ifdef __X86__
    unsigned int    x;

    _asm    mov x, 01e0fh;     // float number 1e0f + h 
    if( x != 0x1e0f ) _fail;
    _asm    mov x, 0e1fh;      // float number 0e1f + h
    if( x != 0x0e1f ) _fail;
    _asm    mov x, 0e1ah;      // float number 0e1 + ah
    if( x != 0x0e1a ) _fail;
    _asm    mov x, 001ah;      // octal number 001 + ah
    if( x != 0x001a ) _fail;
    _asm    mov x, 00e1ah;     // float number 0e1 + ah
    if( x != 0x0e1a ) _fail;
    _asm    mov x, 0070ah;     // octal number 070 + ah
    if( x != 0x070a ) _fail;
    _asm    mov x, 123ah;      // decimal number 123 + ah
    if( x != 0x123a ) _fail;
    _asm    mov x, 1a23h;      // decimal number 1 + a23h
    if( x != 0x1a23 ) _fail;
    _asm    mov x, 00701ah;    // octal number 0701 + ah
    if( x != 0x701a ) _fail;
#endif
    _PASS;
}
