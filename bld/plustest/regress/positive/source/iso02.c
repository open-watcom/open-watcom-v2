#include "fail.h"

template <class T>
    struct equiv {
	friend int operator ==( const T &l, const T &r ) {
	    return l.eq( r );
	}
    };

int count;

struct X : equiv<X> {
    int eq( X const &m ) const {
	return sig == m.sig;
    }
    int sig;
    X() : sig(++count) {
    }
    void same( X const &x ) {
	sig = x.sig;
    }
};

int main() {
    X a1;
    X a2;
    X a3;

    if( a1 == a2 ) fail(__LINE__);
    if( a1 == a3 ) fail(__LINE__);
    if( a2 == a3 ) fail(__LINE__);
    a2.same( a1 );
    if( ! ( a1 == a2 ) ) fail(__LINE__);
    _PASS;
}
