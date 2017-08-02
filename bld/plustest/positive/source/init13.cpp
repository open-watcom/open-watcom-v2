#include "fail.h"
#include <stddef.h>

wchar_t a[] = L"abcd";
char c[] = "abcd";
wchar_t e[] = { L"abcd" };
char f[] = { "abcd" };

void check( char *p, wchar_t *q )
{
    if( p[0] != 'a' ) fail(__LINE__);
    if( p[1] != 'b' ) fail(__LINE__);
    if( p[2] != 'c' ) fail(__LINE__);
    if( p[3] != 'd' ) fail(__LINE__);
    if( p[4] != '\0' ) fail(__LINE__);
    if( q[0] != 'a' ) fail(__LINE__);
    if( q[1] != 'b' ) fail(__LINE__);
    if( q[2] != 'c' ) fail(__LINE__);
    if( q[3] != 'd' ) fail(__LINE__);
    if( q[4] != '\0' ) fail(__LINE__);
}

int main()
{
    check( c, a );
    check( f, e );
    _PASS;
}
