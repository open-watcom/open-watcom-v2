#include <wclist.h>
#include <iostream.h>



void data_val_prt( int * data, void * str ) {
    cout << ">" << (char *)str << "< [" << *data << "]\n";
    cout.flush();
}

int main() {
    WCPtrDList<int>     list;
    int                 d1(1);
    int                 d2(2);
    int                 d3(3);

    WCValDList<int *> * list2 = new WCValDList<int *>();
    list.insert( &d1 );
    list.append( &d2 );
    list2->insert( &d3 );
    cout << "1:1 list\n";
    list.forAll( data_val_prt, "ptrdlist<int *>" );
    cout << "2:1 list\n";
    list2->forAll( data_val_prt, "ptrdlist<int *>" );
    *list2 = list;
    cout << "1:2 list\n";
    list.forAll( data_val_prt, "ptrdlist<int *>" );
    cout << "2:2 list\n";
    list2->forAll( data_val_prt, "ptrdlist<int *>" );
    delete( list2 );
    return 0;
}
