#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>
#include <strdata.h>

static void test1( void );
static void test2( void );
static void test3( void );
static void test4( void );
static void test5( void );
static void test6( void );


int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}



static void test1() {
    WCValSList<String>  list;
    String              d1("svstr#1");
    String              d2("svstr#2");
    String              d3("svstr#3");
    String              d4("svstr#4");

    list.append( d1 );
    list.append( d2 );
    list.append( d3 );
    list.append( d4 );
    WCValSListIter< String> liter(list);
    String data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}



static void test2() {
    WCValDList<String>  list;

    list.append( String("dvstr#1") );
    list.append( String("dvstr#2") );
    list.append( String("dvstr#3") );
    list.append( String("dvstr#4") );
    WCValDListIter<String> liter(list);
    String data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}



static void test3() {
    WCPtrSList<String>  list;

    list.append( new String("spstr#1") );
    list.append( new String("spstr#2") );
    list.append( new String("spstr#3") );
    list.append( new String("spstr#4") );
    WCPtrSListIter<String> liter(list);
    String * data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << *data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}



static void test4() {
    WCPtrDList<String>  list;

    list.append( new String("dpstr#1") );
    list.append( new String("dpstr#2") );
    list.append( new String("dpstr#3") );
    list.append( new String("dpstr#4") );
    WCPtrDListIter<String> liter(list);
    String * data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << *data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}



static void test5() {
    WCIsvSList<str_sdata>  list;

    list.append( new str_sdata("distr#1") );
    list.append( new str_sdata("distr#2") );
    list.append( new str_sdata("distr#3") );
    list.append( new str_sdata("distr#4") );
    WCIsvSListIter<str_sdata> liter(list);
    str_sdata * data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << *data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}



static void test6() {
    WCIsvDList<str_ddata>  list;

    list.append( new str_ddata("distr#1") );
    list.append( new str_ddata("distr#2") );
    list.append( new str_ddata("distr#3") );
    list.append( new str_ddata("distr#4") );
    WCIsvDListIter<str_ddata> liter(list);
    str_ddata * data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << *data << "]\n";
    };
    list.clearAndDestroy();
    cout << "\n";
    cout.flush();
}
