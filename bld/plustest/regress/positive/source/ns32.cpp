#include "fail.h"

#ifdef __WATCOM_NAMESPACE__

namespace alpha {
    template <class T>
	inline T f (T const & a) {
	    return a + 1;
	}
    
    inline long b ( long a ) {
	return f ( a ) + 1;
    }
    
    static long q( long );
    static long r( long );
}

template <class T>
    inline T f (T const & a) {
	return a * 2;
    }

inline long t ( long a ) {
    return f ( a ) * 2;
}

int main ( void ) {
    long a = 100;
    long c;
    
    using namespace alpha;
    
    c = b ( a );
    if( c != 102 ) _fail;
    c = t( c );
    if( c != 408 ) _fail;
    if( q( c ) != 58 ) _fail;
    if( r( c ) != 2 ) _fail;
    _PASS;
}

long alpha::q( long a ) {
    return a / 7;
}

namespace alpha {
    long r( long a ) {
	return a % 7;
    }
}
#else
ALWAYS_PASS
#endif
