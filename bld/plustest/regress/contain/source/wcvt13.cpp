// test grow parameter

#include <iostream.h>
#include <wcvector.h>
#include "testexpt.h"

int main() {
    int a = 1;
    int b = 2;
    int i;

    cout << "Should see 1 2 3 4 5\n";
    WCValOrderedVector<int> vect( 0 );
    vect.exceptions( WCExcept::check_all );
    test_except( vect.insert( a ), resize_required, "1" );
    vect.exceptions( 0 );
    for( i = 0; i < WCDEFAULT_VECTOR_RESIZE_GROW; i++ ){
	vect.insert( a );
    }
    vect.exceptions( WCExcept::check_all );
    test_except( vect.insert( a ), resize_required, "2" )

    WCValOrderedVector<int> vect2( 0, 4 );
    vect2.exceptions( WCExcept::check_all );
    test_except( vect2.insert( a ), resize_required, "3" );
    vect2.exceptions( 0 );

    for( i = 0; i < 4; i++ ){
	vect2.insert( a );
    }
    vect2.exceptions( WCExcept::check_all );
    test_except( vect2.insert( a ), resize_required, "4" );

    WCValOrderedVector<int> vect3( 4, 0 );
    for( i = 0; i < 4; i++ ){
	if( !vect3.insert( a ) ) cout << "insert failed\n";
    }
    if( vect3.insert( a ) ) cout << "insert did not fail\n";
    vect3.exceptions( WCExcept::check_all );
    test_except( vect3.insert( a ), resize_required, "5" );

    vect.clear();
    vect2.clear();
    vect3.clear();
    return 0;
}
