#include "fail.h"

int ctors;
unsigned assigns;
unsigned copies;

unsigned signature;

struct CD {
    unsigned sig;
    inline CD();
    CD( CD const &s );
    ~CD();
    void operator =( CD const &s );
};

CD::CD() : sig(signature++) {
    ++ctors;
}

CD::CD( CD const &s ) : sig(signature++) {
    ++ctors;
    ++copies;
}

CD::~CD() {
    --ctors;
    if( ctors < 0 ) {
	fail(__LINE__);
	ctors = 0;
    }
}

void CD::operator =( CD const &s ) {
    ++assigns;
    sig = signature++;
}

CD loop_switch( int v )
{
    CD x;

    for( int i = 0; i < 10; ++i ) {
	if( i == v ) {
	    switch( v ) {
	    case 1:
		return x;
	    case 2:
		return CD();
	    case 3:
		break;
	    default:
		return CD();
	    }
	    break;
	}
    }
    return x;
}

CD switch_loop( int v )
{
    CD x;

    switch( v ) {
    case 1:
	return x;
    case 2:
	return CD();
    case 3:
	break;
    default:
	return CD();
    case 4:
    case 5:
    case 6:
	for( int i = 0; i < 10; ++i ) {
	    if( i == v ) {
		if( v == 4 ) {
		    return x;
		} else if( v == 5 ) {
		    return CD();
		}
	    }
	    break;
	}
	break;
    }
    return x;
}

int main()
{
    {
	CD x;
    
	for( int i = 1; i < 10; ++i ) {
	    x = loop_switch( i );
	    if( assigns != i ) fail(__LINE__);
	    if( ctors < 1 ) fail(__LINE__);
	    while( ctors > 1 ) {
		fail(__LINE__);
		--ctors;
	    }
	    switch_loop( i );
	    if( assigns != i ) fail(__LINE__);
	    if( ctors < 1 ) fail(__LINE__);
	    while( ctors > 1 ) {
		fail(__LINE__);
		--ctors;
	    }
	}
    }
    if( ctors != 0 ) fail(__LINE__);
    _PASS;
}
