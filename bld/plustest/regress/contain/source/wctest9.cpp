#include <wclist.h>
#include <iostream.h>

static void test1();

int main() {
    test1();
    return( 0 );
}


void test1() {
    WCValSList<int>     list2;

    WCValSList<int> * list = new WCValSList<int>();
    list->insert( 2 );
    list->insert( 1 );
    list->insert( 5 );
    list2 = *list;
    delete list;
    int data;
    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << data << "\n";
    };
    cout.flush();
}
