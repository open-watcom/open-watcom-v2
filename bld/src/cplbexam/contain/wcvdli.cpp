#include <wclistit.h>
#include <iostream.h>


//
// insert elem after all elements in the list less than or equal to
// elem
//

void insert_in_order( WCValDList<int> &list, int elem ) {
    if( list.entries() == 0 ) {
        // cannot insert in an empty list using a iterator
        list.insert( elem );
    } else {

        WCValDListIter<int> iter( list );
        while( ++iter ) {
            if( iter.current() > elem ) {
                // insert elem before first element in list greater
                // than elem
                iter.insert( elem );
                return;
            }
        }

        // iterated past the end of the list
        // append elem to the end of the list
        list.append( elem );
    }
}


void main() {
    WCValDList<int> list;

    insert_in_order( list, 5 );
    insert_in_order( list, 20 );
    insert_in_order( list, 1 );
    insert_in_order( list, 25 );

    cout << "List elements in ascending order:\n";

    WCValDListIter<int> iter( list );
    while( ++iter ) {
        cout << iter.current() << "\n";
    }

    cout << "List elements in descending order\n";

    // iterator is past the end of the list
    while( --iter ) {
        cout << iter.current() << "\n";
    }
}
