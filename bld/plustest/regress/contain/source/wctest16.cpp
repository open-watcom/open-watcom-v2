#include <wclist.h>
#include <iostream.h>
#include <string.hpp>


int main() {
    WCPtrSList<String>  list2;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    WCPtrSList<String> * list = new WCPtrSList<String>();
    list->insert( &d1 );
    list->insert( &d2 );
    list->insert( &d3 );
    list2 = *list;
    String * data;
    while( !list->isEmpty() ) {
        data = list->get();
        cout << *data << "\n";
    };
    cout << "\n";
    cout.flush();

    delete list;

    WCPtrSList<String> list3 = list2;

    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << *data << "\n";
    };
    cout << "\n";
    cout.flush();

    while( !list3.isEmpty() ) {
        data = list3.get();
        cout << *data << "\n";
    };
    cout.flush();
    return 0;
}
