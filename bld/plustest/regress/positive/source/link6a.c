#include "fail.h"
#include <stdio.h>
#include "link6.h"

extern "C" void myfail( unsigned line )
{
    fail(line);
}

extern "C" void foo( int x )
{
    Exception z(x);

    throw z;
}

extern void try_catch();

int main()
{
    try {
	try_catch();
    } catch( ... ) {
	fail( __LINE__ );
    }
    _PASS;
}
