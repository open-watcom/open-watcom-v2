#include <wclist.h>
#include <wcstack.h>
#include <wcqueue.h>
#include <wclistit.h>

class sint : public WCSLink {
    int     data;
};

class dint : public WCDLink {
    int     data;
};

void phooey( WCValSList<int> & );

void foo( void ) {
    WCPtrSList<int>                     list1;
    WCPtrDList<int>                     list2;
    WCValSList<int>                     list3;
    WCValDList<int>                     list4;
    WCIsvSList<sint>                    list5;
    WCIsvDList<dint>                    list6;
    WCPtrSListIter<int>                 iter1;
    WCPtrDListIter<int>                 iter2;
    WCValSListIter<int>                 iter3;
    WCValDListIter<int>                 iter4;
    WCIsvSListIter<sint>                iter5;
    WCIsvDListIter<dint>                iter6;
    WCQueue<int,WCValSList<int> >       queue1;
    WCStack<int,WCValSList<int> >       stack1;

    phooey( list3 );
}
