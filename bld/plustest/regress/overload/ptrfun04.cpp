#include "dump.h"


// Rule: you can only take the address of a static member function
// Exception: &S::f where f is non-static will match a constant member
//            pointer S::*


#if __WATCOM_REVISION__ >= 8
struct S {

    int foo( int ) { GOOD; return 0; } ;	
    static int foo( double ) { return 0; }; 
    int moo( int ) { GOOD; return 0; }; 
    static int boo( double ) { GOOD; return 0; };

    int goo( int (S::*f)( int ) ) { GOOD; return 0; }; 	// (1)
    int goo( int (*f)( double ) ) { GOOD; return 0; };	// (2)

    void fun()
    {
        goo( S::foo );	// should find (2) only with -za because member to
			// mptr auto conversion not ANSI, so will be of type
			// function, which converts auto. to ptr to fn and
			// matches the static member function
			//
		       	// ambiguous with extensions enabled (mbr -> mptr
			// conversion allowed) (see diag0252.c)
			// Microsoft and Borland: ambig
	CHECK_GOOD(18);

        goo( &foo );    // should find (2) only
			// Microsoft : & illegal on bound member function (??)
			// Borland : ambiguous
	CHECK_GOOD(18+18);
        goo( foo );	// should find (2) only
			// Microsoft and Borland: goo ambiguous call
	CHECK_GOOD(18+18+18);
	goo( &S::moo );	// should find (1) only
	CHECK_GOOD(18+18+18+17);
	goo( &S::boo );	// should find (2) only
	CHECK_GOOD(18+18+18+17+18);
    }
};



int main()
{
    S s;

    s.fun();
    return errors != 0;
}
#else
int main( void )
{
  FORCE_GOOD(18);
  CHECK_GOOD(18);
  FORCE_GOOD(18);
  CHECK_GOOD(18+18);
  FORCE_GOOD(18);
  CHECK_GOOD(18+18+18);
  FORCE_GOOD(17);
  CHECK_GOOD(18+18+18+17);
  FORCE_GOOD(18);
  CHECK_GOOD(18+18+18+17+18);
  return errors != 0;
}
#endif
