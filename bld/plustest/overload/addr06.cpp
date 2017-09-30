#include "dump.h"

void g( signed char ) BAD;
void g( double ) BAD;
void g( int ) BAD;
void g( unsigned int ) BAD;
void g( float ) GOOD;			//<<=== this is the key
void g( short int ) BAD;
void g( unsigned short int ) BAD;
void g( long int ) BAD;
void g( unsigned long int ) BAD;

typedef void (* T1)( unsigned char );
typedef void (* T2)( signed char );
typedef void (* T3)( double );
typedef void (* T4)( int );
typedef	void (*	T0)( float );		//<<=== this is the key
typedef void (* T5)( unsigned int );
typedef void (* T6)( short int );
typedef void (* T7)( unsigned short int );
typedef void (* T8)( long int );
typedef void (* T9)( unsigned long int );

void f(	T0, T0,	T0, T0,	    T0, T0, T0, T0,
	T0, T0,	T0, T0,	    T0, T0, T0, T0 ) GOOD;
void f(	T0, T0,	T0, T0,	    T0, T0, T0, T0,
	T0, T0,	T0, T0,	    T0, T0, T0, T1 ) BAD;

void foo( void )
{
    f( &g, &g, &g, &g,  &g, &g, &g, &g,
       &g, &g, &g, &g,  &g, &g, &g, &g );
}
int main( void ) {
    foo();
    CHECK_GOOD( 25 );
    return errors != 0;
}
