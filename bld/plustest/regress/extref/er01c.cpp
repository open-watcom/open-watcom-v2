#include "er01.h"

int c( void ) {
    int r = 0;
    try {
	throw_X();
    } catch( X &c ) {
	r = c.x;
    }
    return( r );
}
