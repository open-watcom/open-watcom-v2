#include "fail.h"

template <class T>
    T inc0( T x ) {
	return x + 1;
    }
template <class T>
    T inc1( T x ) {
	return inc0( x );
    }
template <class T>
    T inc2( T x ) {
	return inc1( x );
    }
template <class T>
    T inc3( T x ) {
	return inc2( x );
    }
template <class T>
    T inc4( T x ) {
	return inc3( x );
    }

int foo( int i ) {
    return inc4(i);
}
#if __WATCOM_REVISION__ >= 8
namespace x {
    namespace y {
	template <class T>
	    T inc0( T x ) {
		return x + 1;
	    }
	template <class T>
	    T inc1( T x ) {
		return inc0( x );
	    }
	template <class T>
	    T inc2( T x ) {
		return inc1( x );
	    }
	template <class T>
	    T inc3( T x ) {
		return inc2( x );
	    }
	template <class T>
	    T inc4( T x ) {
		return inc3( x );
	    }
	
	int foo( int i ) {
	    return inc4(i);
	}
    }
    template <class T>
	T inc0( T x ) {
	    return x + 1;
	}
    template <class T>
	T inc1( T x ) {
	    return inc0( x );
	}
    template <class T>
	T inc2( T x ) {
	    return inc1( x );
	}
    template <class T>
	T inc3( T x ) {
	    return inc2( x );
	}
    template <class T>
	T inc4( T x ) {
	    return inc3( x );
	}
    
    int foo( int i ) {
	return inc4(i);
    }
}
#endif

int main() {
    if( foo( 1 ) != 2 ) fail(__LINE__);
#if __WATCOM_REVISION__ >= 8
    if( x::foo( 1 ) != 2 ) fail(__LINE__);
    if( x::y::foo( 1 ) != 2 ) fail(__LINE__);
#endif
    _PASS;
}
