#include <iostream.h>
#include <wcskip.h>
#include <wcskipit.h>


// WCValSkipListList
class testVSL {
private:
    int i;
public:
    testVSL( int data ) : i( data ) {};
    int data() { return( i ); };
    testVSL() : i( 0 ) {};
    testVSL( const testVSL & o ) { i = o.i; };
    int operator== ( const testVSL & r )const { return( i == r.i ); };
    int operator< ( const testVSL & r )const { return( i < r.i ); };
    testVSL &operator = ( const testVSL & o ) { i = o.i; return *this; };
};

// WCPtrSkipListTable
class testPSL {
private:
    int i;
    testPSL() : i( 0 ) {};
    testPSL( const testPSL & o ) { i = o.i; };
    testPSL &operator = ( const testPSL & o ) { i = o.i; return *this; };
public:
    testPSL( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPSL & r )const { return( i == r.i ); };
    int operator< ( const testPSL & r )const { return( i < r.i ); };
};

// WCValSkipListSet
typedef testVSL testVSLS;
// WCPtrSkipListSet
typedef testPSL testPSLS;

// WCValSkipListDict Key
typedef testVSLS testVSLDK;

// WCValSkipListDict Value test
class testVSLDV {
private:

public:
    int i;
    testVSLDV( int data ) : i( data ) {};
    int data() { return( i ); };

    testVSLDV() : i( 0 ) {};
    testVSLDV( const testVSLDV & o ) { i = o.i; };
    testVSLDV &operator = ( const testVSLDV & o ) { i = o.i; return *this; };
};

// WCPtrSkipListDict Key
typedef testPSLS testPSLDK;

// WCPtrSkipListDict Value
class testPSLDV {
private:
    int i;
    testPSLDV() : i( 0 ) {};
    testPSLDV( const testPSLDV & o ) { i = o.i; };
    testPSLDV &operator = ( const testPSLDV & o ) { i = o.i; return *this; };
public:
    testPSLDV( int data ) : i( data ) {};
    int data() { return( i ); };
};

    

const int AS = 10;	// array size
#define array_objects { 1, 9, 3, 6, 10, 2, 4, 8, 5, 7 };

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
    
int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}

void test1() {
    WCValSkipList<testVSL> skip_list;

    testVSL array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( array[ i ] );
    };

    WCValSkipListIter<testVSL> iter( skip_list );

    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
};

void test2() {
    WCPtrSkipList<testPSL> skip_list;

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( new testPSL( array[ i ] ) );
    };

    WCPtrSkipListIter<testPSL> iter( skip_list );

    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";

    skip_list.clearAndDestroy();
};

void test3() {
    WCValSkipListSet<testVSLS> skip_list;

    testVSLS array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( array[ i ] );
    };

    WCValSkipListSetIter<testVSLS> iter( skip_list );

    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
};

void test4() {
    WCPtrSkipListSet<testPSLS> skip_list;

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( new testPSLS( array[ i ] ) );
    };

    WCPtrSkipListSetIter<testPSLS> iter( skip_list );

    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    skip_list.clearAndDestroy();
};

void test5() {
    WCValSkipListDict<testVSLDK,testVSLDV> skip_list;

    testVSLDK array[ AS ] = array_objects;
    testVSLDV arrayv[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( array[ i ], arrayv[ i ] );
    };

    WCValSkipListDictIter<testVSLDK,testVSLDV> iter( skip_list );

    while( ++iter ) {
	cout << iter.key().data() << ":" << iter.value().data() << " ";
    }
    cout << "\n";
};

void test6() {
    WCPtrSkipListDict<testPSLDK, testPSLDV> skip_list;

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	skip_list.insert( new testPSLDK( array[ i ] ), new testPSLDV( array[ i ] ) );
    };

    WCPtrSkipListDictIter<testPSLDK, testPSLDV> iter( skip_list );

    while( ++iter ) {
	cout << iter.key()->data() << ":" << iter.value()->data() << " ";
    }
    cout << "\n";
    skip_list.clearAndDestroy();
};
