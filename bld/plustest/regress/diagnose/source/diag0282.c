static char c;
static int an_int;

static const int zero_int = 0;

void foo1( char *b = 'A' );		// error
void foo2( char *b = c );		// error
void foo3( char *b = 0 );		// OK
void foo4( char *b = zero_int );	// OK

void goo1( int * i = 1 );		// error
void goo2( int * i = an_int );		// error
void goo3( int * i = 0 );		// OK
void goo4( int * i = zero_int );	// OK

void moo1( void * i = 1 );		// error
void moo2( void * i =  an_int );	// error
void moo3( void * i =  0 );		// OK
void moo4( void * i =  zero_int );	// OK

struct S {
    int *f;
    void *g;
    char *h;
    S() : f(4), g(3), h('A') {}		// 3 errors
    S(int i) : f(0), g(0), h(0) { i = i;}	// OK
    S(short j) : f(zero_int), g(zero_int), h(zero_int) {j=j;}	// OK
    S(int i, char d) : f(i), h(d) {}		// 2 errors
};
