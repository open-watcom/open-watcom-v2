static union {
    int a;
    int b;
};

void foo( void )
{
    a = 1;
    union {
	int x;
	int y;
    };
    x = 1;
}

static int;

class C;

int i = sizeof( C );

struct A {
    static int a = 1;
    virtual int foo() = 1;
};

#include <stdarg.h>

void foo( int a )
{
    va_list args;

    va_start( args, a );
    va_end( args );
}

void foo( int a, ... )
{
    va_list args;

    va_start( args, a );
    va_end( args );
}

struct no_ctor {
    int x;
};
struct yes_ctor {
    yes_ctor(int&);
};
void test_new()
{
    new no_ctor();
    new yes_ctor();
}
struct VB {
    ~VB();
};
struct VC {
};
struct VQ : VB {
};
struct VA : VB, VC, VQ {
    int a;
    virtual ~VA() = 0;
};

struct VD : VA {
    int d;
    ~VD();
};
