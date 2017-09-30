#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace x {
    template <class T>
	struct S {
	    T x[10];
	    T set( T i, T v ) {
		v ^= x[i];
		x[i] ^= v;
		v ^= x[i];
		return v;
	    }
	};
    namespace q {
	int i = 'i';
    };
    typedef enum E { A, B, C } T;
    int E;
};

using x::q;
using x::T;
using x::S;
using x::E;

int main() {
    if( &q::i != &x::q::i ) fail(__LINE__);
    if( q::i != 'i' ) fail(__LINE__);
    size_t x = sizeof( T );
    if( x != sizeof( x::T ) ) fail(__LINE__);
    T e;
    e = x::A;
    e = x::B;
    enum E f;
    f = x::A;
    f = x::B;
    S<short> zz;
    for( int i = 0; i < 10; ++i ) {
	zz.set( i, -i );
    }
    for( i = 0; i < 10; ++i ) {
	if( zz.set( i, i ) != -i ) fail(__LINE__);
    }
    for( i = 0; i < 10; ++i ) {
	if( zz.set( i, -i ) != i ) fail(__LINE__);
    }
    _PASS;
}
#else
ALWAYS_PASS
#endif
