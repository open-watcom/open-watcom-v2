#include <iostream.h>
#include <strstrea.h>

#include "fail.h"

int test( void ) {
    #ifdef __WATCOM_INT64__
    {
	signed __int64 i,j;
	strstream s;

	i = -1234567890123456789I64;
	s << i;
	s >> j;
	if( i != j ) return( 0 );
    }
    {
	unsigned __int64 i,j;
	strstream s;

	i = 1234567890123456789I64;
	s << i;
	s >> j;
	if( i != j ) return( 0 );
    }
    {
	unsigned __int64 i,j;
	strstream s;

	i = 0x1122334455667788I64;
	s << hex << i;
	s >> j;
	if( i != j ) return( 0 );
    }
    #endif
    return( 1 );
}

int main( void ) {
    if( !test() ) _fail;
    _PASS;
}
