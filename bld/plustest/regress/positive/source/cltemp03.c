#include "fail.h"

template <class T>
    struct S {
	static int si;
	static int sj;
    };

template <class T>
    int S<T>::si, S<T>::sj;

struct B {
    static int bi;
    static int bj;
};

int B::bi,B::bj;

void foo( S<int> *p ) {
    p->si = 1;
    p->sj = 2;
}

int main() {
    S<int> x;

    foo( &x );
    if( x.si != 1 ) fail(__LINE__);
    if( x.sj != 2 ) fail(__LINE__);
    if( S<int>::si != 1 ) fail(__LINE__);
    if( S<int>::sj != 2 ) fail(__LINE__);
    if( B::bi != 0 ) fail(__LINE__);
    if( ++B::bj != 1 ) fail(__LINE__);
    _PASS;
}
