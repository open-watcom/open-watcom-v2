#include "fail.h"

int p_count;

struct p {
    int x,y;
    p( int x, int y ) : x(x), y(y) { ++p_count; }
    p( p const &s ) : x(s.x), y(s.y) { ++p_count; }
    ~p() {--p_count;}
};

int main()
{
    for( int j = 0; j < 2; ++j ) {
	try {
	    switch( j ) {
	    case 0:
		throw p(-1,-2);
		break;
	    case 1:
		throw -1;
		break;
	    }
	    fail(__LINE__);
	} catch( int k ) {
	    if( j != 1 ) fail(__LINE__);
	    if( k != -1 ) fail(__LINE__);
	} catch( p v ) {
	    if( j != 0 ) fail(__LINE__);
	    if( v.x != -1 ) fail(__LINE__);
	    if( v.y != -2 ) fail(__LINE__);
	} catch( ... ) {
	    fail(__LINE__);
	}
	if( p_count != 0 ) fail(__LINE__);
    }
    _PASS;
}
