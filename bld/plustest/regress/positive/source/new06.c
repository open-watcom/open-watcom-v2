#include "fail.h"

#define NUM_ALLOCS	100

#define INCR	32

unsigned allocs;
unsigned index;
char buff[NUM_ALLOCS*INCR];
unsigned asize[NUM_ALLOCS];

void *stack_alloc( unsigned size )
{
    unsigned sindex = index;
    index += INCR;
    asize[allocs++] = size;
    return &buff[sindex];
}

void stack_free( void *p )
{
    --allocs;
    index -= INCR;
    if( p != &buff[index] ) {
	fail(__LINE__);
    }
}

void stack_free_size( void *p, unsigned size )
{
    stack_free( p );
    if( size != asize[allocs] ) {
	fail(__LINE__);
    }
}

void *operator new( unsigned s )
{ return stack_alloc( s ); }
void *operator new[]( unsigned s )
{ return stack_alloc( s ); }
void operator delete( void *p )
{ stack_free( p ); }
void operator delete[]( void *p )
{ stack_free( p ); }

int count;

typedef double S;

struct C {
    C() {++count;}
};

struct D {
    ~D() { --count; }
};

struct CD : C, D {
};

struct VD {
    virtual ~VD() { --count; }
};

struct OD {
    void operator delete( void *p )
    { stack_free( p ); }
};

struct ODS {
    void operator delete( void *p, unsigned s )
    { stack_free_size( p, s ); }
};

struct DOD : D, OD {
};

struct DODS : D, ODS {
};

struct VDOD : VD, OD {
};

struct VDODS : VD, ODS {
};

S *pS;
C *pC;
D *pD;
CD *pCD;
VD *pVD;
OD *pOD;
ODS *pODS;
DOD *pDOD;
DODS *pDODS;
VDOD *pVDOD;
VDODS *pVDODS;

int main()
{
    pS = new S;
    pC = new C;
    pD = new D;
    pCD = new CD;
    pVD = new VD;
    pOD = new OD;
    pODS = new ODS;
    pDOD = new DOD;
    pDODS = new DODS;
    pVDOD = new VDOD;
    pVDODS = new VDODS;
    delete pVDODS;
    delete pVDOD;
    delete pDODS;
    delete pDOD;
    delete pODS;
    delete pOD;
    delete pVD;
    delete pCD;
    delete pD;
    delete pC;
    delete pS;
    _PASS;
}
