#include "fail.h"
#include <stdio.h>

typedef char T0;
typedef unsigned char T1;
typedef signed char T2;
typedef enum E1 { a,b,c } T3;
typedef enum E2 { d,e,f } T4;

void throw_something( int kind )
{
    switch( kind ) {
    case 0:
	throw T0(kind);
	break;
    case 1:
	throw T1(kind);
	break;
    case 2:
	throw T2(kind);
	break;
    case 3:
	throw T3(kind);
	break;
    case 4:
	throw T4(kind);
	break;
    default:
	struct S{};
	throw S();
    }
}

int main()
{
    int i;

    for( i = 0; i < 5; ++i ) {
	try {
	    throw_something( i );
	} catch( T0 x ) {
	    if( i != 0 ) fail(__LINE__);
	    if( x != i ) fail(__LINE__);
	} catch( T1 x ) {
	    if( i != 1 ) fail(__LINE__);
	    if( x != i ) fail(__LINE__);
	} catch( T2 x ) {
	    if( i != 2 ) fail(__LINE__);
	    if( x != i ) fail(__LINE__);
	} catch( T3 x ) {
	    if( i != 3 ) fail(__LINE__);
	    if( x != i ) fail(__LINE__);
	} catch( T4 x ) {
	    if( i != 4 ) fail(__LINE__);
	    if( x != i ) fail(__LINE__);
	} catch( ... ) {
	    if( i != 5 ) fail(__LINE__);
	}
    }
    _PASS;
}
