struct B1 {
    virtual int foo( void ) = 0;
    virtual int foo( double ) = 0;
    virtual ~B1();
};
struct B2 {
    virtual int foo( void ) = 0;
    virtual int foo( char ) = 0;
    virtual int bar( void ) = 0;
    virtual ~B2();
};
struct B12 : B1, B2 {
    virtual int foo( void );
    virtual ~B12();
};
struct B3 : B12 {
    virtual int foo( char );
    virtual ~B3();
};
struct D : B3 {
    virtual int foo( double );
    virtual int bar( void );
    virtual ~D();
};
B1 ac1;
B2 ac2;
B12 ac3;
B3 ac4;
D c5;

struct S;

extern struct S foo( struct S );

S z;

struct S bar( struct S x, int i )
{
    S y;
    y = S(i);
    y = (S)(i);
    i = y;
    return foo(x);
}

struct T {
    S q;
    T() {}
};

S x;

S a[1];

struct S {
    virtual void foo( void ) = 0;
    S(int=1);
};

extern S bar( S );

S y;

S b[1];

struct S sam( struct S x, int i )
{
    S y;
    y = S(i);
    y = (S)(i);
    i = y;
    return foo(x);
}
