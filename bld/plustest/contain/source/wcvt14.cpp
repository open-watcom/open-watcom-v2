#include <wcvector.h>
#include <iostream.h>
#include <strdata.h>
#include "testexpt.h"

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main( void ) {
    test_except( test1(), not_empty, "test1" );
    test_except( test2(), not_empty, "test2" );
    test_except( test3(), not_empty, "test3" );
    test_except( test4(), not_empty, "test4" );
    test_except( test5(), not_empty, "test5" );
    test_except( test6(), not_empty, "test6" );
    cout.flush();
    return 0;
}

void test1() {
    WCValVector<int> vect;

    vect.exceptions( WCExcept::not_empty );
    vect[ 0 ] = 0;
};
    
void test2() {
    WCPtrVector<int> vect( 20 );
    int i = 0;

    vect.exceptions( WCExcept::not_empty );
    vect[ 0 ] = &i;
};
    
void test3() {
    WCValOrderedVector<int> vect( 20 );
    int i = 0;

    vect.exceptions( WCExcept::not_empty );
    vect.insert( i );
};
    
void test4() {
    WCPtrOrderedVector<int> vect( 20 );
    int i = 0;

    vect.exceptions( WCExcept::not_empty );
    vect.insert( &i );
};
    
void test5() {
    WCValSortedVector<int> vect( 20 );
    int i = 0;

    vect.exceptions( WCExcept::not_empty );
    vect.insert( i );
};
    
void test6() {
    WCPtrSortedVector<int> vect( 20 );
    int i = 0;

    vect.exceptions( WCExcept::not_empty );
    vect.insert( &i );
};
    
