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
    str_data t14312 	= str_data( "14312" );
    str_data t25 	= str_data( "25" );
    str_data t30 	= str_data( "30" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t1234567 	= str_data( "1234567" );
    str_data t13 	= str_data( "13" );
    str_data t542 	= str_data( "542" );
    str_data a;

    WCValOrderedVector<str_data> * 	vect
    				= new WCValOrderedVector<str_data>( 2 );

    if( !vect->isEmpty() ) cout << "new list is not empty\n";
    if( vect->entries() != 0 )cout << "ValOrdered >0 entries after init\n" ;
    if( -1 != vect->index( t14312 ) ) cout << "Found 14312 in an empty list\n";
    if( vect->contains( t25 ) ) cout << "Found 25 in empty list\n";
    if( vect->find( t30, a ) ) cout << "Found " << a << "in empty list\n";

    a = vect->first();			// just to make sure these don't cause
    					// a runtime error
    if( vect->isEmpty() ) cout << "non-empty list is empty\n";

    vect->insert( t25 );
    vect->insert( t0 );
    vect->insert( t50 );
    vect->insert( t0 );
    vect->insert( t1234567 );

    if( !vect->contains( t25 ) ) cout << "didn't find 25\n";
    if( !vect->contains( t50 ) ) cout << "didn't find 50\n";
    if( !vect->contains( t1234567 ) ) cout << "didn't find 1234567\n";
    if( vect->contains( t13 ) ) cout << "did contain 13\n";

    if( vect->entries() != 6 )cout << "ValOrdered != 5 entries after insert\n" ;

    if( vect->find( t25, a ) ) cout << "Found " << a << " (OK)\n";
    if( vect->find( t13, a ) ) cout << "did find 13\n";

    if( vect->first() != str_data() ) cout << "\"\" (inserted by first) not first\n";
    if( vect->last() != t1234567 ) cout << "1234567 not last\n";

    if( vect->index( t0 ) != 2 ) cout << "0 not found a index 2\n";
    if( vect->index( t542 ) != -1 ) cout << "found 542 index\n";

    if( vect->occurrencesOf( str_data( "0" ) ) != 2 ) cout << "occur 0 != 2\n";
    if( vect->occurrencesOf( str_data( "7634" ) ) != 0 ) cout << "occur 7634 != 0\n";
    
    delete( vect );
}

void test2() {
    int i;

    str_data a = str_data( "25" );
    str_data aa = str_data( "25" );
    str_data b = str_data( "50" );
    str_data bb = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    str_data f = str_data( "13" );
    str_data f2 = str_data( "542" );
    str_data find_var;
    WCPtrOrderedVector<str_data> 	vect( 2 );
    
    if( !vect.isEmpty() ) cout << "new list is not empty\n";
    if( vect.entries() != 0 )cout << "PtrOrdered >0 entries after init\n" ;
    if( vect.find( &a ) ) cout << "Found in empty ptrordered list\n";
    if( vect.contains( &a ) ) cout << "Found 25 in empty list\n";
    if( vect.occurrencesOf( &f ) ) cout << "13 occurred in empty list\n";

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );

    if( vect.entries() != 5 )cout << "PtrOrdered != 5 entries after insert\n" ;
    if( !vect.contains( &aa ) ) cout << "didn't find 25\n";
    if( !vect.contains( &bb ) ) cout << "didn't find 50\n";
    if( !vect.contains( &d ) ) cout << "didn't find 1234\n";
    if( vect.contains( &f ) ) cout << "did find 13\n";

    find_var = *vect.find( &bb );
    cout << "Found " << find_var << " (OK)\n";
    if( vect.find( &f ) ) cout << "did find 13\n";

    if( vect.first() != &a ) cout << "ptr ord 25 not first\n";
    if( vect.last() != &d ) cout << "ptr ord 1234567 not last\n";

    if( vect.index( &c ) != 1 ) cout << "0 not found a index 1\n";
    if( vect.index( &f2 ) != -1 ) cout << "found 542 index\n";

    if( vect.isEmpty() ) cout << "non-empty ptrord list is empty\n";

    if( vect.occurrencesOf( &aa ) != 1 ) cout << "25 occurred != 1 time\n";

    vect.resize( 5 );

    for( i = 0; i < 5; i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
}


void test3() {
    str_data t14312 	= str_data( "14312" );
    str_data t25 	= str_data( "25" );
    str_data t30	= str_data( "30" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t1234567 	= str_data( "1234567" );
    str_data t34143	= str_data( "34143" );
    str_data t13 	= str_data( "13" );
    str_data t542 	= str_data( "542" );
    str_data tn1	= str_data( "-1" );
    str_data a;
    str_data b;
    str_data inserted_by_last;
    int i;

    WCValSortedVector<str_data> 	vect( 0 );

    if( !vect.isEmpty() ) cout << "new list is not empty\n";
    if( vect.contains( t25 ) ) cout << "Found 25 in empty list\n";
    if( vect.find( t30, a ) ) cout << "Found " << a << "in empty list\n";
    if( vect.entries() != 0 )cout << "ValSorted >0 entries after init\n" ;
    if( vect.occurrencesOf( t50 ) != 0 ) cout << "50 occurred != 0 time\n";

    inserted_by_last = vect.last();

    vect.insert( t25 );
    if( vect.isEmpty() ) cout << "non-empty valsort list is empty\n";
    vect.insert( t0 );
    vect.insert( t50 );
    vect.insert( t1234567 );
    vect.insert( t0 );
    vect.insert( tn1 );
    vect.insert( t34143 );
    if( vect.entries() != 8 )cout << "ValSorted != 8 entries after insert\n" ;

    if( !vect.contains( t25 ) ) cout << "didn't find 25\n";
    if( !vect.contains( t50 ) ) cout << "didn't find 50\n";
    if( !vect.contains( tn1 ) ) cout << "didn't find -1\n";
    if( !vect.contains( t1234567 ) ) cout << "didn't find 1234567\n";
    if( vect.contains( t13 ) ) cout << "did find 13\n";
    if( vect.find( t25, a ) ) cout << "Found " << a << " (OK)\n";
    if( vect.find( t13, a ) ) cout << "did find 13\n";

    if( vect.first() != inserted_by_last ) cout << "\"\" not first\n";
    if( vect.last() != t50 ) cout << "50 not last\n";

    if( vect.index( t0 ) != 2 ) cout << "0 not found a index 2\n";
    if( vect.index( t542 ) != -1 ) cout << "found 542 index\n";

    if( vect.occurrencesOf( t50 ) != 1 ) cout << "50 occurred != 1 time\n";

    for( i = 0; i < vect.entries(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";

    vect.clear();
    for( i = 0; i < vect.entries(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";
}

void test4() {
    str_data a = str_data( "25" );
    str_data aa = str_data( "25" );
    str_data b = str_data( "50" );
    str_data bb = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    str_data f = str_data( "13" );
    str_data f2 = str_data( "542" );
    str_data g = str_data( "-1" );
    str_data h = str_data( "34143" );
    str_data i = str_data( "-1" );
    str_data find_var;
    WCPtrSortedVector<str_data> 	vect( 5 );

    if( !vect.isEmpty() ) cout << "new list is not empty\n";
    if( vect.find( &a ) ) cout << "Found in empty ptrordered list\n";
    if( vect.entries() != 0 )cout << "PtrSorted >0 entries after init\n" ;
    if( vect.contains( &a ) ) cout << "Found 25 in empty list\n";
    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );
    vect.insert( &g );
    vect.insert( &h );
    vect.insert( &i );

    if( vect.entries() != 8 )cout << "PtrSorted != 8 entries after insert\n" ;
    if( !vect.contains( &aa ) ) cout << "didn't find 25\n";
    if( !vect.contains( &bb ) ) cout << "didn't find 50\n";
    if( !vect.contains( &d ) ) cout << "didn't find 1234\n";
    find_var = *vect.find( &bb );
    cout << "Found " << find_var << " (OK)\n";
    if( vect.find( &f ) ) cout << "did find 13\n";
    if( vect.contains( &f ) ) cout << "did find 13\n";

    if( vect.first() != &g ) cout << "-1 not first\n";
    if( vect.last() != &b ) cout << "50 not last\n";

    if( vect.index( &c ) != 2 ) cout << "0 not found a index 1\n";
    if( vect.index( &f2 ) != -1 ) cout << "found 542 index\n";

    if( vect.isEmpty() ) cout << "non-empty ptrsort list is empty\n";

    if( vect.occurrencesOf( &i ) != 2 ) cout << "-1 occurred != 2 time\n";
    if( vect.occurrencesOf( &c ) != 2 ) cout << "0 occurred != 2 time\n";


    for( int k = 0; k < vect.entries(); k++ ){
	cout << *vect[ k ] << ' ';
    }
    cout << "\n";

    vect.clear();
    for( k = 0; k < vect.entries(); k++ ){
	cout << *vect[ k ] << ' ';
    }
    cout << "\n";
    if( !vect.isEmpty() ) cout << "cleared list is not empty\n";
}
