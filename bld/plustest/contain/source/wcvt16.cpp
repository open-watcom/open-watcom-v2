#include <iostream.h>
#include <wcvector.h>

// test WCValVector
class testVV {
    int i;
public:
// required for test
    testVV( int data ) : i( data ) {};
    int data() { return( i ); };
// required by WCValVector
    testVV() : i( 0 ) {}
    testVV( const testVV &o ) { i = o.i; };
    testVV & operator=( const testVV &o ) { i = o.i; return *this; };
};

// test WCPtrVector
class testPV {
private:
    int i;
    testPV();
    testPV( const testPV &o );
    testPV & operator=( const testPV &o );
public:
// required for test
    testPV( int data ) : i( data ) {};
    int data() { return( i ); };
};

// test WCValOrderedVector
class testOVV {
    int i;
public:
// required for test
    testOVV( int data ) : i( data ) {};
    int data() { return( i ); };
// required by WCValOrderedVector
    testOVV() : i( 0 ) {}
    testOVV( const testOVV &o ) { i = o.i; };
    testOVV & operator=( const testOVV &o ) { i = o.i; return *this; };
    int operator ==( const testOVV& r ) const { return( i == r.i ); };
};
    
// test WCPtrOrderedVector
class testOPV {
private:
    int i;
    testOPV();
    testOPV( const testOPV &o );
    testOPV & operator=( const testOPV &o );
public:
// required for test
    testOPV( int data ) : i( data ) {};
    int data() { return( i ); };
// required by WCPtrOrderedVector    
    int operator ==( const testOPV& r ) const { return( i == r.i ); };
};
    
// test WCValSortedVector
class testSVV {
    int i;
public:
// required for test
    testSVV( int data ) : i( data ) {};
    int data() { return( i ); };
// required by WCValOrderedVector
    testSVV() : i( 0 ) {}
    testSVV( const testSVV &o ) { i = o.i; };
    testSVV & operator=( const testSVV &o ) { i = o.i; return *this; };
    int operator ==( const testSVV& r ) const { return( i == r.i ); };
    int operator <( const testSVV& r ) const { return( i < r.i ); };
};
    
// test WCPtrSortedVector
class testSPV {
private:
    int i;
    testSPV();
    testSPV( const testSPV &o );
    testSPV & operator=( const testSPV &o );
public:
// required for test
    testSPV( int data ) : i( data ) {};
    int data() { return( i ); };
// required by WCPtrOrderedVector    
    int operator ==( const testSPV& r ) const { return( i == r.i ); };
    int operator <( const testSPV& r ) const { return( i < r.i ); };
};
    
    
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
    cout.flush();
    return 0;
}

void test1() {
    WCValVector<testVV> vect;
    testVV a( 0 );
    testVV b( 1 );
    testVV c( 2 );

    vect[ 0 ] = a;
    vect[ 1 ] = b;
    vect[ 2 ] = c;

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ].data() << " ";
    }
    cout << "\n";
}

void test2() {
    WCPtrVector<testPV> vect;
    testPV a( 0 );
    testPV b( 1 );
    testPV c( 2 );

    vect[ 0 ] = &a;
    vect[ 1 ] = &b;
    vect[ 2 ] = &c;

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ]->data() << " ";
    }
    cout << "\n";
}

void test3() {
    WCValOrderedVector<testOVV> vect;
    testOVV a( 0 );
    testOVV b( 1 );
    testOVV c( 2 );

    vect.insert( a );
    vect.insert( b );
    vect.insert( c );

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ].data() << " ";
    }
    cout << "\n";
}

void test4() {
    WCPtrOrderedVector<testOPV> vect;
    testOPV a( 0 );
    testOPV b( 1 );
    testOPV c( 2 );

    vect.insert( &a );
    vect.insert( &b );
    vect.insert( &c );

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ]->data() << " ";
    }
    cout << "\n";
}

void test5() {
    WCValSortedVector<testSVV> vect;
    testSVV c( 2 );
    testSVV a( 0 );
    testSVV b( 1 );

    vect.insert( a );
    vect.insert( b );
    vect.insert( c );

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ].data() << " ";
    }
    cout << "\n";
}

void test6() {
    WCPtrSortedVector<testSPV> vect;
    testSPV b( 1 );
    testSPV a( 0 );
    testSPV c( 2 );

    vect.insert( &a );
    vect.insert( &b );
    vect.insert( &c );

    for( int i = 0; i < 3; i++ ) {
	cout << vect[ i ]->data() << " ";
    }
    cout << "\n";
}
