// BADDECL.C -- TEST ERROR CHECKING ON DECLARATIONS
//
// 91/10/01     -- Ian McHardy          -- defined

// 7.1
static char;                    // vars need names
extern int;			// ditto


double;

void junk( void ){}

typedef char Pc;                // ok
static Pc;                      // needs name

// 7.1.1

static class C1{ public: int a; };// class declaration cannot be static
extern struct S10{ const volatile unsigned short int cvusi; };// or extern
auto class C10{ public: int b; };		// or auto
register class C11{ public: long double ld; };	// or register
auto long unsigned int lus;	// cannot be auto at global scope
register float f;		// ditto     register

typedef static int sint;	// typedef cannot be static or extern
typedef extern int eint;	// ditto

char *f1();			// OK
static char *f1();		// error: changing linkage to internal (p98)

static int i1;			// OK
int i1;				// error: redeclaration

static int i2;                  // OK
extern int i2;			// OK, still internal linkage

int i3;				// OK
static int i3;			// error: changing linkage 

extern i4;			// OK
static int i4;			// error: changing linkage

struct S1;                      // OK (pg 99)
extern S1 s1;
extern S1 f2();
extern void g( S1 );

void h()
{
    g( s1 );                    // error: S1 undefined
    f2();                       // error: S1 undefined
}

register auto int i5;		// cannot be register and auto

static extern auto i6;		// cannot have more than one stortage class
extern register int i7;		// specifier

// 7.1.2

inline int i8;			// only functions can be inline
virtual int i9;			// ditto

virtual int f3()		// not a member function
{
    return( 0 );
}

inline class c2{ public: int i10; };// class can not be inline

// 7.1.3

typedef int f4(){ return 12; } a_funtion;// cannot typedef a function decl
typedef long i11;		// OK
long i11;			// i11 redefined

void f5()
{
    int i11;                    // OK hides typename i11
}

struct S1{ int i12; double d1; };
class S1{ double d2; int i13; };// redeclaration
typedef int S1;			// redeclaration

class complex{ public: double real; double imaginary; };
typedef int complex;		// redeclaration

typedef int I;			// OK
short I shi;			// cannot combine type specifiers
unsigned I ui;			// ditto



// 7.1.6

void f6( void )
{
    const int i14;

    i14 = 12;			// error: i14 is const
    i14 += 12;			// ditto
}

// 7.2
enum colour{ orange };		// OK
enum fruit{ orange };		// redeclaration

enum something{ i15 };		// OK	

int i15;			// redeclatation

enum numbers{
    one = 1,			// OK
    two,
    three,
    sixty_five = 65.01,		// must be integral
    numbero_uno = 1		// OK
};

void f7( void )
{
    colour col;

    col = 15;			// not enumerated value
    col = 0;			// not enumerated value
    col++;			// not enumerated value
    col %= 2;			// not enumerated value
    col = orange;		// OK
}
