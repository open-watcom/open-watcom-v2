#if 1
int x29;
#endif blah
struct X1 {
    int near *p;
    int far *q;
    int near *foo( int x );
    int near *bar( int x );
};

int near *X1::foo( int x )
{
    return x ? 0 : p;
}

int near *X1::bar( int x )
{
    return x ? 0 : q;
}

char c18 = ''';
;;;;;;;
char c20 = '\';
;;;;;;;
char c22 = '
';
;;;;;;;
char c25 = '\
';
;;;;;;;
int x31( int far x,
	int far y )
{
    int far z = x + y;
    return z;
}
struct X37 {
};
struct __export X39 {
};
struct __declspec(dllimport) X41 {
};
struct X43 : X37, X39 { };
struct X44 : X37, X41 { };
struct X45 : X39, X41 { };
struct __export X43a : X37, X39 { };
struct __export X44a : X37, X41 { };
struct __export X45a : X39, X41 { };
struct x49 {
    struct __cdecl B1 {
	virtual void foo( int, int );
    };
    struct __pascal B2 {
	virtual void foo( int, int );
    };
    struct B3 {
	virtual void foo( int, int );
    };
    struct D1 : B1, B2 {
	virtual void foo( int, int );
    };
    struct D2 : B1, B3 {
	virtual void foo( int, int );
    };
    struct __cdecl D3 : B3 {
	virtual void foo( int, int );
    };
    struct __cdecl D4 : B2 {
	virtual void foo( int, int );
    };
    struct __cdecl D5 : B1 {
	virtual void foo( int, int );
    };
    struct D6 : B2 {
	virtual void foo( int, int );
    };
};
struct x78 {
    struct B {
	B();
    };
    struct D : B {
	D(){};
    };
};
struct X86 {
    struct B {
	B();
    };
    struct D : B {
	virtual B();
	D(){}
    };
    B q;
} x95;

void x97( X86::D *p )
{
    p->B();
}
int x101 = 0x;
#define x102 0x
#if 0
#define x102 0x
#endif
#include <limits.h>
int x107 = LONG_MAX * 4;

#define x109 0
#define x110 /* nothing */

#if x109 && x110
int x113 = 3;
#endif

#if 1.
int x117 = 3;
#endif
#define x119 1 ##
#define x120 ## 1

struct x122 {
    int operator() ( int, int = 1 );
    int operator[] ( int, int = 1 );
};

int x126( x122 &r )
{
    return r(1);
}

void x132() {
    int a[10];
    enum { b };
    b += 1;
    ++b;
    b++;
    b = 2;
    a += 1;
    ++a;
    a++;
    a = 0;
    *a += 1;
    ++*a;
    (*a)++;
    *a = 0;
}
