#include <stdio.h>

struct A {
    int a;
    ~A();
};
A::~A() {
    printf( "dtor %d\n", a );
}

A a1 = { 1 };
static A a2 = { 2 };

void f1( void ) {
    A a3 = { 3 };
    printf( "f1: %d\n", a3.a );
}
void f2( void ) {
    static A a4 = { 4 };
    printf( "f2: %d\n", a4.a );
}

int main( void )
{
    f1();
    f2();
    printf( "main: %d %d\n", a1.a, a2.a );
    return 0;
}

