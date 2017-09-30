#include "fail.h"
#include <string.h>
#include <strstrea.h>

int main() {
    {
	ostrstream os;
    
	os << ends;
	if( strcmp( os.str(), "" ) != 0 ) fail(__LINE__);
    }
    {
	ostrstream os;
    
	os << "123";
	os << ends;
	if( strcmp( os.str(), "123" ) != 0 ) fail(__LINE__);
    }
    {
	ostrstream os;
    
	os << "123";
	os.width( 6 );
	os << "456";
	os << ends;
	if( strcmp( os.str(), "123   456" ) != 0 ) fail(__LINE__);
    }
    {
	ostrstream os;
    
	os << "123";
	os.width( 6 );
	os << "456";
	os.width( 3 );
	os.flags( os.flags() | ios::left );
	os.write( "789", 3 ); // shouldn't affect 'width'
	os << "";
	os << ends;
	if( strcmp( os.str(), "123   456789   " ) != 0 ) fail(__LINE__);
    }
    _PASS;
}
