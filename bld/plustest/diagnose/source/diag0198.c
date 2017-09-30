struct QQ1;

struct QQ1 {	// location of 'QQ1' should be here
};

typedef int QQ1;

struct __cdecl SS1 {
    int a;
};

union UU5 {
    SS1 x;
};

#define MOD1	far
#define MOD2	near
#define MOD3	__based(__segname("_DATA"))

struct SS5 {
    static int MOD1 x11;
    static int MOD1 x12;
    static int MOD1 x13;
    static int MOD2 x21;
    static int MOD2 x22;
    static int MOD2 x23;
    static int MOD3 x31;
    static int MOD3 x32;
    static int MOD3 x33;
};

			//  MSC		WATCOM
int MOD1 SS5::x11;	//   .		  .
int MOD2 SS5::x12;	// error	 error
int MOD3 SS5::x13 = 3;	//   !		  OK
int MOD1 SS5::x21;	// error	 error
int MOD2 SS5::x22;	//   .		  .
int MOD3 SS5::x23;	// error	 error
int MOD1 SS5::x31 = 5;	//   !		  OK
int MOD2 SS5::x32;	// error	 error
int MOD3 SS5::x33;	//   .		  .

struct X1;
extern X1 x2();

void x4()
{
   x2();    // error: X1 undefined
}

// infinite recursion in compiler copying return value from foo()(?)
struct XX2;

XX2 xx4();

int xx6 = xx4().a;
// p.98 ARM example (7.1.1)
static char *f();
char *f() {		// OK, still static
    return 0;
}

char *g();
static char *g() {	// error (WATCOM C++ forgives this)
    return 0;
}

static int a;
int a;			// error

static int b;
extern int b;		// OK, still static

int c;
static int c;		// error

extern d;
static int d;		// error

struct C {
    C();
    ~C();
};

struct D {
    C c;
    D(int a);
    ~D();
};

D::D(int a) : c(a) {
}


void x39( char x, char y, char z, signed q, unsigned r )
{
    x = ( y & z );
    x = ( y | z );
    x = ( y ^ z );
    x = ~y;
    x = +y;
    x = !y;
    x = ( y || z );
    x = ( y && z );
    x = ( y < z );
    x = ( y > z );
    x = ( y <= z );
    x = ( y >= z );
    x = ( y == z );
    x = ( y != z );
    x |= 0x1000;
    x = ( q < r );
    x = ( r >= 0 );
    x = ( r < 0 );
    x = ( r > 0 );
}

void x66( char _based(void) *p, char _based(void) *q, _segment sel )
{
    p = q + 1;
    *(sel:>p) = 0;
}

struct B68 {
    int b;
    B68();
    B68(B68&);
};
struct D73 : B68 {
    B68 d;
};

B68::B68() : b(0) {
}
B68::B68(B68&s) : b(s.b) {
}

D73 x15( int x )
{
    D73 v;

    v.b = x;
    v.d.b = x;
    return v;		// no warning
}


void x25()
{
    D73 i = x15(1);	// warning
    D73 a;
    a = x15(2);
}

template<class Type>
    class DList {
	friend class DLIterator<Type, DList<Type> >;
    };

template<class Type, class LType>
    class DLIterator {};

template<class Type>
    class DLIter: public DLIterator< Type, DList<Type> > {};

DLIter<int> x116;
