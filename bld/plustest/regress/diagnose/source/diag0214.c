#define test( type ) \
type foo##type( type x, type y ) \
{ \
    type z; \
 \
    z = x + y; \
    return z; \
}

#define testa( dtype, stype ) \
void foo##stype##dtype( stype x, dtype y ) \
{ \
    y = x;\
}

typedef unsigned char unsigned_char;
typedef unsigned short unsigned_short;
typedef unsigned long unsigned_long;
typedef signed char signed_char;

test( char );
test( unsigned_char );
test( signed_char );
test( short );
test( unsigned_short );
test( unsigned );
test( int );
test( long );
test( unsigned_long );

testa( char ,  char );
testa( char ,  unsigned_char );
testa( char ,  signed_char );
testa( char ,  short );
testa( char ,  unsigned_short );
testa( char ,  unsigned );
testa( char ,  int );
testa( char ,  long );
testa( char ,  unsigned_long );

testa( unsigned_char ,  char );
testa( unsigned_char ,  unsigned_char );
testa( unsigned_char ,  signed_char );
testa( unsigned_char ,  short );
testa( unsigned_char ,  unsigned_short );
testa( unsigned_char ,  unsigned );
testa( unsigned_char ,  int );
testa( unsigned_char ,  long );
testa( unsigned_char ,  unsigned_long );

testa( signed_char ,  char );
testa( signed_char ,  unsigned_char );
testa( signed_char ,  signed_char );
testa( signed_char ,  short );
testa( signed_char ,  unsigned_short );
testa( signed_char ,  unsigned );
testa( signed_char ,  int );
testa( signed_char ,  long );
testa( signed_char ,  unsigned_long );

testa( short ,  char );
testa( short ,  unsigned_char );
testa( short ,  signed_char );
testa( short ,  short );
testa( short ,  unsigned_short );
// testa( short ,  unsigned );		// 16-32 difference
testa( short ,  int );
testa( short ,  long );
testa( short ,  unsigned_long );

testa( unsigned_short ,  char );
testa( unsigned_short ,  unsigned_char );
testa( unsigned_short ,  signed_char );
testa( unsigned_short ,  short );
testa( unsigned_short ,  unsigned_short );
// testa( unsigned_short ,  unsigned ); // 16-32
testa( unsigned_short ,  int );
testa( unsigned_short ,  long );
testa( unsigned_short ,  unsigned_long );

testa( unsigned ,  char );
testa( unsigned ,  unsigned_char );
testa( unsigned ,  signed_char );
testa( unsigned ,  short );
testa( unsigned ,  unsigned_short );
testa( unsigned ,  unsigned );
testa( unsigned ,  int );
//testa( unsigned ,  long );	// 16-32
//testa( unsigned ,  unsigned_long );	// 16-32

testa( int ,  char );
testa( int ,  unsigned_char );
testa( int ,  signed_char );
testa( int ,  short );
testa( int ,  unsigned_short );
testa( int ,  unsigned );
testa( int ,  int );
//testa( int ,  long );		// 16-32
//testa( int ,  unsigned_long );// 16-32

testa( long ,  char );
testa( long ,  unsigned_char );
testa( long ,  signed_char );
testa( long ,  short );
testa( long ,  unsigned_short );
testa( long ,  unsigned );
testa( long ,  int );
testa( long ,  long );
testa( long ,  unsigned_long );

testa( unsigned_long ,  char );
testa( unsigned_long ,  unsigned_char );
testa( unsigned_long ,  signed_char );
testa( unsigned_long ,  short );
testa( unsigned_long ,  unsigned_short );
testa( unsigned_long ,  unsigned );
testa( unsigned_long ,  int );
testa( unsigned_long ,  long );
testa( unsigned_long ,  unsigned_long );

int x121( int x, int y, int z )
{
    return x <= y < z;
}

int x126( int x, int y, int z )
{
    return x <= y == z;
}

struct X131 {
    struct X132 *p;
};
struct X132 {
    int m;
};

void x138( X131 *p )
{
    p->p->m = 1;
}

extern void x143( struct Q143 * );

struct A145 {
    virtual int foo( unsigned, unsigned ) = 0;
};

struct DA149 : A145 {
    int foo( unsigned );
};

struct C153 : DA149 {
    /*virtual*/ int foo( unsigned, unsigned );
};

C153 ok;

struct X159 {
    struct B {
    };
    struct D : virtual B {
    };
    
    struct X {
	virtual B *fn( int, ... );
    };
    struct Y : X {
	virtual D *fn( int, ... );
    };
};

struct X173 {
    struct VV {
	virtual int foo( int, ... );
    };
    struct DD : virtual VV {
	virtual int foo( int, ... );
    };
    
    DD x;
} x182;

void x184( int );

template <class T>
    struct X187 {
	X187( T x )
	{
	    x184( x );
	}
    };

template <class T>
    struct X195 {
	operator X187<T>();
    };

template <class T>
    X195<T>::operator X187<T>() {
	return 0;
    }
int __export x203;
int __export (*(x204(int,int)))( int, int, int )
{ return 0; }
int __declspec( dllexport ) (*(x206(int,int)))( int, int, int )
{ return 0; }
int __declspec( thread ) (*(x208(int,int)))( int, int, int )
{ return 0; }

struct A221 {
    virtual void foo() = 0;
};

A221 &x215();

void x220( A221 const &r = x215() );

void x219() {
    x220();
}

struct X223 {
    operator void();
};

struct U227;

struct B229 {
};
struct D229 : B229 {
    operator D229();
    operator D229 const &();
    operator D229 volatile &();
    operator D229 &();
    operator B229();
    operator B229 const &();
    operator B229 volatile &();
    operator B229 &();
    operator U227();
    operator X223();
};

typedef struct X244 {
} X244, const *CP244, far *FP244, const far *CFP244;

CP244 s1244;

CFP244 s2244;

int i244, const x244, const ***pc244;

void x253( int v, X244 vs )
{
    i244 = v;
    x244 = v;
    ***pc244 = v;
    *s1244 = vs;
    *s2244 = vs;
}

int x262( int m, int n )
{
top:
    --m;
here:
    if( m ) goto bottom;
    goto anywhere;
    goto top;
bottom:
    --n;
    if( n ) goto top;
    return n + m;
}

int x276( char c )
{
    switch( c ) {
    case 'a':
    case 'b':
    case 'a':
    case '\xff':
    case -2:
    case -1:
	++c;
	break;
    }
    return c;
}
/*///*/
/*////*/
/*/////*/
/*/**/
/*/**/
/*/**/
/*/**/
///*************/
struct X298 {
    void foo();
    void bar() const;
    int m;
};

void X298::bar() const {

    m = 1;

    this->m = 1;

    (*this).m = 1;

    this->foo();

    foo();

    (*this).foo();

    foo();

    this->foo();

    (*this).foo();

    return;
}
