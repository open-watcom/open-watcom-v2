#include <wcvector.h>
#include <iostream.h>
#include "strdata.h"

static void test1();
static void test2();
static void test3();
static void test4();
static void test5();
static void test6();

int main(){
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    cout.flush();
    return 0;
}

void test1() {
    int i;
    WCValVector<long> 	vect( 5 );

    vect[ 2 ] = 25;
    vect[ 0 ] = 50;
    vect[ 1 ] = vect[ 3 ] = 0;
    vect[ 4 ] = 1234567;

    WCValVector<long> 	vect2( vect );
    vect[ 0 ] = -1;
    vect[ 6 ] = 10;

    vect = vect2;

    if( vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( vect == vect ) ) cout << "Vectors are equivalent!!\n";

    vect[ 5 ] = 0;
    
    const WCValVector<long> 	vect3( vect );
    if( vect3[ -1 ] != 50 ) cout << "vect3[ -1 ] failed\n";
    if( vect3[ 2 ] != 25 ) cout << "vect3[ 2 ] failed\n";
    if( vect3[ 6 ] != 0 ) cout << "vect3[ 6 ] failed\n";

    vect = vect;
    for( i = 0; i < vect.length(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";
}


void test2() {
    int i;

    long	a = 25;
    long 	b = 50;
    long	c = 0;
    long 	d = 1234;
    long	e = -1;
    WCPtrVector<long> 	vect( 5 );

    vect[ 2 ] = &a;
    vect[ 0 ] = &b;
    vect[ 1 ] = vect[ 3 ] = &c;
    vect[ 4 ] = &d;

    WCPtrVector<long> 	vect2( vect );
    vect[ 0 ] = &e;
    vect[ 6 ] = &e;

    vect = vect2;

    if( vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( vect == vect ) ) cout << "Vectors are equivalent!!\n";

    vect[ 5 ] = &c;

    const WCPtrVector<long> 	vect3( vect );
    if( vect3[ -1 ] != &b ) cout << "vect3[ -1 ] failed\n";
    if( vect3[ 2 ] != &a ) cout << "vect3[ 2 ] failed\n";
    if( vect3[ 6 ] != &c ) cout << "vect3[ 6 ] failed\n";

    vect = vect;
    for( i = 0; i < vect.length(); i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
}


void test3() {
    str_data t25 	= str_data( "25" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t1234567 	= str_data( "1234567" );
    WCValOrderedVector<str_data> * 	vect
    				= new WCValOrderedVector<str_data>( 2 );

    vect->insert( t25 );
    vect->insert( t0 );
    vect->insert( t50 );
    vect->insert( t0 );
    vect->insert( t1234567 );

    WCValOrderedVector<str_data> 	vect2( *vect );
    (*vect)[ 0 ] = t0;
    vect->insert( t1234567 );

    *vect = vect2;

    if( *vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( *vect == *vect ) ) cout << "Vectors are equivalent!!\n";

    vect->insert( t0 );

    const WCValOrderedVector<str_data> 	vect3( *vect );
    if( vect3[ -1 ] != t25 ) cout << "vect3[ -1 ] failed\n";
    if( vect3[ 2 ] != t50 ) cout << "vect3[ 2 ] failed\n";
    if( vect3[ 6 ] != t0 ) cout << "vect3[ 6 ] failed\n";

    vect = vect;
    for( int i = 0; i < vect->entries(); i++ ) {
	cout << (*vect)[ i ] << ' ';
    }
    cout << "\n";

    delete( vect );
}

void test4() {
    int i;

    str_data a = str_data( "25" );
    str_data b = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    WCPtrOrderedVector<str_data> 	vect( 2 );
    
    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );

    WCPtrOrderedVector<str_data> 	vect2( vect );
    vect[ 0 ] = &d;
    vect.insert( &d );

    vect = vect2;

    if( vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( vect == vect ) ) cout << "Vectors are equivalent!!\n";

    vect.insert( &c );

    vect = vect;
    for( i = 0; i < vect.entries(); i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
}


void test5() {
    str_data t25 	= str_data( "25" );
    str_data t0 	= str_data( "0" );
    str_data t50 	= str_data( "50" );
    str_data t1234567 	= str_data( "1234567" );
    str_data t34143	= str_data( "34143" );
    str_data tn1	= str_data( "-1" );
    int i;
    WCValSortedVector<str_data> 	vect( 0 );

    vect.insert( t25 );
    vect.insert( t0 );
    vect.insert( t50 );
    vect.insert( t1234567 );
    vect.insert( t0 );
    vect.insert( tn1 );
    vect.insert( t34143 );

    WCValSortedVector<str_data> 	vect2( vect );
    vect[ 0 ] = t34143;
    vect[ 5 ] = t1234567;

    vect = vect2;

    if( vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( vect == vect ) ) cout << "Vectors are equivalent!!\n";

    vect.insert( t0 );

    vect = vect;
    for( i = 0; i < vect.entries(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";
}

void test6() {
    str_data a = str_data( "25" );
    str_data b = str_data( "50" );
    str_data c = str_data( "0" );
    str_data d = str_data( "1234" );
    str_data g = str_data( "-1" );
    str_data h = str_data( "34143" );
    str_data i = str_data( "-1" );
    WCPtrSortedVector<str_data> 	vect( 5 );

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &d );
    vect.insert( &g );
    vect.insert( &h );
    vect.insert( &i );

    WCPtrSortedVector<str_data> 	vect2( vect );
    vect[ 0 ] = &h;
    vect[ 1 ] = &h;

    vect = vect2;

    if( vect == vect2 ) cout << "Vectors are not equivalent!!\n";
    if( !( vect == vect ) ) cout << "Vectors are equivalent!!\n";

    vect.insert( &c );

    vect = vect;
    for( int k = 0; k < vect.entries(); k++ ){
	cout << *vect[ k ] << ' ';
    }
    cout << "\n";
}
