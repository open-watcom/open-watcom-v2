typedef DOUBLE1;
typedef DOUBLE1 DOUBLE2;
void x3( int m )
{
    int i;

    for( i = 0; i < m; ++i ) {
	continue;
	int x = 0;
    }
    while( i++ < (2*m) ) {
	continue;
	int x = 0;
    }
    do {
	continue;
	int x = 0;
    } while( i++ < (3*m) );
}

class X;
extern X *xp;
extern int arr[];
typedef int UNKA[];
UNKA *arrp;
UNKA **arrpp;

void x28()
{
    xp++;	// error
    arrp++;	// error
    arrpp++;
}

struct X { int i; };
int arr[10];

X x;

void x40() {
    xp = &x;
    arrp = &arr;// error
    xp++;
    arrp++;	// error
}

#include <stdio.h>

void x49(const char* x) { printf( "%s\n", x ); }

#define x51(x) x49(#x)

void x53( long x );

void x55()
{
    x49("\xffffffffffffffffffffffffffffffff");
    x49("09943543953495349593459349593459345");
    x51(test);
    x51(998723452363453);
    x51(0998723452363453);
    x53(998723452363453);
    x53(0998723452363453);
}
    #if 998723452363453 < 0998723452363453
    #endif

struct X68 { int q; struct N { int q; }; struct D : N {};  void ack(); };

void x70( X68 *p, int *w, X68::N *r, X68::D *s )
{
    s->X68::N::s = 1;
    w->x = 1;
    p->x = 1;
    r->n = 1;
}

void X68::ack() {
    e = 1;
}
