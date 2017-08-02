#include "fail.h"
#include <stdio.h>

#pragma inline_recursion(on)
#pragma inline_depth(4)

int /*inline*/ fact( int i )
{
    if( i == 0 ) {
        return 1;
    }
    return fact(i-1)*i;
}

#define mac_fact0( i ) ((i) ? (i) : 1)
#define mac_fact1( i ) ((i) ? (i)*mac_fact0(i-1) : 1)
#define mac_fact2( i ) ((i) ? (i)*mac_fact1(i-1) : 1)
#define mac_fact3( i ) ((i) ? (i)*mac_fact2(i-1) : 1)
#define mac_fact4( i ) ((i) ? (i)*mac_fact3(i-1) : 1)
#define mac_fact5( i ) ((i) ? (i)*mac_fact4(i-1) : 1)
#define mac_fact6( i ) ((i) ? (i)*mac_fact5(i-1) : 1)
#define mac_fact7( i ) ((i) ? (i)*mac_fact6(i-1) : 1)
#define mac_fact8( i ) ((i) ? (i)*mac_fact7(i-1) : 1)
#define mac_fact9( i ) ((i) ? (i)*mac_fact8(i-1) : 1)
#define mac_fact10( i ) ((i) ? (i)*mac_fact9(i-1) : 1)

int main() {
    if( fact(1) != mac_fact10(1) ) fail(__LINE__);
    if( fact(2) != mac_fact10(2) ) fail(__LINE__);
    if( fact(3) != mac_fact10(3) ) fail(__LINE__);
    if( fact(4) != mac_fact10(4) ) fail(__LINE__);
    if( fact(5) != mac_fact10(5) ) fail(__LINE__);
    if( fact(6) != mac_fact10(6) ) fail(__LINE__);
    _PASS;
}
