#pragma message( "hello" );
extern int x1( void );

int x3( void ) {
    for(;;) {
	if( x1() ) return 0;
    }
    if( 1 && x1() ) x1();
}

int x10( void ) {
    {
	for(;;) {
	    if( x1() ) return 0;
	}
	if( 1 && x1() ) x1();
    }
}

int x19( void ) {
    goto done;
    for(;;) {
	if( x1() ) return 0;
    }
    if( 1 && x1() ) x1();
    done:;
}

int x28( void ) {
    {
	{
	    for(;;) {
		if( x1() ) return 0;
	    }
	    if( 1 && x1() ) x1();
	}
    }
}

int x39( void ) {
    {
	if( x1() ) {
	    for(;;) {
		if( x1() ) return 0;
	    }
	    if( 1 && x1() ) x1();
	}
    }
}

int x50( void ) {
    if( x1() ) {
	{
	    for(;;) {
		if( x1() ) return 0;
	    }
	    if( 1 && x1() ) x1();
	}
    }
}

struct X61 {
    int a,b,c;
};

void x65( X61 *p, int X61::* mp )
{
    p->*mp = 1;
}

void x70( short &x )
{
    (int&)x = 1;
}
#pragma message( "hello" );
#pragma message( / );
#pragma error "there."
#pragma error /
int x76( int a, int b )
{
    int c, d;
    return 0;
}
extern __declspec(dllexport) int x83;
extern __declspec(dllimport) int x83;
extern int __export x85;
extern __declspec(dllimport) int x85;
struct X87 {
    static int const a = 0;
    static int const b = 1;
    void foo() = 0;
    void bar() = 1;
    virtual void ack() = 0;
    virtual void sam() = 1;
};
struct X95 {
    int i;
    int j : 1;
};

void x100( X95 const *p )
{
    p->j = 0;	// no error reported
    p->i = 1;	// error reported
}

struct _toplevel;

typedef struct _intermediate
{
    const struct _toplevel *mydata;
} intermediate;

typedef struct _toplevel
{
    intermediate *otherdata;
} toplevel;

typedef const toplevel *ptoplevel;

class __export X121 {
public:
    static int __based(__segname("_DATA")) x;
    static int __far y;
    static int __based(__segname("_DATA")) xe;
    static int __far ye;
};
int __far X121::x = 1;
int __based(__segname("_DATA")) X121::y = 1;
int __based(__segname("_DATA")) __export X121::xe = __LINE__;
__declspec(dllexport) int __far X121::ye = __LINE__;
