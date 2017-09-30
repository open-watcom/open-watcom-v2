#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>


int main() {
    WCPtrSList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    list.insert( &d1 );
    list.insert( &d2 );
    list.insert( &d3 );
    WCPtrSListIter<String> liter(list);
    String * data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << *data << "]\n";
    };
    cout << "\n";
    cout.flush();
    return 0;
}
