enum { E1 };
 
int f3( int y ) { 
    y = ( y ? y : (int)E1 );
    return( y ? y : E1 );
} 

struct X8 {
    X8( X8 const &, int );
    void foo( int );
};

X8::X8( X8 const &, int = 0 )
{
}

void X8::foo( int = 1 ) {
}

void bar( X8 x );
void foo( X8 y )
{
    bar( y );
}

struct C {
};
struct S {
    operator C &();
};

void bar( C & );

C &foo( S &x )
{
    C &r = x;	// OK
    bar( x );	// OK
    return x;	// OK
}

struct T {
    operator C();
};

C &sam( T &x )
{
    C &r = x;	// error!
    bar( x );	// error!
    return x;	// error!
}

struct X59 {
    void foo();
    virtual void bar();
};

virtual void X59::foo()
{
}

X59 x68;

template <class T>
    struct X27 {
	T a;
    }

X27<int> *p;	// has to be last in file
