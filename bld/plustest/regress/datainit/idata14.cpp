#include <stdio.h>

#define FORMAT_STRING "int=%d int=%d A=%d B=%d int=%d C=[%d,%d,%d,%d,%d]\n"

class A { public: int a; A( int b, int c ){ a = b*c; }; };
class B { public: int b; };
class C {
    public:
	int bit1 : 4;
	int bit2 : 4;
	int bit3 : 4;
	int bit4 : 4;
	int bit5 : 4;
};

int f( void ) { return 317; }

// External Public
// simple, ctor, aggregate
    int eps = 1;
    int epc( 2 );
    A epca( 3, 3 );
    B epca2 = { 4 };
    int epa[1] = { 5 };
    C epab = { 6, 7, 8, 9, 10 };
    int ep_hook = printf( "EP:" FORMAT_STRING,
		    eps, epc, epca.a, epca2.b, epa[0],
		    epab.bit1, epab.bit2, epab.bit3, epab.bit4, epab.bit5 );

// External Static
// simple, ctor, aggregate
    static int ess = 1;
    static int esc( 2 );
    static A esca( 3, 3 );
    static B esca2 = { 4 };
    static int esa[1] = { 5 };
    static C esab = { 6, 7, 8, 9, 10 };
    int es_hook = printf( "ES:" FORMAT_STRING,
		    ess, esc, esca.a, esca2.b, esa[0],
		    esab.bit1, esab.bit2, esab.bit3, esab.bit4, esab.bit5 );

// Internal Auto
// simple, ctor, aggregate
void iaf( void )
{
    auto int ias = 11;
    auto int ias2 = f();
    auto int iac( 12 );
    auto A iaca( 13, 14 );
    auto B iaca2 = { 15 };
    auto int iaa[1] = { 16 };
    auto C iaab = { 6, 7, 8, 9, 10 };
    printf( "IA:" FORMAT_STRING,
    	  ias, iac, iaca.a, iaca2.b, iaa[0], 
	  iaab.bit1, iaab.bit2, iaab.bit3, iaab.bit4, iaab.bit5 );
    printf( "\tIA2: int=%d\n", ias2 );
}

// Internal Static
// simple, ctor, aggregate
void isf( void )
{
    static int iss = 17;
    static int iss2 = f();
    static int isc( 19 );
    static A isca( 20, 20 );
    static B isca2 = { 21 };
    static int isa[1] = { 22 };
    static int isa2[] = { 23, 24, 25 };
    static C isab = { 6, 7, 8, 9, 10 };
    printf( "IS:" FORMAT_STRING,
    	  iss, isc, isca.a, isca2.b, isa[0],
	  isab.bit1, isab.bit2, isab.bit3, isab.bit4, isab.bit5 );
    printf( "\tIS2: int=%d int=[%d,%d,%d]\n",
          iss2, isa2[0], isa2[1], isa2[2] );
}

int main( void )
{
    iaf();
    isf();
    return 0;
}
