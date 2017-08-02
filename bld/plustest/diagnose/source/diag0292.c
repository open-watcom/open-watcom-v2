// compile -zpw -zk0
// Å\
namespace x0 {
    char *p = "Å\";
    int a = 'Å\';
};
namespace x1 {
struct OK1 {
    int m;
    void operator =( int );
};
struct OK2 : OK1 {
    int m;
};
struct OK3 {
    int m;
    OK2 f;
};
struct OK4 {
    int m;
    static OK3 k;
};
struct BAD1 {
    int m;
    void operator =( BAD1 & );
};
struct BAD2 : BAD1 {
    int m;
};
struct BAD3 {
    int m;
    BAD2 f;
};
struct OK5 {
    int m;
    static BAD3 k;
};

struct Q {
    union {
	OK1 m1;
	OK2 m2;
	OK3 m3;
	OK4 m4;
	OK5 m5;
    };
};

struct R {
    union {
	BAD1 m1;
	BAD2 m2;
	BAD3 m3;
    };
};
};
namespace x52 {
#pragma pack(push,8)
struct S {
    char c;
    double d;
};
#pragma pack(pop)
};
namespace x60 {
struct s *foo( int );
struct s {
}
ack( int x ) {
}
}
namespace x67 {
extern void gnarf( void );

struct Q {
    Q();
    ~Q();
    int foo() const;
};

void bar( void ) {
    Q q;

    if( q.foo ) {
	gnarf();
    }
}
};
namespace x84 {
struct S { };
typedef void (S::* mp) ( void );

void ack( S *p ) {
    p->*mp();
}
};
namespace x92 {
// internal compiler error
class A;
class B;
class F1
{
    F1( A * );
    F1( B * );
};

class D
{
    D();
    F1 f;
};

D::D()
    : f( 0 )	// <--- internal compiler error
{
}
};
namespace x113 {
struct S {
    mutable const int &i;
    S( int &x ) : i(x) {
    }
    void foo() const {
	i = 2;
    }
};
#include <setjmp.h>
namespace x127 {

template <class T> void delete_object( T*& object )
{
  if( object !=  0L  )
  {
    delete object;
    object =  0L ;
  }
}

class CValue
{
public:
        int i;
};

void foo()
{
        CValue *v;
        jmp_buf b;

  (void) setjmp(b);
        delete_object(v); // Move this above the setjmp and it works!!!
}
};

void foo( S *p ) {
    p->foo();
}
struct A {
            int &i;
	            A();
};

    void f(const A *p) {
	        p->i = 0;  // OK
    }
}
#define A10	3
#define A11	1e+A10
namespace {
    double d = A11;
};
namespace x172 {
// macro __u will automatically generate a unique name
#define __up(a,b)       a##b
#define __ue(a,b)       __up(a,b)
#define __u             __ue(__u,__LINE__)

void a();
int __u( int i ) {
    return i == a() || a() == i;
}
int __u( int i ) {
    return i != a() || a() != i;
}
int __u( int i ) {
    return i <= a() || a() >= i;
}
int __u( int i ) {
    return i << a() || a() >> i;
}
int __u( int i ) {
    return i * a() || a() * i;
}
int __u( int i ) {
    return a() || a();
}
}
namespace x198 {
namespace x1 {
    struct B {
	virtual void foo( int, int );
    };
    struct D : B {
	void __cdecl foo( int, int );
	D();
    };
    D::D() {
	B *p = this;
	D *q = this;
	q->foo( 1, 2 );
	p->foo( 3,4 );
    }
    D x;
}
namespace x7 {
    struct B {
	virtual void __cdecl foo( int, int );
    };
    struct D : B {
	void foo( int, int );
	D();
    };
    D::D() {
	B *p = this;
	D *q = this;
	q->foo( 1, 2 );
	p->foo( 3,4 );
    }
    D x;
}
}
namesapce x232 {
int h(1);

int f(1,2,3);

int g( int(1,2,3) );

foo() {
    int q(1),
    f(1,2,3);
}
};
namespace x244 {
struct A {
    void f1( int, bool = false );
    void f2( int );
    void f2( int, bool = false );
    void f3( int, bool = false );
    void f4( int, char );
    void A::f4( int, char = 'a' ) {
    }
    A( A const &, bool );
};

void A::f1( int ) {	// error
}

void A::f2( int ) {	// ok
}

void A::f0( int ) {	// error
}

A::A( A const &, bool = false ) {	// error
}

void A::f3( int, bool = false ) {	// error
}
}
namespace x271 {
    // we don't generate an error
void f( int a, char *b ) { a=a; b=b; }
void f( int a ) { a=a; }

void *pf = f;
}
namespace x278 {
extern void foo( __int64 x );
void bar( unsigned x )
{
    foo( __int64( -(x) ) );
    foo( __int64( -(2147483648) ) );
    foo( __int64( +(x) ) );
    foo( __int64( +(2147483648) ) );
}
}
namespace x288 {
#if 0
char *e = "(???)???-????";
char *f = "(" "???" ")"" ???" "-" "????";
#else
char *e = "(???)???-????";
char *f = "(" "???" ")" "???" "-" "????";
char *g = "(\?\?\?)\?\?\?-\?\?\?\?";
#endif
};
// should not get warnings
class x299 {
    friend void *operator new( unsigned );
    friend void operator delete( void * );
    friend void *operator new[]( unsigned );
    friend void operator delete[]( void * );
};
namespace x305 {
    // should not get warnings
    class N {
	friend void *operator new( unsigned );
	friend void operator delete( void * );
	friend void *operator new[]( unsigned );
	friend void operator delete[]( void * );
    };
};
namespace x314 {
    // should get warnings
    void *operator new( unsigned );
    void operator delete( void * );
    void *operator new[]( unsigned );
    void operator delete[]( void * );
    class N {
	friend void *operator new( unsigned );
	friend void operator delete( void * );
	friend void *operator new[]( unsigned );
	friend void operator delete[]( void * );
    };
};
#if 0
Version 35I63
#endif
namespace x330 {
struct foo
{
    static void qw() const;
    static void bar();
};

void foo::bar() const
{
}
}
namespace x341 {
struct B {
};

template <class T: public B>
    class S {
	T *p;
    };
};
/* ??/??/?? */
// ??/??/??
#error last line
// Å
