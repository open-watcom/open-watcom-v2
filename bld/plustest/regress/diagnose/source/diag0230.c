#define euniq(xx) QQ##xx
#define uniq(xx) euniq(xx)
#define U uniq(__LINE__)
struct X1 {
    int f( int i )
    {
	return i;
    }
    int f( int& r )	// error: not sufficiently different
    {
	return r;
    }
    void g( int i )
    {
	f( i );	// if it doesn't complain above,
		    // it should at least complain here
    }
};
struct X20 {
    void f1( int );
    void f2( int& );
    void f3( const int& );
    void g( void )
    {
	f1( 2.2 );	// ok
	f2( 2.2 );	// error: temporary needed to init nonconst ref
	f3( 2.2 );	// ok, temporary used
    }
    void h( void )
    {
	int a = int(2.2);
	f2( a );		// ok
    }
};
struct X36 { };
struct Y37 { };

X36 operator + ( const X36&, const Y37& );
Y37 operator + ( const X36&, const Y37& );	// error: functions differ only in
					// return types
void x42( X36 r1, Y37 r2 );
void U( X36 a, Y37 b )
{
    X36 r1 = a + b;	// error: ambiguous
    Y37 r2 = a + b;	// error: ambiguous
    x42( r1, r2 );	// required addition to circumvent optimizer
}
typedef int T48;

void x50( int i ) { i++; };
void x50( T48 i ) { i++; };

enum E53 { a };

void g56( int i ) { i++; };
void g56( E53   i ) { i=i; }; // ok
// both handle this correctly
void x58( char * )	{};
void x58( char [] )	{};	// error: same as char *
void x58( char [7] )	{};	// error: same as char *
void x58( char [9] )	{};	// error: same as char *

void x63( char (*)[10] )	{};
void x63( char [5][10] )	{};	// error: same as g( char (*)[10] );
void x63( char [7][10] )	{};	// error: same as g( char (*)[10] );
void x63( char (*)[20] )	{};	// ok, different from g( char (*)[10] );
struct X67 {
    // both handle this correctly
    class B {
    public:
	int f( int );
    };
    class D : public B {
    public:
	int f( char * );
    };
    void h( D* pd )
    {
	pd->f( 1 );		//error: D::f(char *) hides B::f(int)
	pd->B::f( 1 );	// ok
	pd->f( "Ben" );	// ok
    }
};
struct x86 {
    struct B {
	void operator= ( int i );
	B( int i );
    };
    struct D : B {
    };
    void f( void )
    {
	D x = 1;	// error: not in scope
	x = 2;		// error: not in scope
    }
    void x98( float, int );
    void x98( int, float );
    void x101( void )
    {
	x98( 1, 1 );	// error: ambiguous resolution
    }
    void x102( int, double );
    void x102( double, int );
    void x102( void )
    {
	x102( 1, 1 );	// error: ambiguous
    }
};
struct x109 {
    struct complex {
	complex( double );
    };
    void h( int, complex );
    void h( double, double );
    void hh( void )
    {
	h( 3, 4 );	// used to be ok: h( double( 3 ), double( 4 ) );
		    // now ambiguous
    }
};
struct x121 {
    struct complex {
	complex( double );
    };
    void h( int, complex, complex = 0 );
    void h( double, double, complex = 0 );
    void hh( void )
    {
	h( 3, 4 );		// used to be ok: h( double( 3 ), double( 4 ) );
			    // now ambiguous
    }
};
struct x133 {
    struct complex {
	complex( double );
    };
    void h( int, complex, complex = 0 );
    void h( double, double, complex = 0 );
    void hh( void )
    {
	h( 3, 4, 0 );	// ambiguous
    }
    int f( void );
    int f( int i = 1 );	// error, potentially ambiguous
    void g( void )
    {
	f( 11 );	// ok
    }
};
struct x150 {
    int f( );
    int f( int i = 1 );
    
    void g( void )
    {
	f();	// error: ambiguous
    }
};
class X159 {
    public:
	void f();
};
class Y163 {
    public:
    	operator X159();
};
void U( void )
{
    Y163 a;
    a.f();
}
struct x172 {
    void f( char * );
    void f( const char * );
    void f2( char * );
    void g( char *pc, const char *pcc )
    {
	f( pc );	// f(char *)
	f( pcc );	// f(const char *)
    
	f2( pc );	// f2(char *)
	f2( pcc );	// error: cannot initialize 'char *' with 'const char *'
    
    }
    void x191( char );
    void x191( float );
    
    void U( void )
    {
	x191( 1 );
	x191( 1L );
    }
};
struct x194 {
    void f( char& );
    void f( short );
    
    void g( void )
    {
	f( 'c' );	// error: f(char&) requires temporary which
		    // is illegal since f() is not 'const char &'
    }
};
struct x204 {
    void f( char );
    void f( double );
    
    void g( void )
    {
	f( 'a' );	// f(char)
	f( 0 );	// error: ambiguous: f(char) or f(double) ?
    }
};
struct x214 {
    class A {};
    class B : public A {};
    class C : public B {};
    
    void g( A * );
    void g( B * );
    
    C cc;
    
    void f( void )
    {
	g( &cc );	// g( B * )
    }
    
    void h( void * );
    void h( A * );
    
    void hh( void )
    {
	h( &cc );	// h(A*)
	h( 0 );	// error: ambiguous, h(void *) or h(A *)?
    }
};
struct x238 {
    void f( int );
    void f( int ... );
    void f( int, char * ... );
    
    void g( void )
    {
	f( 1 );		// error: ambiguous f(int) or f(int ...)
	f( 1, 2 );		// ok f(int ...)
	f( 1, "asdf" );	// ok f(int, char * ...)
	f( 1, "asdf", 2 );	// ok f(int, char * ...)
    }
};
struct x251 {
    // both handle this correctly
    class Y {
	public:
	    operator int ();
	    operator double ();
    };
    
    void f( Y y )
    {
	int i = y;		// Y::operator int ()
	double d = y;	// Y::operator double()
	float f = y;	// error: ambiguous
    }
};
struct x266 {
    class X {
	public:
	    X( int );
    };
    class Y {
	public:
	    Y( long );
    };
    void f( X );
    void f( Y );
    void g( void )
    {
	f( 1 ); 	// ambiguous
    }
};

    struct x282 {
	operator int ();
    };
    struct y285 {
	y285( x282 );
    };
    y285 operator+ ( y285, y285 );
    void U( x282 a, x282 b )
    {
	a + b;	// error: ambiguous
		    //	operator+ ( y285(a), y285(b) ) or
		    //	a.operator int() + b.operator int()
    }

int x298( double );
int x298( int );
int (*U)( double ) = &x298;
int (*U)( int ) = &x298;
int (*U)( ... ) = &x298;		// error: type mismatch
int x302( double );
int x302( int );
int (*U)( double ) = &x302;
int (*U)( int ) = &x302;

int x307( int );
int (*U)( ... ) = (int (*)( ... ))&x307;	// ok

int x307( int ... );
int (*U)( int, int ) = &x307;	// error: type mismatch
class X312 {};
class x313 {};
class x314 : public X312, public x313 {};

x314 *x316( void );
x313 *(*U)( void ) = &x316;	// error

x314 *x319( void );

void x321( x314 * );
void (*U)( x313 * ) = &x321;	// error
struct x323 {
    int operator! ();
    int g1( void );
};
struct x327 : x323 {
    int g2( void );
};

int operator!( x327& );

int U( x327& d )
{
    return !d;
}

int operator!( x323& );

int U( x323& b )
{
    return !b;	// error: ambiguous
}
struct X344 {
    int operator! ();
    int g1( void );
};
struct X348 : X344 {
    int g2( void );
};

int operator!( X348& );

int U( X348& d )
{
    return !d;
}

int operator!( X344& );

int U( X344& b )
{
    return !b;	// error: ambiguous
}

int X348::g2( void )
{
    return !*this;	// ::operator!(X348&)
}

int X344::g1( void )
{
    return !*this;	// error: ambiguous
}
class x375 {
    public:
    	x375 operator+ ( int );
};
x375 operator+( x375, double );

void U( x375 b )
{
    x375 a;
    a = b + 1;		// X::operator+( int )
    a = b + 1.0;	// ::operator+( X, double )
    a = b + 1L;		// error: ambiguous
    			// ztc resolves to ::operator+( X, double )
}
class x389 {};

void U( void )
{
    x389 a,b;
    a = b;			// default meaning of assignment
}

x389& operator=( x389&, const x389& );	// error: '=' must be a member function
struct x398 {
    int m;
};

class x398ptr {
    	x398 *p;
	// other nifty information
    public:
    	x398ptr( const char *arg );
	x398 *operator->( void );
};

x398ptr::x398ptr( const char *arg )
{
    p = 0;
    // store away information based on 'arg'
}

x398 *x398ptr::operator->( void )
{
    if( p ) {
	// check p
	// update information
    }
    else {
    	// init p using information
    }
    return( p );
}

void U( x398ptr y, x398ptr &yr, x398ptr *yp )
{
    int i = y->m;	// y.operator->()->m
    i = yr->m;		// yr.operator->()->m
    i = yp->m;		// error: Yptr does not have a member m
}
