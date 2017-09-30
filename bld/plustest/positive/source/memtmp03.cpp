#include "fail.h"

namespace ns
{
template< class T >
struct A
{
    struct AA
    {
        template< class U >
        struct AAB
        { };
    };

    int f()
    {
        return 1;
    }

    template< class U >
    struct B
    {
        template< class V >
        struct C
        {
            struct ABCD
            { };

            int f()
            {
                return 3;
            }
        };
    };
};
}

int main()
{
    ::ns::A<int> a;
    if( a.f() != 1 ) fail( __LINE__ );

    ::ns::A<int>::AA aa;

    ::ns::A<int>::AA::AAB<int> aab;

    ::ns::A<int>::B<char> b;

    ::ns::A<int>::B<char>::C<int> c;
    if( c.f() != 3 ) fail( __LINE__ );

    ::ns::A<int>::B<char>::C<int>::ABCD abcd;

    _PASS;
}
