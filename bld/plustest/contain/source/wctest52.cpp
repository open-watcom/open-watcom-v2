#include <wclist.h>
#include <iostream.h>
#include <strdata.h>

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    cout << "There should be no output\n";
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
    WCIsvSList<str_sdata> list;
    WCIsvSList<str_sdata> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

void test2() {
    WCIsvDList<str_ddata> list;
    WCIsvDList<str_ddata> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

void test3() {
    WCValSList<str_data> list;
    WCValSList<str_data> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

void test4() {
    WCValDList<str_data> list;
    WCValDList<str_data> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

void test5() {
    WCPtrSList<str_data> list;
    WCPtrSList<str_data> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

void test6() {
    WCPtrDList<str_data> list;
    WCPtrDList<str_data> list2;

    if( list == list2 ) cout << "Lists should not be equal\n";
    if( !(list == list) ) cout << "Lists should be equal\n";
}

