#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct S {
    int a;
    S( int x ) : a(x) {}
};

const S &r = -1;

void clean_stack()
{
    auto char a[1024];

    memset( a, 0, 1024 );
}

int main()
{
    clean_stack();
    if( r.a != -1 ) fail(__LINE__);
    _PASS;
}
