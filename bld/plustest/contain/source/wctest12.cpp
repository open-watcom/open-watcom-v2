#include <wclist.h>
#include <iostream.h>


int main() {
    WCPtrDList<int>     list2;
    int                 d1(7);
    int                 d2(2);
    int                 d3(9);

    WCPtrDList<int> * list = new WCPtrDList<int>();
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

    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << *data << "\n";
    };
    cout.flush();
    return 0;
}
