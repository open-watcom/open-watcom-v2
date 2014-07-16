#include "fail.h"

template <class T> struct W;
template <class T> struct V : W<T> {
    W<T> *p;
    short x;
    V(short x=2) : p(this), x(x), W<T>(x) {
    }
};
template <class T> struct W {
    V<short> *q;
    short zz;
    W( short x = 3 ) : q( 0 ), zz(x) {
    }
};


int main() {
    W<double> *q = new W<double>( 5 );

    if( q->zz != 5 ) fail(__LINE__);
    if( q->q != 0 ) fail(__LINE__);
    V<short> x;
    if( x.x != 2 ) fail(__LINE__);
    if( x.zz != 2 ) fail(__LINE__);
    if( x.q != 0 ) fail(__LINE__);
    if( &x != x.p ) fail(__LINE__);
    if( x.p->zz != 2 ) fail(__LINE__);
    if( x.p->q != 0 ) fail(__LINE__);
    _PASS;
}
