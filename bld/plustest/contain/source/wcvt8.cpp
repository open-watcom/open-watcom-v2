#include <wcvector.h>
#include <iostream.h>
#include "strdata.h"

void test1();
void test2();
void test3();
void test4();


int main(){
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
    str_data t1234567 	= str_data( "1234567" );
    str_data t13 	= str_data( "13" );
    str_data t542 	= str_data( "542" );
    str_data t;
    int i;

    WCValOrderedVector<str_data> * 	vect
    				= new WCValOrderedVector<str_data>( 2 );
				
    cout << "ValOrdered\n";
    if( vect->removeAt( 5 ) ) cout << "should not return true\n";
    if( vect->removeFirst() ) cout << "should not return true\n";
    if( vect->removeLast() ) cout << "should not return true\n";

    vect->exceptions( WCExcept::check_all^WCExcept::resize_required );

    if( vect->remove( t50 ) ) cout << "Remove returned true on empty\n";
    if( 0 != vect->removeAll( t50 ) ) cout << "RemoveAll != 0 on empty\n";

    vect->insert( t25 );
    vect->insert( t0 );
    vect->insert( t50 );
    vect->insert( t0 );
    vect->insert( t1234567 );

#define print1						\
    for( i = 0; i < vect->entries(); i++ ){		\
	cout << ( *vect )[ i ] << ' ';			\
    }							\
    cout << "\n";

    print1;

    cout << "vect->removeAll 0\n";
    if( vect->removeAll( t0 ) != 2 ) cout << "not 2 0's\n";
    print1;

    cout << "removing 50\n";
    if( !vect->remove( t50 ) ) cout << "did not return true\n";
    print1;

    vect->exceptions( 0 );

    cout << "vect->removeAt 10\n";
    if( !vect->removeAt( 10 ) ) cout << "did not get 1234567";
    print1;

    vect->exceptions( WCExcept::check_all );

    if( vect->remove( t13 ) ) cout << "vect->remove 13 returned true\n";

    if( vect->removeAll( t542 ) != 0 ) cout << "vect->remove all 540 != 0\n";

    print1;

    vect->clear();
    delete( vect );
    cout.flush();
}

void test2() {
    int i;

    str_data a = str_data( "25" );
    str_data b = str_data( "50" );
    str_data bb = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    str_data f2 = str_data( "542" );
    WCPtrOrderedVector<str_data> 	vect( 2 );
    
    cout << "PtrOrdered\n";

    str_data *t = vect.removeAt( 5 );
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.removeFirst();
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.removeLast();
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.remove( &bb );
    if( t != 0 ) cout << "should have returned NULL";
    if( 0 != vect.removeAll( &a ) ) cout << "RemoveAll != 0 on empty\n";

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );

#define print2					\
    for( i = 0; i < vect.entries(); i++ ){	\
	cout << *vect[ i ] << ' ';		\
    }						\
    cout << "\n";

    print2;

    if( 0 != vect.remove( &f2 ) )cout << "removing 542 returned != NULL\n";

    cout << "removingat -354\n";
    t = vect.removeAt( -354 );
    if( t != &a ) cout << "did not get 25\n";
    print2;

    vect.exceptions( WCExcept::check_all );

    cout << "removingAll 0\n";
    if( vect.removeAll( &c ) != 2 ) cout << "did not remove 2 0's\n";
    print2;

    cout << "removing 50\n";
    if( vect.remove( &bb ) != &b ) cout << "did not remove 1 50\n";
    print2;
    cout.flush();
    vect.clear();
}


void test3() {
//    str_data t14312 	= str_data( "14312" );
    str_data t25 	= str_data( "25" );
//    str_data t30	= str_data( "30" );
    str_data t0 	= str_data( "0" );
    str_data t0_	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t1234567 	= str_data( "1234567" );
    str_data t34143	= str_data( "34143" );
    str_data t13 	= str_data( "13" );
    str_data t542 	= str_data( "542" );
    str_data tn1	= str_data( "-1" );

    str_data t;
    int i;

    WCValSortedVector<str_data> 	vect( 0 );

    cout << "ValSorted\n";

    if( vect.removeAt( 5 ) ) cout << "should not return true\n";
    if( vect.removeFirst() ) cout << "should not return true\n";
    if( vect.removeLast() ) cout << "should not return true\n";

    vect.exceptions( WCExcept::check_all );

    if( vect.remove( t50 ) ) cout << "Remove returned true on empty\n";
    if( 0 != vect.removeAll( t50 ) ) cout << "RemoveAll != 0 on empty\n";

    vect.exceptions( WCExcept::index_range );

    vect.insert( t25 );
    vect.insert( t0 );
    vect.insert( t50 );
    vect.insert( t1234567 );
    vect.insert( t0_ );
    vect.insert( tn1 );
    vect.insert( t34143 );

#define print3						\
    for( i = 0; i < vect.entries(); i++ ){		\
	cout << vect[ i ] << ' ';			\
    }							\
    cout << "\n";

    print3;

    cout << "vect.removeAll 0\n";
    if( vect.removeAll( t0_ ) != 2 ) cout << "not 2 0's\n";
    print3;

    cout << "removingAt 3\n";
    if( !vect.removeAt( 3 ) ) cout << "did not remove at 34143\n";
    print3;

    cout << "removing 25\n";
    if( !vect.remove( t25 ) ) cout << "remove 25 returned false\n";

    if( vect.remove( t13 ) ) cout << "vect.remove 13 returned true\n";

    if( vect.removeAll( t542 ) != 0 ) cout << "vect.remove all 540 != 0\n";

    cout << "removing Last\n";
    if( !vect.removeLast() ) cout << "50 not last\n";
    print3;

    cout << "removing First\n";
    if( !vect.removeFirst() ) cout << "-1 not last\n";
    print3;
    cout.flush();
    vect.clear();
}

void test4() {
    str_data a = str_data( "25" );
    str_data aa = str_data( "25" );
    str_data b = str_data( "50" );
    str_data bb = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
//    str_data f = str_data( "13" );
    str_data f2 = str_data( "542" );
    str_data g = str_data( "-1" );
    str_data h = str_data( "34143" );
    str_data i = str_data( "-1" );
    int k;

    WCPtrSortedVector<str_data> 	vect( 5 );

    cout << "PtrSorted\n";
    str_data *t = vect.removeAt( 5 );
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.removeFirst();
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.removeLast();
    if( t != 0 ) cout << "should have returned NULL";
    t = vect.remove( &bb );
    if( t != 0 ) cout << "should have returned NULL";
    if( 0 != vect.removeAll( &a ) ) cout << "RemoveAll != 0 on empty\n";

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );
    vect.insert( &g );
    vect.insert( &h );
    vect.insert( &i );

#define print4					\
    for( k = 0; k < vect.entries(); k++ ){	\
	cout << *vect[ k ] << ' ';		\
    }						\
    cout << "\n";

    print4;

    if( vect.remove( &f2 ) )cout << "removing 542 returned true\n";

    cout << "removingat 3414\n";
    t = vect.removeAt( 3414 );
    if( t != &b ) cout << "did not get 50\n";
    print4;

    vect.exceptions( WCExcept::check_all );

    cout << "removingAll 0\n";
    if( vect.removeAll( &c ) != 2 ) cout << "did not remove 2 0's\n";
    print4;

    cout << "removing -1\n";
    if( vect.remove( &i ) != &g ) cout << "did not remove -1\n";
    print4;

    if( vect[ 0 ] != &i ) cout << "correct -1 not left\n";

    cout << "removingAll 25\n";
    if( vect.removeAll( &aa ) != 1 ) cout << "did not remove 1 25\n";
    print4;

    t = vect.removeAt( 2 );
    if( t != &h ) cout << "wrong value removed from loc 2\n";

    cout << "removing Last\n";
    t = vect.removeLast();
    if( t != &d ) cout << "1234 not last\n";
    print4;

    cout << "removing First\n";
    t = vect.removeFirst();
    if( t != &i ) cout << "-1 not last\n";
    print4;
    cout.flush();
    vect.clear();
}
