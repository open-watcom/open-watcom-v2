#include "fail.h"
#include <stddef.h>

#define UNICODIFY( quote ) L##quote

wchar_t *p = UNICODIFY("abcdefgh");
wchar_t *q = UNICODIFY("0123456789");

int main()
{
    int i;

    for( i = 0; i < 8; ++i ) {
	if( p[i] != ( 'a' + i ) ) fail(__LINE__);
    }
    for( i = 0; i < 10; ++i ) {
	if( q[i] != ( '0' + i ) ) fail(__LINE__);
    }
    _PASS;
}
