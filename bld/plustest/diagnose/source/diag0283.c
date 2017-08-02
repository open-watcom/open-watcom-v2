#include <stdio.h>
namespace x1 {
    x2( void *x )
    {
	((void*(void))x)();
    }
    int *p;
    int x;
    
    void foo() {
    void const *a[] = {
	&typeid( p ),
	&typeid( x ),
	&typeid( int **** ),
    };
    }
    void const *a[] = {
	&typeid( p[1] ),
	&typeid( &x ),
	&typeid( char **** ),
    };
    
    struct S {
	virtual void foo();
    };
    void * foo( S *p ) {
	typeid( *p );
	return dynamic_cast<void*>(p);
    }
};
namespace x26 {
    class A { virtual void f(); };
    class B { virtual void g(); };
    class D : public virtual A, private B {};
    void g() {
	D   d;
	B*  bp = (B*)&d;  // cast needed to break protection
	A*  ap = &d;      // public derivation, no cast needed
	D&  dr = dynamic_cast<D&>(*bp);  // succeeds
	ap = dynamic_cast<A*>(bp);       // succeeds
	bp = dynamic_cast<B*>(ap);       // fails
	ap = dynamic_cast<A*>(&dr);      // succeeds
	bp = dynamic_cast<B*>(&dr);      // fails
    }
    class E : public D , public B {};
    class F : public E, public D {};
    void h() {
	F   f;
	A*  ap  = &f;                    // succeeds: finds unique A
	D*  dp  = dynamic_cast<D*>(ap);  // fails: yields 0
				   // f has two D sub-objects
	E*  ep  = (E*)ap;                // ill-formed:
				   // cast from virtual base
	E*  ep1 = dynamic_cast<E*>(ap);  // succeeds
    }
};
namespace x52 {
    struct S;
    void foo( S *p ) {
	typeid( *p );
	typeid( S );
    }
};
namespace x63 {
    // we don't seem to be setting ref'd on sym
    static short foo( short ) {	// unref'd
	return 0;
    }
    static int foo( int ) {
	return 0;
    }
    static char foo( char ) {	// unref'd
	return 0;
    }
    int (*fp1)( int ) = foo;
    
    static int bar( int ) {
	return 0;
    }
    int (*fp2)( int ) = bar;
    // change to a warning
    typedef struct X {
    };
    void x83() {
	char c;
	c += 256;
	c -= 256;
	c /= 256;
	c %= 256;
	c >>= 2;
	c &= 256;
	c ^= 256;
	c |= 256;
	c = 256;
    }
    __int64 x95[] = {
	9999999999l,
	9999999999u,
	9999999999ui64,
	9999999999lu,
	9999999999ul,
	9999999999,
	0x9999999999999999l,
	0x9999999999999999,
	077777777777777777,
	0xffffffffffffffff,
	0xfffffffffffffffff,
	01777777777777777777777,
	017777777777777777777777,
	18446744073709551615,
	18446744073709551616,
    };
    __int64 x112[] = {
	18446744073709551616,
	16,32,64,127,255,511,1023,2049,8193,16383,
	0x1ffffffffffffffff,
	03777777777777777777777,
    };
    namespace x118 {
	char c;
	short s;
	int i;
	long l;
	void foo() {
	    c = 0xffffffffffffffff;
	    s = 0xffffffffffffffff;
	    i = 0xffffffffffffffff;
	    l = 0xffffffffffffffff;
	}
    };
    namespace x130 {
	struct a {
	    a & operator =( a const & );
	};
	
	struct fee : a {
	     const char array[10];
	     int i;
	     fee();
	};
	
	void foo( fee *p, fee *q ) {
	    *p = *q; // error: cannot gen op=
	}
	
	struct nfee : a {
	     const char array[10];
	     int i;
	     nfee();
	};
	foo(){
	    nfee f;
	    nfee g;
	    g=f;
	}
    };
    namespace x156 {
	#define TLS __declspec( thread )

	void func1()
	{
	    TLS int tls_data;		// Wrong!
	}
	
	int func2( TLS int tls_data )	// Wrong!
	{
	    return tls_data;
	}

	static class W{};
	
	TLS class A	// Wrong! Classes are not objects
	{
	    public:
		int a;
		int b;
		int c;
		int foo(int x) { return( x + 1 ); }
	};
	#undef TLS
	#define x181(x) x
	void x181()
	{
	}
	namespace x185 {
	    // add to diagnose test stream
	    class Wrap {
		short data;
	    public:
		Wrap() : data(-1) {}
		operator short& () { return data; }
		operator short const& () { return data; }
	    };
	    
	    
	    int main() {
		Wrap warray[10];
		*warray = 5;
	    
		Wrap *wcur = warray+9;
		while (*wcur==-1) --wcur;	// Compiler reports E000 here
		return 0;
	    }
	};
    };
};
namespace x207 {
typedef void (*FN)( void );

void ack() {
}

FN const a[] = {
    ack,
};
FN const *b[] = {
    ack,	// this doesn't work
};
struct S {
    int S::a : 2;
    int S::a : 2;
    int operator + : 1;
    int operator + : 1;
};
// ICE 57
struct QQ {
    short : 1;
    short a : 1;
    short : 1;
};
short &foo( QQ *p ) {
    return p->a;
}
};
namespace x235 {
struct S {
    volatile char a : 1;
    char b : 3;
};
char foo();

void foo( S *p ) {
    p->a = p->b;
    p->a = (char) p->b;
    p->a = (int) foo();
    p->a = foo();
    p->a = (char) foo();
    p->a = (short) foo();
    p->a = 3;
    p->a = p->a ? 3 : 1;
    p->a = p->a ? 1 : 0;
}
};
#error last line
