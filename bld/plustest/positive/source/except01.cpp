#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    for( int i = 0; i < 2; ++i ) {
	try {
	    if( i == 0 ) {
		throw (int **) 0;
	    } else {
		throw (int ***) 0;
	    }
	} catch( int ** ) {
	    if( i != 0 ) {
		fail(__LINE__);
	    }
	} catch( int *** ) {
	    if( i != 1 ) {
		fail(__LINE__);
	    }
	}
    }
    _PASS;
}
