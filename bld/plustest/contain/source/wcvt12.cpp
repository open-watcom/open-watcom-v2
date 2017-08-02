// date     who             what
// -------- --------------- --------------------------------------------
// 96/02/20 J.W.Welch       Changed some rvalue casts to be lvalue casts

#include <wcvector.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();


int main() {
    cout << "there should be 6 sets of 5 supposed to happen exceptions\n";
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    cout.flush();
    return 0;
};


void test1() {
    str_data a;
    str_data b( "test" );
    WCValVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCValVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCValVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect[ 0 ] = b;

    const WCValVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    not_happen_except( a = c_vect2[ 1 ];, "index n2" );
    test_except( a = c_vect2[ 2 ];, index_range, "index 5" );
    vect.clear();
    vect2.clear();
    ((WCValVector<str_data>&)c_vect).clear();
    ((WCValVector<str_data>&)c_vect2).clear();
}


void test2() {
    const str_data *a;
    str_data b( "test" );
    WCPtrVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCPtrVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCPtrVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect[ 0 ] = &b;

    const WCPtrVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    not_happen_except( a = c_vect2[ 1 ];, "index n2" );
    test_except( a = c_vect2[ 2 ];, index_range, "index 5" );
    vect.clear();
    vect2.clear();
    ((WCPtrVector<str_data>&)c_vect).clear();
    ((WCPtrVector<str_data>&)c_vect2).clear();
}


void test3() {
    str_data a;
    str_data b( "test" );
    WCValOrderedVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCValOrderedVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCValOrderedVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect.insert( b );

    const WCValOrderedVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    test_except( a = c_vect2[ 1 ];, index_range, "index 5" );
    vect.clear();
    vect2.clear();
    ((WCValOrderedVector<str_data>&)c_vect).clear();
    ((WCValOrderedVector<str_data>&)c_vect2).clear();
}


void test4() {
    const str_data *a;
    str_data b( "test" );
    WCPtrOrderedVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCPtrOrderedVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCPtrOrderedVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect.insert( &b );

    const WCPtrOrderedVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    test_except( a = c_vect2[ 1 ];, index_range, "index 5" );
    vect.clear();
    vect2.clear();
    ((WCPtrOrderedVector<str_data>&)c_vect).clear();
    ((WCPtrOrderedVector<str_data>&)c_vect2).clear();
}


void test5() {
    str_data a;
    str_data b( "test" );
    WCValSortedVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCValSortedVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCValSortedVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect.insert( b );

    const WCValSortedVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    test_except( a = c_vect2[ 1 ];, index_range, "index 5" );

    vect.clear();
    vect2.clear();
    ((WCValSortedVector<str_data>&)c_vect).clear();
    ((WCValSortedVector<str_data>&)c_vect2).clear();
}


void test6() {
    const str_data *a;
    str_data b( "test" );
    WCPtrSortedVector<str_data> vect( 2 );
    vect.exceptions( WCExcept::check_all );

    WCPtrSortedVector<str_data> vect2( 0 );
    vect2.exceptions( WCExcept::check_all );
    const WCPtrSortedVector<str_data> c_vect( vect2 );

    test_except( a = c_vect[ 0 ];, empty_container, "index 1" );
    test_except( a = c_vect[ -1 ];, empty_container, "index 2" );
    test_except( a = c_vect[ 1 ];, empty_container, "index 3" );

    vect.insert( &b );

    const WCPtrSortedVector<str_data> c_vect2( vect );

    not_happen_except( a = c_vect2[ 0 ];, "index n1" );
    test_except( a = c_vect2[ -1 ];, index_range, "index 4" );
    test_except( a = c_vect2[ 1 ];, index_range, "index 5" );

    vect.clear();
    vect2.clear();
    ((WCPtrSortedVector<str_data>&)c_vect).clear();
    ((WCPtrSortedVector<str_data>&)c_vect2).clear();
}
