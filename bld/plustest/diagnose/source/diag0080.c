#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

#include <stddef.h>
#define UNIQUE( line )  x##line
#define PRE_UNIQUE( parm )      UNIQUE( parm )
#define U               PRE_UNIQUE(__LINE__)
int a;
char *p;
struct S { } x;
struct S1 {} *p1;
struct S2 {} *p2;
struct O {
    int bf : 1;
    static int sm;
    void fn();
    enum E { en = 1 };
};
struct UO;
void *pv;
void (*pf)( void );
void fn()
{
    p += p;                     // ERR_BOTH_PTRS
    ++x;                        // ERR_NOT_PTR_ARITH
    x + 1;                      // ERR_LEFT_NOT_PTR_ARITH
    1 + x;                      // ERR_RIGHT_NOT_PTR_ARITH
    a = 10 - p;                 // ERR_SUB_PTR_FROM_ARITH
    a = p;                      // ERR_PTR_TO_ARITH_ASSIGNMENT
    x = x * 1;                  // ERR_LEFT_EXPR_MUST_BE_ARITHMETIC
    x = 1 * x;                  // ERR_RIGHT_EXPR_MUST_BE_ARITHMETIC
    x = x ^ 1;                  // ERR_LEFT_EXPR_MUST_BE_INTEGRAL
    x = 1 ^ x;                  // ERR_RIGHT_EXPR_MUST_BE_INTEGRAL
    *x;                         // ERR_EXPR_MUST_BE_POINTER_TO
    a = offsetof( int, a );     // ERR_OFFSETOF_CLASS
    p1 = p2;                    // ERR_PTR_CONVERSION
    1 = a;                      // ERR_LEFT_MUST_BE_LVALUE
    ( a - 1 ) = a;              // ERR_LEFT_MUST_BE_LVALUE
    a = offsetof( O, bf );      // ERR_OFFSETOF_BITFIELD
    a = offsetof( O, sm );      // ERR_OFFSETOF_STATIC
    a = offsetof( O, fn );      // ERR_OFFSETOF_FUNCTION
    a = offsetof( O, en );      // ERR_OFFSETOF_ENUM
    a = offsetof( UO, xx );     // ERR_OFFSETOF_UNDEFINED
    a = ( a ? p : a );          // ERR_NOT_PTR_OR_ZERO
    a = sizeof( *pf );          // ERR_CANT_TAKE_SIZEOF_FUNC
    a = sizeof( *pv );          // ERR_CANT_TAKE_SIZEOF_VOID
    pf[1];                      // ERR_PTR_FUNC_OR_VOID_LEFT
    1[pv];                      // ERR_PTR_FUNC_OR_VOID_RIGHT
    ++pv;                       // ERR_PTR_FUNC_OR_VOID
}
struct RFG { int data_member; void func_member (); };

int RFG::*pdm1;
int RFG::*pdm2;

int (RFG::*pfm1) ();
int (RFG::*pfm2) ();

void U()
{
  if (pdm1 == pdm2)
        ;
  if (pdm1 > pdm2)              // ERR_LEFT_NOT_PTR_ARITH
        ;
  if (pfm1 == pfm2)
        ;
  if (pfm1 > pfm2)              // ERR_LEFT_NOT_PTR_ARITH
        ;
}

void U()
{
    switch( a )
    case 0: int z = 3;          // ERR_DCL_SWITCH
    if( a )
        int z = 2;              // ERR_DCL_IF
    else
        int z = 1;              // ERR_DCL_ELSE
    for( int i = 1; i < 10; ++i )
        for( int i = 1; i < 10; ++i )
            int i = 3;          // ERR_DCL_FOR
    while( a )
        int z = 2;              // ERR_DCL_WHILE
    do
        int z = 2;              // ERR_DCL_DO
    while( z != 0 );
    switch( (char) a ) {
    case 0xff01:
    case 0xfe01:                // ERR_DUPLICATE_CONV_CASE_VALUE
        ++a;
    }
}

void U()
{
    pv = this;                  // ERR_NO_THIS_PTR_DEFINED
}

int & U()
{ int a; return a; }            // ERR_RET_AUTO_REF

struct VV {
};
struct CC : virtual VV {
    void *operator new( size_t );
    void *operator new( size_t, char );
    void *operator new( size_t, signed char );
    void *operator new( size_t, char, char );
    CC( int );
    CC();
};

CC *U( VV *p )
{
    return (CC*) p;             // ERR_CONVERT_FROM_VIRTUAL_BASE
}

void U( void, void, void )      // ERR_NO_VOID_PARMS
{
}

void U()
{
    CC *p = new ( *p) CC;               // ERR_NEW_OVERLOAD_FAILURE
    CC *q = new CC[3](2);               // ERR_CANT_INIT_NEW_ARRAY
    CC *r = new (1.0) CC;               // ERR_NEW_OVERLOAD_AMBIGUOUS
    r = q;
    void(1);
}

void U()
{
    int a[10];

    a++;                                // ERR_ARRAY_LEFT
    ++a;                                // ERR_ARRAY_LEFT
    a = pv;                             // ERR_ARRAY_LEFT
    a = 0;                              // ERR_ARRAY_LEFT
    enum a *p;                          // ERR_UNDECLARED_ENUM_SYM
    a::t *q;                            // ERR_UNDECLARED_CLASS_SYM
    p = q;
}
typedef void U( int = 1, int = 2 );     // ERR_DEFAULT_ARGS_IN_A_TYPE
void U()
{
    void *pv;
    void volatile *pvv;
    void const *pcv;

    pv = pvv;
    pv = pcv;
}
// add default args
void xx_OK( int, int, int );
void xx_OK( int, int, int = 1 );
void xx_OK( int, int = 2, int );
// add default args
void xx_BAD( int, int, int );
// oops! void xx_BAD( int, int, int = 1 );
void xx_BAD( int, int = 2, int );
struct S158 {
    S158(int);
    operator int();
    int a;
};

S158 U( int b, int i, S158 s )
{
    return b ? i : s;
}

void U( S158 *p, CC *q )
{
    a = *q;
}
struct S173;

int S173::U = 1;
struct S176 {
    typedef int T;
};
void U( S176 *p )
{
    p->T = 1;
}
typedef struct V183 {
    int v;
} *PV185;
typedef struct D186 : virtual V183 {
    int d;
} *PD188;
D186 U( PV185 *q )
{
    PD188 *p = PD188(q);
    return *q;
}
struct S193 {
    S193(int);
    S193(char);
};
struct T197 {
};

void U()
{
    T197 y = (T197) 0;
    S193 x = (S193) 1.0;
    y = (T197) 0;
    x = (S193) 1.0;
}
T197 U()
{
    return 0;
}
S193 U()
{
    return 1.0;
}
extern void f216( D186 & );
void U( V183 &r )
{
    f216( r );
}
const void *p221 = &a;
void *p222 = p221;
struct S223 {
    operator int();
};
double U( S223 *p )
{
    return p->operator double();
}
int *U( const int *p )
{
    return p;
}
const int &U( const int q )
{
    int z = q;
    return z;
}
void __export __export U();
struct B240 {
    B240(B240&);
    B240();
};
struct D244 : private B240 {
};
D244 v246(0,B240());
D244 *p247;
const D244 *p248;
volatile D244 *p249;
volatile const D244 *p250;
void *p251 = p247;
void *p252 = p248;
void *p253 = p249;
void *p254 = p250;
void __far f255();
int __far v256;
void *p257 = f255;
void *p258 = &v256;
struct S259 {
    void operator =(char);
    void operator =(int);
};
S259 v263;
void U()
{
    v263 = 1.0;
}
struct U {
    int m;
    static void fn()
    {
        m = 1;  // Error!
    }
};
struct S275 {
    S275(S275&);
};
struct S278 {
    S278(S278&);
};
struct S281 : S275, S278 {
    S281(S275&);
    S281(S278&);
    S281(S281&);
};
struct S286 {
    S281 a[10];
};
S286 v289;
S286 v290(v289);
void f291();
void f291(int);
extern int f293( int, ... );
void U()
{
    if( f291 )
        a++;
    if( &f291 )
        a++;
    if( &f291 && &f291 )
        a++;
    if( f291 && f291 )
        a++;
    f293( 0, f291 );
    f293( 0, &f291 );
}
void U( const int &rc, volatile int &rv )
{
    int &r1 = rc;
    int &r2 = rv;
    int const &r3 = rv;
    int const &r4 = r1;
    int volatile &r5 = rc;
    int volatile &r6 = r1;
}
struct U {
    void mf();
    int md;
    struct U {
        void f()
        {
            mf();
            md = 1;
        }
    };
};
struct S327 {
    int a;
    int f();
};
struct S330 : S327 {
};
struct A332 : S330, S327 {
};
int A332::* U = &A332::a;
int (A332::* U)() = &A332::f;
struct PRI335 : private S327 {
};
int PRI335::* U = &PRI335::a;
int (PRI335::* U)() = &PRI335::f;
struct PRO338 : protected S327 {
};
int PRO338::* U = &PRO338::a;
int (PRO338::* U)() = &PRO338::f;
struct S341 {
};
int S341::* U = &S330::a;
int (S341::* U)() = &S330::f;
void U(
    int S327::* mp,
    A332* p1,
    PRI335* p2,
    PRO338* p3,
    S341* p4,
    A332& r1,
    PRI335& r2,
    PRO338& r3,
    S341& r4,
    int i )
{
    i->a = 1;
    (i->*mp) = 1;
    (p1->*mp) = 1;
    (p2->*mp) = 1;
    (p3->*mp) = 1;
    (p4->*mp) = 1;
    (((S327 *)0)->*mp) = 1;
    (r1.*mp) = 1;
    (r2.*mp) = 1;
    (r3.*mp) = 1;
    (r4.*mp) = 1;
    (i.*mp) = 1;
}
void U( int S327::* mp, int *p, int S341::* wp )
{
    if( mp == p ) {
    }
    if( p == mp ) {
    }
    if( mp > p ) {
    }
    if( p < mp ) {
    }
    if( wp == mp ) {
    }
    ( int S327::* ) a;
}
struct S384 {
    ~S384();
    int a;
};
void U( S384 *p, S384 &r )
{
    p->~S341();
    p->S341::~S384();
    (~q)();
    r.S384::~S384();
}
void U( int S327::* mp )
{
    char S327::* cp = mp;
}
extern void f403();
void U( int b, int t )
{
    b ? t : f403();
    b ? f403() : t;
    b ? S327() : b;
    S327() ? t : b;
}
void U( int *pi, const int *pci, volatile int *pvi )
{
    void *p1;
    void *p2;
    void *p3;
    p1 = pi;
    p2 = pci;
    p3 = pvi;
}
void f420( int & );
int & f421( volatile int *p )
{
    f420( *p );
    return *p;
}
struct V426 {
    virtual void f();
};
struct B429 : virtual V426 {
    virtual void f();
};
struct B432 : virtual V426 {
    virtual void f();
};
struct D435 : B429, B432 {
    D435()
    {
    }
};
struct S327 {
    int a;
    int f();
};
void U(
    S327 *p1 = (S341 *) a,
    S327 *p2 = (A332 *) a,
    S327 *p3 = (PRI335 *) a,
    S327 *p3 = (PRO338 *) a );
extern "Qwerty" int U;
#blah
void U()
{
    return;
    if( a )
        return;
}
void U()
{
    void *p = (void *)( new class C {} );
}
#define M461 ##x

#pragma aux prag463 = \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd 0xcd \
parm caller [];

void f486( int * );
void f486( int (*)[3] );

void U()
{
    int a[3];

    f486( a );
    f486( &a );
}

void U(char n, char s, char e, char w){
    cout << "corner(" << n << ", " << s << ", " << e << ", " << w << ");"
    goto
}

void U()
{
    int *a +
}

struct U {
    int *next
    private
};

#endif
