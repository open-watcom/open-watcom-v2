#include "fail.h"

#ifdef __WATCOM_RTTI__

#include <typeinfo.h>

struct B1 {
    virtual void foo() {}
    int b1;
};
struct B2 {
    virtual void foo() {}
    int b2;
};
struct D : B1 {
    virtual void foo() {}
    int d;
};

B2 &test_ref( B1 &r ) {
    return dynamic_cast< B2 & >( r );
}
B2 *test_ptr( B1 *r ) {
    return dynamic_cast< B2 * >( r );
}

D d;
B2 b2;

unsigned ok;

int main() {
    try {
	// B2 not in hierarchy
	test_ref( d );
	_fail;
    } catch( exception const &w ) {
	++ok;
    } catch( ... ) {
	_fail;
    }
    try {
	B1 *p = 0;
	// reference == NULL
	test_ref( *p );
	_fail;
    } catch( exception const &w ) {
	++ok;
    } catch( ... ) {
	_fail;
    }
    try {
	// run-time type doesn't match what compiler thinks it is
	B1 *p = (B1*)&b2;
	test_ref( *p );
	_fail;
    } catch( exception const &w ) {
	++ok;
    } catch( ... ) {
	_fail;
    }
    try {
	// B2 not in hierarchy
	if( test_ptr( &d ) != NULL ) _fail;
	++ok;
    } catch( exception const &w ) {
	_fail;
    } catch( ... ) {
	_fail;
    }
    try {
	B1 *p = 0;
	// reference == NULL
	if( test_ptr( p ) != NULL ) _fail;
	++ok;
    } catch( exception const &w ) {
	_fail;
    } catch( ... ) {
	_fail;
    }
    try {
	// run-time type doesn't match what compiler thinks it is
	B1 *p = (B1*)&b2;
	if( test_ptr( p ) != NULL ) _fail;
	++ok;
    } catch( exception const &w ) {
	_fail;
    } catch( ... ) {
	_fail;
    }
    if( ok != 6 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
