#include "fail.h"

template <class T>
    struct S {
	static int s0;
	static int s1;
    };

template <class T>
    int S<T>::s0;

template <class T>
    int S<T>::s1;

int main() {
    S<int> x;
    x.s0++;
    x.s1++;
    if( x.s0 != 1 ) _fail;
    if( x.s1 != 1 ) _fail;
    _PASS;
}
