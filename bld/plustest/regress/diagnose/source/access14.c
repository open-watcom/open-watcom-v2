struct A {
    void foo( int );
    static void foo( int, int );
    void foo( double );
    int a;
};

struct B : A {
    int b;
};

struct C : A {
    int a;
};

struct D : B, C {
    int d;
};

void foo( D *p )
{
    p->foo( 1, 2 );
}
