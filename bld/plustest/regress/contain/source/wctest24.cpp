#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>
#include <strdata.h>

static void test1( void );


int main() {
    test1();
    return 0;
}



static void test1() {
    WCValSList<String>  list;
    WCValSList<String>  list2;
    String              d1("svstr#1");
    String              d2("svstr#2");
    String              d3("svstr#3");
    String              d4("svstr#4");
    String              d5("srover:1#1");
    String              d6("srover:1#2");

    list.append( d1 );
    list.append( d2 );
    list.append( d3 );
    list.append( d4 );
    list2.append( d3 );
    list2.append( d2 );
    list2.append( d1 );
    WCValSListIter<String> liter(list);
    String data;
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << data << "]\n";
    };
    cout << "\ndo a reset\n\n";
    liter.reset();
    if( liter() ) {
        cout << "[" << liter.current() << "]\n";
    }
    if( liter() ) {
        cout << "[" << liter.current() << "]\n";
    }
    cout << "\ndo a reset to list 2\n\n";
    liter.reset( list2 );
    for(;;) {
        if( !liter() ) break;
        data = liter.current();
        cout << "[" << data << "]\n";
    };
    cout << "\n";
    cout.flush();
}
