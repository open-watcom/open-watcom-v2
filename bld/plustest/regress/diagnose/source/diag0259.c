struct x349F1 {
    void (*operator()( int ))( int );
    void (*operator()( int, int ))( int, int );
};
struct x349F2 {
    void (*operator()( int, int ))( int );
    void (*operator()( int ))( int, int );
};
struct x349A {
};
x349F1 operator ,( x349A, int );
x349F2 operator ,( int, x349A );

void x349f() {
    (x349A(),0)(0)(0);
    (x349A(),0)(0)(0,0);
    (x349A(),0)(0,0)(0);
    (x349A(),0)(0,0)(0,0);
    (0,x349A())(0)(0);
    (0,x349A())(0)(0,0);
    (0,x349A())(0,0)(0);
    (0,x349A())(0,0)(0,0);
}
namespace S {
};
struct S {
};
namespace i {
};
int i;
namespace CT {
};
template <class T> struct CT {
};
int a;
namespace a {
};
typedef int X;
namespace X {
};
template <class T> struct CT2 {};
namespace CT2 { };
namespace x20 {
    namespace x {
	int x;
    }
    namespace e {
	namespace z {
	};
	int y;
    }
    namespace x {
	int z;
    }
    namespace x {
	int w;
    }
    namespace x {
	int a;
	namespace {
	    int qq;
	    int qq;
	    namespace ZZ {
		namespace {
		    namespace AA {
			namespace {
			    namespace XX {
				int dd;
				int dd;
			    };
			};
		    };
		};
	    };
	};
    }
    namespace x {
	int b;
    }
    namespace x {
	int c;
	int c;
	namespace y {
	    int i;
	}
    }
    namespace __x = x;
    namespace __x__y = x::y;
    namespace __x = x;
    namespace __x = x;
    namespace __x = __x;
    namespace __x = x;
    namespace __x = __x;
    namespace __x = __x;
    namespace __x = e::z;
    struct S {
	void x() {
	    int x;
	    ;;
	    __x::y::i++;
	    x::y::i++;
	    __x__y::i++;
	}
    };
    
    void foo( S *p ) {
	p->x();
    }
}

typedef int q;
namespace c = q;
namespace x90 {
    int qq;
    namespace {
	int x;
	extern int y = 3;
	static int z;
	static int foo( int, int );
	int ack() {
	    return 0;
	}
    }
};
namespace x102 {
union U {
    struct {         
        int i;
    };               
};                
struct S {
    U u;
};
S s2 = { 256 };		// this one complains about integer truncation
S s3 = { { 256 } };		// this one complains about integer truncation
S s1 = { 255 };		// this one does not complain
};
namespace x115 {
    extern "C" {
	int waste;
	template <class T>
	    struct S {
		T *p;
		S( T * );
	    };
	template <class T>
	    S<T>::S( T *p ) : p(p) {
		++waste;
	    }
    ;;
	S<int> x(0);
    };
};
namespace x131 {
    union U {
	int x, y;
    } x = { 1, 2 };
};
void x136() {
    char* p, q;
}
void x139() {
    char* p, q;
    char far *a,*b,*c;
}
namespace x143 {
    static union {
	class C {
	    int c;
	} m;
    };
    struct S {
    };
    static union {
	int S;
    };
    struct X {
	enum E { A, B };
	int a[X::B];
    };
    const struct Q {};
    struct X18 {
	char b1 : 16;
	bool b2 : 32;
	short b3 : 24;
    };
};
extern "C" {
    int WinMain( int );
};
int WinMain( int, int ) {
    return 0;
}
namespace x171 {
    void *alloc( unsigned );
    void *operator new( unsigned x ) {
	return alloc(x);
    }
    void operator delete( void * ) {
    }
    struct S {
	static void *alloc( unsigned );
	void *operator new( unsigned x ) {
	    return alloc(x);
	}
	void operator delete( void * ) {
	}
    };
};
void *alloc( unsigned );
void *operator new( unsigned x ) {
    return alloc(x);
}
void operator delete( void * ) {
}

x171::S *x194() {
    return new x171::S;
}

struct x198 {
    namespace N {
	int x;
    };
};
void x203(){
    namespace N {
	int x;
    };
};

template <class T>
    namespace x210 {
	int x;
    };
#if __WATCOM_REVISION__ >= 7
//--------------------------------------------------------
extern void Assert( int, const char *, const char *, const char *,
const char *, int );
#define __WTEXT( x ) x

#define WAssertEx(__x,__msg)\
	(((int)(__x))? (void)0						\
		     : Assert( 0, __WTEXT(#__x), __WTEXT(__msg),\
		                       __WTEXT(__FUNCTION__), \
		                       __WTEXT(__FILE__), __LINE__ ))

#define WASSERTEX(__x,__msg)  WAssertEx(__x,__msg)

void x227() {
    WASSERTEX( 1 );
}
#error
#endif
#undef X232
#define X232
#if +X232
#error OK
#else
#error bad!
#endif
#if 1+X232
#error OK
#else
#error bad!
#endif
#if (1+X232
#error bad!
#else
#error bad!
#endif

namespace x249 {
    namespace N {
	void f();
	void g();
	void h();
	namespace O {
            void N::f() {
              // error
	    }
	}
    }
    struct S {
	void N::h() {
	    // error
	}
    };
    void N::g() {
      // OK
    }
}
char * x270( int n ) {
  return new char(n + 1); // bug is here, but syntax is legal.
}
x314()
{
        float f = 1.0;
        int *i = (int *)0;
        if(i == &f)	// notes! should have different types
                return -1;
        return 0;
}
void main()
{
    int x;
    if( x = 0 );
    if( x = x );
}
namespace x288 {
class A
    {
     public:
        int i;
        int f() { return 2; }
     };
int as()
    {
    A **i = new A *[3];
    A *s;
    A * j = i[s->f];  // Note missing paren
    return 0;
    }
}
namespace x303 {
#define XX( v, a ) v = a;

char *p;
char *q;

void foo() {
    XX( p, "asdf", 1,2,3,3 );
    XX( q, "qw", 1, 2 );
}
}
namespace x313 {
    using namespace x313;
};

// add in here*********

#line 123 "zip.y"
syntax error here (should be line 123 "zip.y")
#goof should be line 124 "zip.y"
#error should be line 125 zip.y


#error last line
