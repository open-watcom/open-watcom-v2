// binding a ref to a generic type is req'd for STL.94\EXAMPLES\ITER3.CPP
#include "fail.h"

template <class T>
    struct S {
	T p;
	S( T x ) : p(x) {
	}
	T get() {
	    return p;
	}
    };

int z = 14;
S< int & > x = z;

template <class T>
    T foo( S<T> *p ) {
	return p->get();
    }

int bar() {
    return foo( &x );
}

int main() {
    if( bar() != 14 ) fail(__LINE__);
    z++;
    if( bar() != 15 ) fail(__LINE__);
    _PASS;
}
