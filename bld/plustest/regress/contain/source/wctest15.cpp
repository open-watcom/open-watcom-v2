#include <wclist.h>
#include <iostream.h>


int main() {
    WCPtrSList<int>     list2;
    int                 d1(7);
    int                 d2(2);
    int                 d3(9);

    WCPtrSList<int> * list = new WCPtrSList<int>();
    list->insert( &d1 );
    list->insert( &d2 );
    list->insert( &d3 );
    list2 = *list;
    int * data;
    while( !list->isEmpty() ) {
        data = list->get();
        cout << *data << "\n";
    };
    cout << "\n";
    cout.flush();

    delete list;

    WCPtrSList<int> list3 = list2;

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
