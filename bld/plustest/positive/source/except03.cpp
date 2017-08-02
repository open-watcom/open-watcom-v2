#include "fail.h"
// 94/08/05 -- J.W.Welch -- disabled for medium model (need to upgrade
//                          typesig for function pointers)

#include <stdio.h>
#include <stdlib.h>

int foo_called;

void foo()
{
    ++foo_called;
}

int main()
{
    try {
        throw foo;
    } catch( void (*fn)() ) {
        fn();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( foo_called != 1 ) {
	fail(__LINE__);
    }
    _PASS;
}
