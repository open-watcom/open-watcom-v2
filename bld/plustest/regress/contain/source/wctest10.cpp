#include <wclist.h>
#include <iostream.h>


int main() {
    WCPtrSList<int *>   list2;
    int                 d1(7);
    int                 d2(2);
    int                 d3(9);
    int *               p1(&d1);
    int *               p2(&d2);
    int *               p3(&d3);

    WCPtrSList<int *> * list = new WCPtrSList<int *>();
    list->insert( &p1 );
    list->insert( &p2 );
    list->insert( &p3 );
    list2 = *list;
    int * * data;
    while( !list->isEmpty() ) {
        data = list->get();
        cout << **data << "\n";
    };
    cout << "\n";
    cout.flush();

    delete list;

    while( !list2.isEmpty() ) {
        data = list2.get();
        cout << **data << "\n";
    };
    cout.flush();
    return 0;
}
