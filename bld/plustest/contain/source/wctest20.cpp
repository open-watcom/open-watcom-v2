#include <wclist.h>
#include <iostream.h>
#include <string.hpp>

static void test1( void );

int main() {
    try {
        test1();
    } catch( ... ) {
        cout << "we got caught\n";
    }
    cout.flush();
    return 0;
}


void test1 ( void ) {
    WCPtrSList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    list.append( &d1 );
    list.append( &d2 );
    list.append( &d3 );
    cout << "[" << *list.find(3) << "] - find(3)\n";
    cout << "[" << *list.find(4) << "] - find(4)\n";
    cout << "[" << *list.find(-1) << "] - find(-1)\n";
    list.exceptions( WCListExcept::index_range );
    try {
        cout << "[" << *list.find(3) << "] - find(3)\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::index_range ) {
            cout << "\n** the index value of a find call was out of range **\n";
        }
    }
    try {
        cout << "[" << *list.find(4) << "] - find(4)\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::index_range ) {
            cout << "\n** the index value of a find call was out of range **\n";
        }
    }
    try {
        cout << "[" << *list.find(-1) << "] - find(-1)\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::index_range ) {
            cout << "\n** the index value of a find call was out of range **\n";
        }
    }
    cout.flush();
}
