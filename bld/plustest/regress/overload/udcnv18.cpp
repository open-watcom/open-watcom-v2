#include "dump.h"

int f( const int *) { GOOD; return 1;};	// (1)
int f( int *) { GOOD; return 1;};	// (2)

int g( const int &) { GOOD; return 1;}; // (3)
int g( int &) {GOOD; return 1;}; 	// (4)

struct X {
    void f() const {GOOD};		// (5)
    void f() {GOOD};			// (6)

};

void g( const X & a, X b )
{
    a.f(); 				// calls (5)
    b.f();				// calls (6)
}

static int I = 1;
struct C
{
    operator int *() { return &I; };
};

int h( const int * ) { GOOD; return 1;}	// (7)
int h( int * ){ GOOD; return 1; }	// (8)

int main()
{
   X a,b;
   C c;
   int i;
   int j  = f(&i); 	// calls (2)

   int k = g(i); 	// calls (4)
   g( a, b);

   h(c);		// calls (8)
   CHECK_GOOD(4+7+10+11+28);
   return errors != 0;
}

