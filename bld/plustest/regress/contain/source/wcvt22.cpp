#include <wcvector.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

int main() {
    WCValOrderedVector<str_data> vect;
    str_data first( "First" );
    str_data negative( "Negative" );
    str_data too_high( "Too high" );

    vect.insertAt( 5, first );
    cout << "Number of entries "  << vect.entries() << "\n";
    vect.insertAt( -100, negative );
    vect.insertAt( 1000, too_high );
    cout << "Vector elements (should be \"Negative  First  Too high\"\n";
    for( int i = 0; i < vect.entries(); i++ ) {
	cout << vect[ i ] << "  ";
    }
    cout << "\n";

    vect.exceptions( WCExcept::index_range );
    vect.insertAt( vect.entries(), too_high );
    vect.insertAt( 0, negative );
    test_except( vect.insertAt( vect.entries() + 1, too_high ), index_range, "too high" );
    test_except( vect.insertAt( -1, negative ), index_range, "-1" );
    return 0;
}
