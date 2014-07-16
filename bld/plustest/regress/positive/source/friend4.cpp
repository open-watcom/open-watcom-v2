#include "fail.h"

template<class T>
class B{
protected:
    T i;
    template< class U >
    friend U fn(const B<U>&);

    template< class U >
    friend U g( const B<U>& );
};

class A : public B<int> {
public:
    A( int ii ) { i = ii;}
};

int main()
{
    A a(9);
    if( 9 != fn(a) ) fail(__LINE__);
    if( 9 != g(a) ) fail(__LINE__);

    _PASS;
}


template< class T >
T g( const B<T> &b )
{
    return b.i;
}
