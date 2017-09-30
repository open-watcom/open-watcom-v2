#include <wcvector.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

void test1();
void test2();
void test3();
void test4();


int main(){
    cout << "Should be 15 supposed to happen exceptions\n";
    test1();
    test2();
    test3();
    test4();
    cout.flush();
    return 0;
}


void test1() {
    str_data t25 	= str_data( "25" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t;

    WCValOrderedVector<str_data> * 	vect
    				= new WCValOrderedVector<str_data>( 2 );
				
    vect->exceptions( WCExcept::check_all );
    cout << "ValOrdered\n";

    test_except( vect->removeAt( 0 ), empty_container, "index 1" );
    test_except( vect->removeFirst(), empty_container, "index 1b" );
    test_except( vect->removeLast(), empty_container, "index 1c" );
    vect->insert( t25 );
    vect->insert( t0 );

    test_except( vect->removeAt( -50 ), index_range, "index 2" );
    test_except( vect->removeAt( 50 ), index_range, "index 3" );

    not_happen_except( vect->removeLast(), "index 4" );
    not_happen_except( vect->removeFirst(), "index 5" );

    vect->clear();
    delete( vect );
    cout.flush();
}


void test2() {
    str_data *t;
    str_data a = str_data( "25" );
    str_data b = str_data( "50" );
    str_data c = str_data( "0" );
    WCPtrOrderedVector<str_data> 	vect( 2 );
    
    vect.exceptions( WCExcept::check_all );
    cout << "PtrOrdered\n";

    test_except( t = vect.removeAt( 0 ), empty_container, "index 1" );
    test_except( t = vect.removeFirst(), empty_container, "index 1b" );
    test_except( t = vect.removeLast(), empty_container, "index 1c" );

    vect.insert( &a );
    vect.insert( &c );

    test_except( t = vect.removeAt( -1 ), index_range, "index 2" );
    test_except( t = vect.removeAt( 2 ), index_range, "index 3" );

    not_happen_except( t = vect.removeLast(), "index 4" );
    not_happen_except( t = vect.removeFirst(), "index 5" );
    cout.flush();
    vect.clear();
}


void test3() {
    str_data t25 	= str_data( "25" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t;

    WCValSortedVector<str_data> 	vect( 3 );

    // make sure index range not happening
    vect.exceptions( WCExcept::resize_required );

    cout << "ValSorted\n";


    not_happen_except( vect.removeAt( 0 ), "index 1" );
    not_happen_except( vect.removeFirst(), "index 1b" );
    not_happen_except( vect.removeLast(), "index 1c" );

    vect.insert( t25 );
    vect.insert( t0 );
    vect.insert( t50 );

    not_happen_except( vect.removeAt( -50 ), "index 2" );
    not_happen_except( vect.removeAt( 50 ), "index 3" );
    cout.flush();
}

void test4() {
    str_data a = str_data( "25" );
    str_data b = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    str_data *t;

    WCPtrSortedVector<str_data> 	vect( 5 );

    vect.exceptions( WCExcept::check_all );

    cout << "PtrSorted\n";

    test_except( t = vect.removeAt( 0 ), empty_container, "index 1" );
    test_except( t = vect.removeFirst(), empty_container, "index 1b" );
    test_except( t = vect.removeLast(), empty_container, "index 1c" );

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );

    test_except( t = vect.removeAt( -1 ), index_range, "index 2" );
    test_except( t = vect.removeAt( 5 ), index_range, "index 3" );

    not_happen_except( t = vect.removeLast(), "index 4" );
    not_happen_except( t = vect.removeFirst(), "index 5" );
    cout.flush();
    vect.clear();
}
