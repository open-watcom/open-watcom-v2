struct X {
    X();
    ~X();
    X & operator =( X & );
};

struct B : X {
};

struct C : B, X {
};

C z;
C *p = &z;

void foo( void )
{
    z = *p;
}

typedef int fn12( int = 1, int = 2 );

fn12 x;
fn12 y;

void bar( void )
{
    x();
    y(3);
}
