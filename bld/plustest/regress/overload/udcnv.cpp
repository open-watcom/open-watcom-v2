#include "dump.h"

//User-Defined Conversions:
// this file should compile without error
// this file needs a lot more work.......

//- constructors take one argument and convert to resulting class type
class Actor {
    public:
	Actor( int ) GOOD;
};
void fctor( ... ) BAD;
void fctor( Actor ) GOOD;
void gctor( void )
{
    fctor( 1 );
}

//- conversion functions must be member operator functions of class A
//	that convert class A into the resulting type
class Aint {
    public:
	operator int() { GOOD; return 0; }
};
void fint( ... ) BAD;
void fint( int ) GOOD;
void gint( void )
{
    Aint a;
    fint( a );
}

//- user-defined conversions are selected based on the resulting type
class Art {
    public:
	operator char() { BAD; return 'a'; }
	operator int() { GOOD; return 0; }
};
void frt( ... ) BAD;
void frt( int ) GOOD;
void grt( void )
{
    Art a;
    frt( a );
}


//- user-defined conversions may be used in conjunction with standard
//	conversions in overloading resolution,
//	- standard conversions may be applied to the input of the
//	  user-defined conversion
//	- standard conversions may be applied to the output of the
//	  user-defined conversion
class Asc {
    public:
	Asc() GOOD;
    	Asc( double ) GOOD;
	operator char() { GOOD; return 'a'; }
};
void fsc( ... ) BAD;
void fsc( int ) GOOD;
void hsc( ... ) BAD;
void hsc( Asc ) GOOD;
void gsc( void )
{
    Asc a;
    int b;
    fsc( a );	// f( (int) A::operator char( a ) );
    hsc( b );	// h( A::A( (double) b ) );
}

int main( void ) {
    gctor();
    gint();
    grt();
    gsc();
    CHECK_GOOD( 444 );
    return errors != 0;
}
