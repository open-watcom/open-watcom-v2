struct A { int a; };
struct B { int b; };

void foo( void )
{
    class B : A, B {
        B *p;
    };
}
