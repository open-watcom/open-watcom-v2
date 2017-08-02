#include <iostream.h>
#include <wchash.h>
#include <wchiter.h>


// WCValHashTable
class testVHT {
private:
    int i;
public:
    testVHT( int data ) : i( data ) {};
    int data() { return( i ); };
    testVHT() : i( 0 ) {};
    testVHT( const testVHT & o ) { i = o.i; };
    testVHT &operator = ( const testVHT & o ) { i = o.i; return *this; };
    int operator== ( const testVHT & r )const { return( i == r.i ); };

    static unsigned hash_fn( const testVHT &object ) { return( object.i ); }
};

// WCPtrHashTable
class testPHT {
private:
    int i;
    testPHT() : i( 0 ) {};
    testPHT( const testPHT & o ) { i = o.i; };
    testPHT &operator = ( const testPHT & o ) { i = o.i; return *this; };
public:
    testPHT( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPHT & r )const { return( i == r.i ); };

    static unsigned hash_fn( const testPHT &object ) { return( object.i ); }
};

// WCValHashSet
typedef testVHT testVHS;
// WCPtrHashSet
typedef testPHT testPHS;

// WCValHashDict Key
typedef testVHS testVHDK;

// WCValHashDict Value test
class testVHDV {
public:
    int i;
    testVHDV( int data ) : i( data ) {};
    int data() { return( i ); };

    testVHDV() : i( 0 ) {};
    testVHDV( const testVHDV & o ) { i = o.i; };
    testVHDV &operator = ( const testVHDV & o ) { i = o.i; return *this; };
};

// WCPtrHashDict Key
typedef testPHS testPHDK;

// WCPtrHashDict Value
class testPHDV {
private:
    int i;
    testPHDV() : i( 0 ) {};
    testPHDV( const testPHDV & o ) { i = o.i; };
    testPHDV &operator = ( const testPHDV & o ) { i = o.i; return *this; };
public:
    testPHDV( int data ) : i( data ) {};
    int data() { return( i ); };
};

    

const int AS = 10;	// array size
#define array_objects { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

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
    WCValHashTable<testVHT> hash( testVHT::hash_fn, 5 );

    testVHT array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( array[ i ] );
    };

    WCValHashTableIter<testVHT> iter( hash );

    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
};

void test2() {
    WCPtrHashTable<testPHT> hash( testPHT::hash_fn, 5 );

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( new testPHT( array[ i ] ) );
    };

    WCPtrHashTableIter<testPHT> iter( hash );

    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";

    hash.clearAndDestroy();
};

void test3() {
    WCValHashSet<testVHS> hash( testVHS::hash_fn, 5 );

    testVHS array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( array[ i ] );
    };

    WCValHashSetIter<testVHS> iter( hash );

    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
};

void test4() {
    WCPtrHashSet<testPHS> hash( testPHS::hash_fn, 5 );

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( new testPHS( array[ i ] ) );
    };

    WCPtrHashSetIter<testPHS> iter( hash );

    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    hash.clearAndDestroy();
};

void test5() {
    WCValHashDict<testVHDK,testVHDV> hash( testVHDK::hash_fn, 5 );

    testVHDK array[ AS ] = array_objects;
    testVHDV arrayv[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( array[ i ], arrayv[ i ] );
    };

    WCValHashDictIter<testVHDK,testVHDV> iter( hash );

    while( ++iter ) {
	cout << iter.key().data() << ":" << iter.value().data() << " ";
    }
    cout << "\n";
};

void test6() {
    WCPtrHashDict<testPHDK, testPHDV> hash( testPHDK::hash_fn, 5 );

    int array[ AS ] = array_objects;

    for( int i = 0; i < AS; i++ ) {
	hash.insert( new testPHDK( array[ i ] ), new testPHDV( array[ i ] ) );
    };

    WCPtrHashDictIter<testPHDK, testPHDV> iter( hash );

    while( ++iter ) {
	cout << iter.key()->data() << ":" << iter.value()->data() << " ";
    }
    cout << "\n";
    hash.clearAndDestroy();
};
