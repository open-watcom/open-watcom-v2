struct xn1 {
    int bool;
};

typedef int bool;
#define X0 23 // comment start \
comment \
end

int x5 = X0; // comment start ...\
comment end
char *x1 = "\777\677\577\477\377\277\177";
char x2 = '\777';
struct x3 {
    struct C {
	C();
    };
    struct D {
	~D();
    };
    typedef struct {
	C x;
    } U1;
    typedef struct {
	D x;
    } U2;
    typedef struct {
	C y;
	D x;
    } U3;
    typedef struct : C {
    } U4;
    typedef struct : D {
    } U5;
    typedef struct : C, D {
    } U6;
};
struct x27 {
    unsigned char uc;
    unsigned short us;
    unsigned int ui;
    unsigned long ul;
    float f;
    double d;
    
    int foo() {
	int x = ( uc != 65.25 );
	int y = ( f == 0.1 );
	int z = ( d == 0.1f );
	return x||y||z;
    }
    void use( int );
    void bar() {
	use( uc < 0 );	// always false
	use( us < 0 );
	use( ui < 0 );
	use( ul < 0 );
	use( uc >= 0 ); // always true
	use( us >= 0 );
	use( ui >= 0 );
	use( ul >= 0 );
	use( 0 > uc ); // always false
	use( 0 > us );
	use( 0 > ui );
	use( 0 > ul );
	use( 0 <= uc ); // always true
	use( 0 <= us );
	use( 0 <= ui );
	use( 0 <= ul );
	use( ++uc < 0 );	// always false
	use( ++us < 0 );
	use( ++ui < 0 );
	use( ++ul < 0 );
	use( ++uc >= 0 ); // always true
	use( ++us >= 0 );
	use( ++ui >= 0 );
	use( ++ul >= 0 );
	use( 0 > ++uc ); // always false
	use( 0 > ++us );
	use( 0 > ++ui );
	use( 0 > ++ul );
	use( 0 <= ++uc ); // always true
	use( 0 <= ++us );
	use( 0 <= ++ui );
	use( 0 <= ++ul );
	use( uc-- < 0 );	// always false
	use( us-- < 0 );
	use( ui-- < 0 );
	use( ul-- < 0 );
	use( uc-- >= 0 ); // always true
	use( us-- >= 0 );
	use( ui-- >= 0 );
	use( ul-- >= 0 );
	use( 0 > uc-- ); // always false
	use( 0 > us-- );
	use( 0 > ui-- );
	use( 0 > ul-- );
	use( 0 <= uc-- ); // always true
	use( 0 <= us-- );
	use( 0 <= ui-- );
	use( 0 <= ul-- );
    }
};
struct x93 {
    unsigned _year;
    unsigned year();
    unsigned month();
    static unsigned (x93::* a[])();
};
unsigned (x93::* a[])() = { year, month };
void x97( x93 *p, x93 * q )
{
    if( p->year > q->year ) {
	if( p->year == q->year ) {
	}
    }
}
struct x118;
void foo( x118 );
void bar( x118 & );
void t( x118 x ) {
    extern x118 &z;
    x118 y;
    foo( x );
    foo( y );
    foo( z );
    bar( x );
    bar( y );
    bar( z );
}
void f131() {
    x118();
    	x118();
		x118();
			x118();
}
class x131;
class x132;

struct x134 {
    static x131 b;
    static const x131 & f() { return(b); } // should work
    static const x132 & g() { return(b); } // should not work
};

x131 const & x140( x134 x )
{

    x131();

    x132();

    x131 q = x132();

    x131 &r = x131();

    x131 &s = x132();


    return x.f();
}
struct x162 {
    struct S;
    int foo( S *, char );
    struct S {
	struct Z {
	    int foo( S * );
	};
	int foo( S * );
	int foo( S *, char );
	friend int foo( S * );
	friend int foo( S *, char );
	friend class Z;
    private:
	int x;
    };
};

int foo( x162::S *p )
{
    return p->x;
}

int x162::foo( S *p, char )
{
    return p->x;
}

int x162::S::Z::foo( S *p )
{
    return p->x;
}
#if 0
#ifdef ack$2
#endif
#ifndef ack$$M#
#endif
#else
#if 1
int __u;
#elif 1$3
#endif
#endif
..\h
int x;;;;;;;;;;;;;;;;;;
..\  ?
int x;;;;;;;;;;;;;;;;;;
..\  x
int x;;;;;;;;;;;;;;;;;;
..?a
int x;;;;;;;;;;;;;;;;;;
..??b
int x;;;;;;;;;;;;;;;;;;
..??-c
int x;;;;;;;;;;;;;;;;;;
extern "C" {
    struct X217 {
	int foo( int );
    };
    typedef int ( X217::* mp220 )( int );
    mp220 x = &X217::foo;
};
struct x223 {
    operator = ( x223 const & );
    operator += ( x223 const & );
};
// omit later
int __declspec(dllimport) x_i228;

extern int x_i228;

extern int __declspec(dllexport) y_x228;

int y_x228;

int __declspec(dllimport) f_i228( int );

extern int f_i228( int );

int __declspec(dllexport) g_x228( int );

int g_x228( int );

// add later
int h_x228( int );

int __declspec(dllexport) h_x228( int )
{
    return( __LINE__ );
}

extern int k_x228;

int __declspec(dllexport) k_x228 = __LINE__;

void x256(void *a)
{
	double	b;
	char	*c;

	b = (*(double (*)(char *, double))a)("X", 3.141);	// OK
	b = (*(double (*)(char *, double))a)("X");		// not enough args
	b = (*(double (*)(char *, double))a)(3.141, "X");	// wrong parm types
	c = (*(double (*)(char *, double))a)("X", 3.141);	// OK
}

void x267( char *, char * );
#define x268( a, b )	x267( #a, #b )


void x271() {
    x268(a,b);
    x268(,c);
    x268(d,);
    x268(,);

    x268(a,b,);
    x268(,,);
    x268(,,,);
    x268(a,b,);
    x268(,,);
    x268(,,,);
    x268(a,b,c,d);
    x268(a,b,c,d,e,f,g,h,i,j,k,l);
}
