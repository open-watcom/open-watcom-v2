struct AMBIG {
    static int foo( int, int );
    void foo( char, int );
};

struct BASE : AMBIG {
};

struct DERIVED : BASE, AMBIG {
};

DERIVED *p;

void bar( void )
{
    p->foo( 1, 0 );		// OK!
    p->foo( 'a', 0 );		// ambiguous
}

extern int a( int, int (*p)[3] );
extern int a( int, int (*p)[] );
extern int a( int, int p[][] );
struct A {
    operator A ();
    A * operator ->();
};
struct B {
    operator B & ();
    A operator ->();
};
struct C {
    B operator ->();
    int a;
};
struct S {
    S operator ->();
    int a;
};
struct Q;
struct T {
    Q operator ->();
    int a;
};
struct Q {
    T operator ->();
    int a;
};

struct OK1 {
    int a;
};
struct OK2 {
    OK1 *operator ->();
    int a;
};
struct OK3 {
    OK2 operator ->();
    int a;
};

OK1 *pOK1;
OK2 pOK2;
OK3 pOK3;

void foo( void )
{
    pOK1->a = 1;
    pOK2->a = 1;
    pOK3->a = 1;
}
