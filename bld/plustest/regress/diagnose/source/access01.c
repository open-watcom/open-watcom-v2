// page 202 ARM
struct A {
    int a;
    int f( int );
    int g( char );
};

class B {
    int a;
public:
    int f( int );
    int h( double );
};

class C : public A, public B {
};

void foo( C *p )
{
    p->a = 1;		// ambiguous
    p->f( 1 );		// ambiguous
    p->g( 'a' );	// OK
    p->h( 1.0 );	// OK
}
