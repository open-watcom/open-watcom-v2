#include "dump.h"

struct S {
    int foo(int){ GOOD; return 0; };		// (1)
    static int goo(int) { GOOD; return 0; };	// (2)
    static int foo( double ) { return 0; };	// (3)
    int moo( int ) { GOOD; return 0; };		// (4)
    void extra(void);
};

typedef int (S::*MPF)(int);
typedef int (* SMPF)(int);

struct T : S {
    static MPF array[];
    static MPF array2[];
    static MPF array3[];
    static SMPF array4[];
};

// tests extensions of S::foo -> &S::foo for non-static members
int (S::*f3)(int) = S::moo; // not overloaded (3)
int (S::*f4)(int) = S::foo; // overloaded (1)

int (S::*barr[2])(int) = { S::moo,	// not overloadeded (4)
                           S::foo };	// overloaded (1)


// FOR MFC
// tests moo -> S::moo (-> &S::moo) extension when in S:: context
// doing data init
#if __WATCOM_REVISION__ >= 8
void S::extra(void) // to create S:: context
{
int (S::*barr[2])(int) = { &moo,	// not overloadeded (4)
                           &foo };	// overloaded (1)
}

MPF T::array2[] = { S::foo }; 		// should need -ze but doesn't yet
MPF T::array3[] = { &foo, foo };	// NEEDED FOR MFC
MPF T::array[] = { &S::foo }; 		// ANSI
#endif
SMPF T::array4[] = { &goo, goo }; 	// ANSI

void f( MPF p )
{
    S s;


    (s.*p)(0);
}

void f2( SMPF p )
{
    (*p)(0);
}

int main()
{
#if __WATCOM_REVISION__ >= 8
    f( T::array[0] );
#else
    FORCE_GOOD(4);
#endif

    CHECK_GOOD(4);
    f2( T::array4[0] );
    CHECK_GOOD(4+5);
#if __WATCOM_REVISION__ >= 8
    f( T::array2[0] );
#else
    FORCE_GOOD(4);
#endif
    CHECK_GOOD(4+5+4);
#if __WATCOM_REVISION__ >= 8
    f( T::array3[0] );
#else
    FORCE_GOOD(4);
#endif
    CHECK_GOOD(4+5+4+4);
    return errors != 0;
}
