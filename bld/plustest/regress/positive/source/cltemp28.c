//caused a crash

#include "fail.h"


template<class T> class A;

template<class T>
struct B {
    A< B<T> > operator+( const T& ) const { return A< B<T> >(); };
    template< class U > friend A< B<U> > operator+( const U&, const B<U>& );
    template< class U > friend A< B<U> > operator* ( const U&, const B<U>& );
    template< class U > friend A< B<U> > operator* ( const B<U>&, const B<U>& );
};

template<class T> class A { };

class D : public B<int>{};

int main() 
{
    D v0;
    v0+1; 

    _PASS;
}
