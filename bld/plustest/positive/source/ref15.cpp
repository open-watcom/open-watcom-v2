#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct S {
    S( void );
    ~S( void ) { delete [] someLongs; }
    const long &operator[](short x) const;
private:
    long *someLongs;
    long noval;
};

S::S() {
    someLongs = new long[10];
    memset( someLongs, 0, sizeof(long) * 10 );
    for( int i = 0; i < 10; ++i ) {
	someLongs[i] = 2*i+1;
    }
    noval = -1;
}

const long &S::operator[](short x) const
{
    // reference analysis must move down ? ops for l-values
    return ( x < 10 ) ? someLongs[x] : noval;
}

long extract( long const &x )
{
    char buff[90];
    memset( buff, 0xaa, sizeof( buff ) );
    return x;
}

void check( unsigned line, ... )
{
    va_list args;

    va_start( args, line );
    long c1 = va_arg( args, long );
    long c2 = va_arg( args, long );
    long c3 = va_arg( args, long );
    if( 2*c1+1 != c3 || 2*c1+1 != c2 || c2 != c3 ) fail( line );
    va_end( args );
}


int main()
{
    S la;

    for( long i=0; i < 10; i++ ) {
	check( __LINE__, i, extract( la[i] ), 2*i+1 );
    }
    check( __LINE__, -1L, extract( la[11] ), -1L );
    _PASS;
}
