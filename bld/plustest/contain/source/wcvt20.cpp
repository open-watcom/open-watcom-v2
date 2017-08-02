#include <wcvector.h>
#include <iostream.h>
#include <malloc.h>

extern int memchk_fail;
    
int main() {
    WCValOrderedVector<int> vect( 10 );

    for( int i = 0; i < 10; i++ ) {
	vect.insert( i );
    }
    memchk_fail = 1;
    if( vect.resize( 5 ) ) cout << "resize should return false\n";
    if( vect.entries() != 10 ) cout << "number of entries should be 10\n";

    memchk_fail = 0;
    if( !vect.resize( 5 ) ) cout << "resize returned false\n";
    if( vect.entries() != 5 ) cout << "number of entries should be 5\n";
    return 0;
};
