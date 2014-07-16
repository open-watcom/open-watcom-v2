#include "fail.h"

namespace
{
    typedef int AC;
}

namespace ns
{
    template< class T>
    struct A
    { };

    template< class T >
    inline bool operator ==( A<T> const & x, T const * y )
    {
        return true;
    }

    typedef A< char > AC;
}

void f()
{
    using namespace ns;
    ns::AC a;

    const bool b = (a == "");
}

void g()
{
    AC a;

    a = 1;
}

int main( )
{
    f();
    g();

    _PASS;
} 
