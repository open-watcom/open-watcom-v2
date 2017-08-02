void __based(__segname("a")) b();
void __based(__segname("b")) b();

void __based(__segname("a")) a();
void __based(__segname("b")) __export a();


#if __WATCOM_REVISION__ >= 7
#define str( x ) #x
#define xstr( x ) str( x )
#define j( x, y )	x##y

char *_x12[] = {

xstr( j(
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
,
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
)
)

,


    str(
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    )
    };
#endif

#include <ctype.h>

struct x76 {
    int foo( int );
};

int x76:::foo( int x ) {
    return ::isalpha( x );
}

struct X84 {
    char foo() {
	return ''';
    }
    double bar() {
	return 1e+;
    }
};

struct x93 {
    typedef int mutable T;
    struct S {
	T x;
    };
    void foo( S const *p ) {
	p->x = 1;
    }
}

#define x103 1e+a
#define x104 '''

double x106 = x103;
char x107 = x104;

struct x109 {
    x109();
    explicit x109( int );
    explicit void operator +();
    explicit ~x109();
    explicit operator int();
    explicit void foo();
};

#define FOO /*
testing */	1

int hi = FOO;

void x123( char * );

void *x125;
#define x126 0
#define x126v ((void*)0)

char *i1 = x125;	// error
char *i2 = x126;
char *i3 = x126v;	// error

void x133() {
    char *a1;
    char *a2;
    char *a3;

    a1 = x125;	// error
    a2 = x126;
    a2 = x126v;	// error

    x123( x125 );	// error
    x123( x126 );
    x123( x126v );	// error
}

struct N {		// non-polymorphic
    struct A {};
    struct D1 : A {};
    struct D2 : A {};
    struct PRI {};
    struct PRO {};
    struct D : D1, D2, private PRI, protected PRO {};
};
struct P {		// polymorphic
    struct A { virtual void x10(){} };
    struct D1 : A {};
    struct D2 : A {};
    struct PRI { virtual void x13(){} };
    struct PRO { virtual void x14(){} };
    struct D : D1, D2, private PRI, protected PRO {};
};
struct U1;
struct U2;

/*
    legend:
	OK	- no diagnostic
    	error	- compile-time error
	fail	- call run-time (may fail)
*/
void x172( N::D *non_poly, P::D *poly, U2 *undef ) {
    {
	dynamic_cast< U1 * >( non_poly );	// target undef'd - error
	dynamic_cast< U1 * >( poly );		// target undef'd - error
	dynamic_cast< N::A * >( undef );	// source undef'd - error
	dynamic_cast< P::A * >( undef );	// source undef'd - error
    }
    {
	dynamic_cast< N::D * >( non_poly );	// same - OK
	dynamic_cast< P::D * >( poly );		// same - OK
	dynamic_cast< P::D * >( non_poly );	// unrelated - error
	dynamic_cast< N::D * >( poly );		// unrelated - fail
    }
    {
	dynamic_cast< N::A * >( non_poly );	// ambiguous - error
	dynamic_cast< P::A * >( poly );		// ambiguous - fail (warning)
	dynamic_cast< N::PRI * >( non_poly );	// private - error
	dynamic_cast< P::PRI * >( poly );	// private - fail (warning)
	dynamic_cast< N::PRO * >( non_poly );	// protected - error
	dynamic_cast< P::PRO * >( poly );	// protected - fail (warning)
    }
}
struct x194 {
    struct A { virtual void f(){} };
    struct D1 : A {};
    struct D2 : A {};
    struct D : D1, D2 {};
    struct E : private D {};
    struct F : protected D {};
    struct G : public D {};
    
    int error, ok;
    
    A *fd( D *p ) {
	++error;
	return dynamic_cast< A* >(p);
    }
    D *fe( E *p ) {
	++error;
	return dynamic_cast< D* >(p);
    }
    D *ff( F *p ) {
	++error;
	return dynamic_cast< D* >(p);
    }
    D *fg( G *p ) {
	++ok;
	return dynamic_cast< D* >(p);
    }
};
struct x222 {
    struct A {};
    struct D1 : A {};
    struct D2 : A {};
    struct D : D1, D2 {};
    struct E : private D {};
    struct F : protected D {};
    struct G : public D {};
    
    int error, ok;
    
    A *fd( D *p ) {
	++error;
	return dynamic_cast< A* >(p);
    }
    D *fe( E *p ) {
	++error;
	return dynamic_cast< D* >(p);
    }
    D *ff( F *p ) {
	++error;
	return dynamic_cast< D* >(p);
    }
    D *fg( G *p ) {
	++ok;
	return dynamic_cast< D* >(p);
    }
};

struct x251 {
    x251(int);
};

extern x251 x255;
int x256 = x255;	// = at 9

x258() {
    int i = x255;	// = at 13
    int j( x255 );
}

int *x263 = 1;
int *x264( 1 );
int *x265[] = {
    1, 2, 3, 0
};
struct x251 {
    x251(int);
};

struct x272 {
    int *q;
    x272( x251 x ) : q(4) {
    }
};

int x278( int, int = 0 );
int x278( int, double = 0 );

int x281( int x ) {
    return x278( x );
}
struct x284 {
    x284();
    x284( int = 0 );
};
x284 x288;

void x289( int, int    , int = 0 );
void x289( int, int = 0, int );
void x289( int, int = 0, int );
void x289( int, int    , int = 0 );
struct x294 {
    struct SD1 {
	SD1( int i = 0 );
	SD1();
	SD1( SD1 const & );
    };
    
    struct SC1 {
	SC1();
	SC1( int );
	SC1( SC1 const &, int i = 0 );
	SC1( SC1 const & );
    };
    
    void d1() {
	SD1 a1;
	SC1 b1;
    }
    
    SD1 ret_SD1( SD1 x ) {
	SD1 y(x);
	SD1 z(y);
	return z;
    }
    
    SC1 ret_SC1( SC1 x ) {
	SC1 y(x);
	SC1 z(y);
	return z;
    }
    
    struct SD2 {
	SD2();
	SD2( int i = 0 );
	SD2( SD2 const & );
    };
    
    struct SC2 {
	SC2();
	SC2( int );
	SC2( SC2 const & );
	SC2( SC2 const &, int i = 0 );
    };
    
    void d2() {
	SD2 a2;
	SC2 b2;
    }
    
    SD2 ret_SD2( SD2 x ) {
	SD2 y(x);
	SD2 z(y);
	return z;
    }
    
    SC2 ret_SC2( SC2 x ) {
	SC2 y(x);
	SC2 z(y);
	return z;
    }
};
class x355
{
        public:
                x355();
                x355(int a = 5);
                ~x355();
};

x355::x355()
{
}

x355::x355(int a)
{
}

x355::~x355()
{
}

void x375()
{
        x355 b;
}
struct x401 {
    struct S {
	virtual int p0();
	virtual int p1( int );
	virtual int p2( int, int );
	virtual int pe( int, int, ... );
    };
    struct D : virtual S {
	int p0();
	int p1( int );
	int p2( int, int );
	int pe( int, int, ... );
    };
    typedef int (S::* mp0)();
    typedef int (S::* mp1)( int );
    typedef int (S::* mp2)( int, int );
    typedef int (S::* mpe)( int, int, ... );
    
    void foo() {
	mp0 v0;
	mp1 v1;
	mp2 v2;
	mpe ve;
    
	v0 = (mp0) &S::p0;
	v0 = (mp0) &S::p1;
	v0 = (mp0) &S::p2;
	v0 = (mp0) &S::pe;
	v1 = (mp1) &S::p0;
	v1 = (mp1) &S::p1;
	v1 = (mp1) &S::p2;
	v1 = (mp1) &S::pe;
	v2 = (mp2) &S::p0;
	v2 = (mp2) &S::p1;
	v2 = (mp2) &S::p2;
	v2 = (mp2) &S::pe;
	ve = (mpe) &S::p0;
	ve = (mpe) &S::p1;
	ve = (mpe) &S::p2;
	ve = (mpe) &S::pe;
    
	v0 = (mp0) &D::p0;
	v0 = (mp0) &D::p1;
	v0 = (mp0) &D::p2;
	v0 = (mp0) &D::pe;
	v1 = (mp1) &D::p0;
	v1 = (mp1) &D::p1;
	v1 = (mp1) &D::p2;
	v1 = (mp1) &D::pe;
	v2 = (mp2) &D::p0;
	v2 = (mp2) &D::p1;
	v2 = (mp2) &D::p2;
	v2 = (mp2) &D::pe;
	ve = (mpe) &D::p0;
	ve = (mpe) &D::p1;
	ve = (mpe) &D::p2;
	ve = (mpe) &D::pe;
    }
};
