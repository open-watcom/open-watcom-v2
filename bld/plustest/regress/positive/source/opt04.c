#include "fail.h"
#include <stdio.h>

unsigned char ret_f0()
{
    return 0xf0;
}

int main()
{
    unsigned char x,y;

    x = ret_f0();
    if( ((int)(signed char) x) < 0 ) {
	if( (int)x < 0xc0 ) fail(__LINE__);
    } else {
	fail(__LINE__);
    }
    y = ret_f0();
    if( ((signed char) y) < 0 ) {
	if( y < 0xc0 ) fail(__LINE__);
    } else {
	fail(__LINE__);
    }
    _PASS;
}
