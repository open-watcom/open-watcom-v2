#include <wclist.h>
#include <iostream.h>
#include <strdata.h>


static void test1();
static void test2();
static void test3();
static void test4();
static void test5();
static void test6();



int main() {
    test1();
    cout << "\n";
    cout.flush();
    test2();
    cout << "\n";
    cout.flush();
    test3();
    cout << "\n";
    cout.flush();
    test4();
    cout << "\n";
    cout.flush();
    test5();
    cout << "\n";
    cout.flush();
    test6();
    cout.flush();

    return( 0 );
}



void test1() {
    WCIsvSList<str_sdata>   ilist;

    ilist.insert( new str_sdata("1:item 1") );
    ilist.insert( new str_sdata("1:data 2") );
    ilist.append( new str_sdata("1:append data") );

    str_sdata * data;
    cout << "Number of entries: " << ilist.entries() << "\n";
    while( !ilist.isEmpty() ) {
        data = ilist.get();
        cout << *data << "\n";
        delete data;
    };
}



void test2() {
    WCIsvDList<str_ddata>   ilist;

    ilist.insert( new str_ddata("2:item 1") );
    ilist.insert( new str_ddata("2:data 2") );
    ilist.append( new str_ddata("2:append data") );

    str_ddata * data;
    cout << "Number of entries: " << ilist.entries() << "\n";
    while( !ilist.isEmpty() ) {
        data = ilist.get();
        cout << *data << "\n";
        delete data;
    };
}



void test3() {
    WCValSList<int>    list;

    int  i = 3;
    list.insert( i );
    i = 2;
    list.insert( i );
    i = 7;
    list.append( i );

    cout << "contains 5? " << list.contains(5) << "\n";
    cout << "contains 7? " << list.contains(7) << "\n";
    cout << "contains 2? " << list.contains(2) << "\n";

    int data;
    cout << "Number of entries: " << list.entries() << "\n";
    while( !list.isEmpty() ) {
        data = list.get();
        cout << data << "\n";
    };
}



void test4() {
    WCValSList<str_data>   list;

    list.insert( str_data("4:item 1") );
    list.insert( str_data("4:data 2") );
    list.append( str_data("4:append data") );

    str_data data;
    cout << "Number of entries: " << list.entries() << "\n";
    while( !list.isEmpty() ) {
        data = list.get();
        cout << data << "\n";
    };
}



void test5() {
    WCValDList<str_data>   list;

    list.insert( str_data("5:item 1") );
    list.insert( str_data("5:data 2") );
    list.append( str_data("5:append data") );

    str_data data;
    cout << "Number of entries: " << list.entries() << "\n";
    while( !list.isEmpty() ) {
        data = list.get();
        cout << data << "\n";
    };
}



void test6() {
    WCPtrSList<str_data>    slist;
    WCPtrDList<str_data>    list;

    slist.insert( new str_data("6:item 1") );
    slist.insert( new str_data("6:data 2") );
    slist.append( new str_data("6:append data") );
    slist.insert( new str_data("6:insert data") );
    list.insert( new str_data("6:item 1") );
    list.insert( new str_data("6:data 2") );
    list.append( new str_data("6:append data") );

    str_data * data;
    cout << "Number of entries: " << slist.entries() << "\n";
    cout << "Number of entries: " << list.entries() << "\n";
    while( !list.isEmpty() ) {
        data = slist.get();
        cout << *data << "\n";
        delete data;
        data = list.get();
        cout << *data << "\n";
        delete data;
    };
    data = slist.get();
    delete data;
}
