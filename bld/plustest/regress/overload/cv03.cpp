#include "dump.h"

// all should choose (...)
void fcr( ... ) GOOD;
void fcr( int & const b ) BAD;

void fvr( ... ) GOOD;
void fvr( int & volatile b ) BAD;

void fcp( ... ) GOOD;
void fcp( int * const b ) BAD;

void fvp( ... ) GOOD;
void fvp( int * volatile b ) BAD;

class A {
    public:
	const int ci;
	volatile int vi;
	A() : ci(0) BAD;
};

void fcm( ... ) GOOD;
void fcm( int A::* const ) BAD;
void fvm( ... ) GOOD;
void fvm( int A::* volatile ) BAD;

void g( void )
{
	const int a = 1;
	volatile int b;
	const int & ra = a;
	volatile int & rb = b;
	const int * pa = &a;
	volatile int * pb = &b;
	const int A::* ma;
	volatile int A::* mb;
    fcr( ra );
    fvr( rb );
    fcp( pa );
    fvp( pb );
    fcm( ma );
    fvm( mb );
}
int main( void ) {
    g();
    CHECK_GOOD( 82 );
    return errors != 0;
}
