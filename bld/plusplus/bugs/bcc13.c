// test case to show possible failure with "fast" member pointers
// Borland fails
struct A {
    char a;
};
struct B {
    char b0[65535-1];
    char b1;
};
struct C : A, B {
};

char B::* p = &C::b1;
char C::* q;

C *pc;

void foo( void )
{
    q = p;
    if( q ) {
        pc->*q = 'o';   // OK
    } else {
        pc->*p = 'f';   // FAIL
    }
}
