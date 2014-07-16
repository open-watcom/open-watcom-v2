#include <stdio.h>
#include "link6.h"

extern "C" void myfail( unsigned );
extern "C" void foo( int );

void try_catch()
{
    try {
	foo( 34 );
    } catch( Exception &r ) {
	if( r.a != 34 ) myfail(__LINE__);
    } catch( ... ) {
	myfail(__LINE__);
    }
    try {
	foo( 28 );
    } catch( Base &r ) {
	if( r.a != 28 ) myfail(__LINE__);
    } catch( ... ) {
	myfail(__LINE__);
    }
}
