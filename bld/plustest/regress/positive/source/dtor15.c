#include "fail.h"
#include <string.h>

int count;

struct S {
    char *s;
    S( char * );
    S( S const & );
    ~S();
    int operator ==( S const & );
};

S::S( char *m ) : s( new char[ strlen(m) + 1 ] ) {
    ++count;
    strcpy( s, m );
}

S::S( S const &r ) : s( new char[ strlen(r.s) + 1 ] ) {
    ++count;
    strcpy( s, r.s );
}

S::~S() {
    --count;
    if( count < 0 ) fail(__LINE__);
    delete s;
}

int S::operator ==( S const &r )
{
    return strcmp( s, r.s ) == 0;
}

void check( int v )
{
    if( v != 0 ) fail(__LINE__);
}

#define MYASSERT( bb )	(bb)?(void)0:check((bb))

int main()
{
    {
	S x("1");
	S y("2");
    
	MYASSERT( x == "1" );
	if( count != 2 ) fail(__LINE__);
	MYASSERT( y == "1" );
	if( count != 2 ) fail(__LINE__);
    }
    if( count != 0 ) fail(__LINE__);
    _PASS;
}
