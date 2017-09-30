#include "fail.h"

// fatal compiler error

template< class T > class A;

class BN {};

template < class T >
class N : public BN{
    friend class A<T>;
    T data;
};

template < class T >
class A {
public:
    A(){};
    virtual T f(BN* np) ;
};

template < class T >
T A<T>::f( BN* np )  {
    return ((const N<T>*) np)->data;
}


int main (void) {
    A<int> a;
    //N<int> n;  //no bug if include this
    _PASS;
}

