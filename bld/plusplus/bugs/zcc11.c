struct C {
    static int a;
    C( int );
    ~C();
    C operator +( C );
};

struct D {
    static int a;
    static void foo( C = ((a=1),C(1)+C(a)), C = C(2)+C(a), C = C(3)+C(a) );
};

void bar( void )
{
    int a = 8;
    D::foo();
    D::foo(4);
    D::foo(4,5);
    D::foo(4,5,6);
}

void sam( void )
{
    int a = 9;
    D::foo();
    D::foo(4);
    D::foo(4,5);
    D::foo(4,5,6);
}
