static int never_defined( int );

typedef int T;

struct S {
    T *p;
    void foo( T x )
    {
	static int bar( int );
	T y;
	++x;
	++y;
	never_defined( x );
    }
    struct T {
	int a;
    };
};

static int a;
static int a;

static int foo( int =0, int = 2 );

void bar( void )
{
    foo();
    foo(2);
    foo(3,4);
}

static int foo( int x, int y )
{
    return( x + y );
}
