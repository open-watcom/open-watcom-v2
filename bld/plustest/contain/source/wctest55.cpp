#include <iostream.h>
#include <wclist.h>
#include <wclistit.h>

class testIsvList : public WCDLink {
private:
    testIsvList( const testIsvList & );
    testIsvList &operator=( const testIsvList & );
    int i;
public:
// used by this test
    testIsvList( int data ) : i( data ) {};
    int data() { return( i ); };
};

class testValList {
private:
    int i;
    testValList & operator=( const testValList & );
public:
// required by WCValS/DList
    testValList() : i( 0 ) {};
    testValList( const testValList &l ) { i = l.i; };
    int operator==( const testValList &r ) const {
	return( i == r.i );
    };
// used by this test
    testValList( int data ) : i( data ) {};
    int data() { return( i ); };
};

class testPtrList {
private:
    int i;
    testPtrList( const testPtrList &l );
    testPtrList();
    testPtrList & operator=( const testPtrList & );
public:
// required by WCPtrS/DList
    int operator==( const testPtrList &r ) const {
	return( i == r.i );
    };
// used by this test
    testPtrList( int data ) : i( data ) {};
    int data() { return( i ); };
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
    return 0;
}

void test1() {
    WCIsvSList<testIsvList> list;
    testIsvList i( 0 );
    testIsvList j( 1 );
    testIsvList k( 2 );

    list.insert( &i );
    list.insert( &j );
    list.insert( &k );

    WCIsvSListIter<testIsvList> iter( list );
    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    cout.flush();
};


void test2() {
    WCIsvDList<testIsvList> list;
    testIsvList i( 0 );
    testIsvList j( 1 );
    testIsvList k( 2 );

    list.insert( &i );
    list.insert( &j );
    list.insert( &k );

    WCIsvDListIter<testIsvList> iter( list );
    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    cout.flush();
};

void test3() {
    WCValSList<testValList> list;
    testValList i( 0 );
    testValList j( 1 );
    testValList k( 2 );

    list.insert( i );
    list.insert( j );
    list.insert( k );

    WCValSListIter<testValList> iter( list );
    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
    cout.flush();
};

void test4() {
    WCValDList<testValList> list;
    testValList i( 0 );
    testValList j( 1 );
    testValList k( 2 );

    list.insert( i );
    list.insert( j );
    list.insert( k );

    WCValDListIter<testValList> iter( list );
    while( ++iter ) {
	cout << iter.current().data() << " ";
    }
    cout << "\n";
    cout.flush();
};

void test5() {
    WCPtrSList<testPtrList> list;
    testPtrList i( 0 );
    testPtrList j( 1 );
    testPtrList k( 2 );

    list.insert( &i );
    list.insert( &j );
    list.insert( &k );

    WCPtrSListIter<testPtrList> iter( list );
    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    cout.flush();
};

void test6() {
    WCPtrDList<testPtrList> list;
    testPtrList i( 0 );
    testPtrList j( 1 );
    testPtrList k( 2 );

    list.insert( &i );
    list.insert( &j );
    list.insert( &k );

    WCPtrDListIter<testPtrList> iter( list );
    while( ++iter ) {
	cout << iter.current()->data() << " ";
    }
    cout << "\n";
    cout.flush();
};
