struct T {
    static ~T();
};
struct S : T {
    int a;
    S();
    S( int x );
    ~S();
};
S::S()
{
    a = 0;
    return 0;
}
S::~S()
{
    ++a;
    return 0;
}
S::S( int x )
{
    a = x;
    return;
}
foo( int x )
{
    if( x ) {
	return x + 1;
    } else if( x + 1 ) {
	return;
    }
}
struct D;
struct B {
    B virtual *foo(void);
    virtual D *foo(int);
    virtual ~B();
};
struct D : B {
    D *foo(void);
    B *foo(int);
    virtual ~D();
};

void test_static_huge( void )
{
    static int huge a[65537];

    a[0] = 1;
}

typedef unsigned size_t;

int foo( int, int );
char foo( int, int );

union U {
    virtual void foo( void );
};

struct A {
    virtual static void sam( void );
    virtual A();
    virtual void *operator new( size_t );
};

struct B1 {
    virtual int foo( void );
    virtual B1 *foo( int, int );
};

struct D1 : B1 {
    float foo( void );
    virtual D1 *foo( int, int );
};

struct B2 {
    virtual void foo( int );
    virtual void foo( char );
    virtual void bar( int );
    virtual void bar( char );
};

struct D2 : B2 {
    virtual void foo( double );
    void bar( double );
};
