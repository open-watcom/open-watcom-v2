#include "fail.h"
//this was reduced down from stlport - it was crashing compiler

template < class T >
class A{
};

template< class T >
class B{
};

template<>
class B< A<bool> >{
};

B< A<bool> > b;

int main() {
    _PASS;
}
