unsigned int long volatile *foo() {
    return new const long unsigned int;
}

struct B5 {
    int x;
    B5( int x = 0, int = 0 ) : x(x) {
    }
    operator int() { return x; }
};

struct F12 {
    operator B5() { return 0; }
private:
    operator int() { return 1; }
};

void f() {
    F12 x;
    if( B5(x) ) f();
    if( !B5(x,1) ) f();
}

struct __declspec(dllexport) __cdecl X24 {
    int foo( int );
    int bar( int (__export __cdecl X24::*fn)( int ) );
    void goo( void );
};
void X24::goo( void ) {
    bar( &X24::foo );
    bar( (int __cdecl (X24::*)( int ))&X24::foo );
}

struct X34 {
    typedef int T;
    union { int T; };
};

template <class T>
    void x40( T ) {
    }

void x46( void *p ) {
    *p;
    1 + *p;
    x40( *p );
    sizeof( x46( 0 ) );
}

struct X50 {
    struct C {
	C( int );
	C( C const & );
	int foo();
    };
    struct CD {
	CD( int );
	CD( CD const & );
	~CD();
	int foo();
    };
    
    void ack( C * );
    void ack( CD * );
    
    void bar() {
	ack( &(C)0 );
	ack( &(CD)0 );
	((C)0).foo();
	((CD)0).foo();
    }
};

class X74 {
    friend int ::foo( X74 * );
    int x;
};

class Y79;
int foo( Y79 * );
class Y79 {
    friend int ::foo( Y79 * );
    int y;
};

int foo( X74 *p )
{
    return p->x;
}

int foo( Y79 *p )
{
    return p->y;
}

struct X95 {
    struct B {
	::q;
	D::a;
    };
    struct BB {
	int m;
	int n;
    };
    struct CC : private BB {
	BB::m;
    };
    void foo( CC *p )
    {
	p->m = 1;
	p->n = 1;
    }
};

#define x115(a,b) a+b
#if x115(1,
	2)
int x118()
{
    return 0;
}
#endif

int x124( int x, int y )
{
    return
    x115
    (
    x
    ,
    y
    )
    ;
}

#define X136 123
#define X136 0xa0a

int x139 = X136;
