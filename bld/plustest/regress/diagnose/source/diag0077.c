#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

struct x379 {
    struct S;
    
    typedef int (S::* mpu)( int );
    
    struct __stdcall S {
	int m( int );
	typedef int (S::* mpo)( int );
    };
    
    typedef int (S::* mpd)( int );
    
    void foo() {
	mpu u;
	mpd d;
	S::mpo o;
    
	u = &S::m;	// error
	d = &S::m;	// OK
	o = &S::m;	// OK
    }
};

extern void __brk0();
extern void __brk0(int);
#pragma aux __brk0 = 0xcc;      /* no overloading */

extern void __brk1();
#pragma aux __brk1 = 0xcc;
#pragma linkage(__brk1,system); /* pragma already set */

extern void __pragma("not") __brk2();

enum E { e1, e2 };

struct TEST_E {
    E x;
    TEST_E();
};

TEST_E::TEST_E() : x(1)         /* assigning 1 to an enum */
{
}

struct B {
    B();
    ~B();
    int f;
};
struct S : B {
    B::f;
    int f;
    int a : 1;
    int a;
};
typedef int S;

S::S( )
{
}

typedef int E;
int e1 = 2;

void foo( void )
{
    z = 1;
    int z;
}

#pragma aux pfn = 0xcc;
int pfn( ... );

void sam( int );
void sam( int & );

typedef unsigned size_t;
void * operator new( size_t & );

void d( int = 1, int = 2 );
void d( int , double = 0 );

struct W {
    void fn( int & x );
    int a;
};

void W::fn( iny & x ) {
    a = x;
}


#endif
