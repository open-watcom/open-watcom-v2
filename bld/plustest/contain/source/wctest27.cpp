#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>


int main() {
    WCValSList<int>     list;
    WCValSList<int>     list2;

    list.append( 2 );
    list.append( 3 );
    list.append( 6 );
    list.append( 7 );
    WCValSListIter<int> liter(list);
    WCValSListIter<int> liter2(list2);
    if( liter.container() == &list2 ) {
        cout << "what?? wrong\n";
    }
    if( liter.container() == &list ) {
        cout << "right\n";
    }
    if( liter2.container() == &list ) {
        cout << "what?? wrong\n";
    }
    if( liter2.container() == &list2 ) {
        cout << "right\n";
    }
    if( liter.container() == liter2.container() ) {
        cout << "what?? wrong\n";
    }
    cout.flush();
    return 0;
}
