// page 203 ARM
struct A {
    enum { E1, E2 };
    static int s;
};

struct B : A {
};

struct C : A, B {
};

int foo( void )
{
    return( C::s + (int) C::E1 );	// OK
}
