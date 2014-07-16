#include "fail.h"
#include <stdio.h>


int x=1;

int bar()
{
    static int count=4;
    if( count < 0 ) fail(__LINE__);
    return count-- > 0;
}

int foo()
{
    int alpha = bar();
retry:
    int handle = bar();
    if( x ) {
        if( bar() ) {
            goto retry;
        }
    }
    return 0;
}

int main()
{
    foo();
    _PASS;
}
