// (JWW) our implementation of String operator+( String&, const String& )
// is incorrect and causes a warning in the following program.
//
// The standard prototype is: String operator+( const String&, const String& )


#include <string.hpp>

String conc( const char* a, const char* b, const char* c )
{
    return String( a ) + String( b ) + String( c );
}
