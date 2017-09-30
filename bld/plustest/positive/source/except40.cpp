#include "fail.h"

struct Except {
};
struct ExceptA : Except {
};
struct ExceptB : Except {
};
struct ExceptC : Except {
};
	

void do_throw( int which ) {
    switch( which ) {
    case 'a':
	throw ExceptA();
	break;
    case 'b':
	throw ExceptB();
	break;
    default:
	throw ExceptC();
    }
}

unsigned common;
unsigned do_A;
unsigned do_B;
unsigned do_UNK;

void test( int which ) {
    try {
	// exception prone code here, that may do a throw
	do_throw( which );
    } catch (...) {
	// common error code here
	++common;
	try {
	    throw;  // re-throw to more specific handler
	} catch (ExceptA&) {
	    // handle ExceptA here
	    ++do_A;
	} catch (ExceptB&) {
	    // handle ExceptB here
	    ++do_B;
	} catch (...) {
	    // handle unknown exceptions here
	    ++do_UNK;
	}
	throw;
    }
}

int main() {
    try { test( 'a' ); } catch( Except & ) {} catch( ... ) {_fail;}
    try { test( 'b' ); } catch( Except & ) {} catch( ... ) {_fail;}
    try { test( 'c' ); } catch( Except & ) {} catch( ... ) {_fail;}
    if( common != 3 ) _fail;
    if( do_A != 1 ) _fail;
    if( do_B != 1 ) _fail;
    if( do_UNK != 1 ) _fail;
    _PASS;
}
