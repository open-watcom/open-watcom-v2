#include "fail.h"

typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

union s_ebx {
   dword _ebx;
   word  _bx;
   struct {
       byte  _bl;
       byte  _bh;
   };
};

void foo( s_ebx *p )
{
    p->_ebx = 0x01020304;
    p->_bx = 0x0506;
    p->_bh = 0x07;
    p->_bl = 0x08;
}

int main()
{
    s_ebx reg;

    foo( &reg );
    if( reg._ebx != 0x01020708 ) fail(__LINE__);
    if( reg._bx != 0x708 ) fail(__LINE__);
    if( reg._bh != 0x07 ) fail(__LINE__);
    if( reg._bl != 0x08 ) fail(__LINE__);
    _PASS;
}
