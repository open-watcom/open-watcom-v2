#include "fail.h"

// see bugzilla #63

struct A
{
    A()
    { }

    struct B
    {
        B()
        { }

        ~B()
        { }

        struct C
        {
            ~C()
            { }
        } c;
    } b;

    struct D
    {
        D()
        { }

        ~D()
        { }

        struct E
        {
            ~E()
            { }
        } e;
    } d;

    ~A()
    { }
};

template< class T >
struct D
{
    ~D()
    { }
};

struct E
{
    E()
    { }

    struct F
    {
        D<int> d;

        ~F()
        { }
    } f;

    struct G
    {
        D<short> d;

        ~G()
        { }
    } g;
};

int main()
{
    A a;
    E e;

    _PASS;
}
