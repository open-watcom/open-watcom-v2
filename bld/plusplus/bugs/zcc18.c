// generates fatal errors for MSC 7.0 and Zortech
struct C {
    C();
    C(C&);
    C operator +( C );
};

C a,b,c,d;

extern C foo( C = a + b, C = c + d );

void bar( void )
{
    foo( foo(), foo( foo() ) );
}
