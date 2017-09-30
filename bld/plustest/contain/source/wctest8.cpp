#include <wclist.h>
#include <iostream.h>

static void test1();
static void test2();

int main() {
    test1();
    test2();
    return( 0 );
}


void test1() {
    WCValSList<int>     list;
    WCValSList<int>     list2;

    list.insert( 2 );
    list.insert( 1 );
    list.insert( 5 );
    list2 = list;
    int data;
    while( !list.isEmpty() ) {
        data = list.get();
        cout << data << "\n";
    };
    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << data << "\n";
    };
    cout.flush();
}


void test2() {
    WCValDList<int>     list;
    WCValDList<int>     list2;

    list.insert( 2 );
    list.insert( 1 );
    list.insert( 5 );
    list2 = list;
    int data;
    while( !list.isEmpty() ) {
        data = list.get();
        cout << data << "\n";
    };
    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << data << "\n";
    };
    cout.flush();
}
