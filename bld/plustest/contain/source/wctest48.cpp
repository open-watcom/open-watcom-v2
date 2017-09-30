#include <wclist.h>
#include <iostream.h>
#include "strdata.h"

int fail_alloc = 0;

void *my_alloc( size_t size ) {
    if( fail_alloc ) {
	return( 0 );
    } else {
	return( new char[ size ] );
    }
}

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    cout << "Should see no output...\n";
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}

void test1(){
    int a;
    WCPtrSList<int> list( my_alloc, 0 );

    fail_alloc = 0;
    if( !list.insert( &a ) ) cout << "Insertion failed!\n";
    if( !list.append( &a ) ) cout << "Append failed!\n";

    fail_alloc = 1;
    if( list.insert( &a ) ) cout << "Insertion worked (shouldn't have)!\n";
    if( list.append( &a ) ) cout << "Append worked (shouldn't have)!\n";

    list.exceptions( WCListExcept::check_all );

    try {
	list.insert( &a );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    try {
	list.append( &a );
	cout << "Should not be here  2\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    list.clear();
    cout.flush();
}

void test2(){
    int a;
    WCPtrDList<int> list( my_alloc, 0 );

    fail_alloc = 0;
    if( !list.insert( &a ) ) cout << "Insertion failed!\n";
    if( !list.append( &a ) ) cout << "Append failed!\n";

    fail_alloc = 1;
    if( list.insert( &a ) ) cout << "Insertion worked (shouldn't have)!\n";
    if( list.append( &a ) ) cout << "Append worked (shouldn't have)!\n";

    list.exceptions( WCListExcept::check_all );

    try {
	list.insert( &a );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    try {
	list.append( &a );
	cout << "Should not be here  2\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    list.clear();
    cout.flush();
}

void test3(){
    int a;
    WCValSList<int> list( my_alloc, 0 );

    fail_alloc = 0;
    if( !list.insert( a ) ) cout << "Insertion failed!\n";
    if( !list.append( a ) ) cout << "Append failed!\n";

    fail_alloc = 1;
    if( list.insert( a ) ) cout << "Insertion worked (shouldn't have)!\n";
    if( list.append( a ) ) cout << "Append worked (shouldn't have)!\n";

    list.exceptions( WCListExcept::check_all );

    try {
	list.insert( a );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    try {
	list.append( a );
	cout << "Should not be here  2\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    list.clear();
    cout.flush();
}

void test4(){
    int a;
    WCValDList<int> list( my_alloc, 0 );

    fail_alloc = 0;
    if( !list.insert( a ) ) cout << "Insertion failed!\n";
    if( !list.append( a ) ) cout << "Append failed!\n";

    fail_alloc = 1;
    if( list.insert( a ) ) cout << "Insertion worked (shouldn't have)!\n";
    if( list.append( a ) ) cout << "Append worked (shouldn't have)!\n";

    list.exceptions( WCListExcept::check_all );

    try {
	list.insert( a );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    try {
	list.append( a );
	cout << "Should not be here  2\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( !( cause & WCListExcept::out_of_memory ) ) {
	    cout << "wrong except\n";
	}
    }
    list.clear();
    cout.flush();
}

void test5() {
    str_sdata a( "Hello" );
    str_sdata b( "Hello" );
    WCIsvSList< str_sdata > list;

    if( TRUE != list.insert( &a ) ) cout << "ERROR 1";
    if( TRUE != list.append( &b ) ) cout << "ERROR 2";
}

void test6() {
    str_ddata a( "Hello" );
    str_ddata b( "Hello" );
    WCIsvDList< str_ddata > list;

    if( TRUE != list.insert( &a ) ) cout << "ERROR 1";
    if( TRUE != list.append( &b ) ) cout << "ERROR 2";
}
