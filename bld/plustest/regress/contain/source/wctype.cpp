#include <wclistit.h>
#include <wclist.h>
#include <wcstack.h>
#include <wcqueue.h>
#include <string.hpp>
#include <iostream.h>

#include "strdata.h"

static void scout( unsigned );

#pragma warning 549 9


int main() {
    WCQueue<str_data,WCValSList<str_data> >   queue1;
    WCQueue<str_sdata,WCValSList<str_sdata> > queue2;
    WCQueue<int,WCValSList<int> >             queue3;

    WCStack<str_data,WCValSList<str_data> >   stack1;
    WCStack<str_sdata,WCValSList<str_sdata> > stack2;
    WCStack<int,WCValSList<int> >             stack3;

    WCPtrSList<String>      list1;
    WCPtrDList<String>      list2;
    WCPtrSList<String *>    list3;
    WCPtrDList<String *>    list4;

    WCValSList<String>      list5;
    WCValDList<String>      list6;
    WCValSList<String *>    list7;
    WCValDList<String *>    list8;

    WCIsvSList<str_sdata>   list11;
    WCIsvDList<str_ddata>   list12;
    WCIsvSList<str_sdata>   list13;
    WCIsvDList<str_ddata>   list14;

    WCPtrSListIter<String>      blist1( list1 );
    WCPtrDListIter<String>      blist2( list2 );
    WCPtrSListIter<String *>    blist3( list3 );
    WCPtrDListIter<String *>    blist4( list4 );

    WCValSListIter<String>      blist5( list5 );
    WCValDListIter<String>      blist6( list6 );
    WCValSListIter<String *>    blist7( list7 );
    WCValDListIter<String *>    blist8( list8 );

    WCIsvSListIter<str_sdata>   blist11( list11 );
    WCIsvDListIter<str_ddata>   blist12( list12 );
    WCIsvSListIter<str_sdata>   blist13( list13 );
    WCIsvDListIter<str_ddata>   blist14( list14 );

    WCPtrSListIter<String>      iter1;
    WCPtrDListIter<String>      iter2;
    WCPtrSListIter<String *>    iter3;
    WCPtrDListIter<String *>    iter4;

    WCValSListIter<String>      iter5;
    WCValDListIter<String>      iter6;
    WCValSListIter<String *>    iter7;
    WCValDListIter<String *>    iter8;

    WCIsvSListIter<str_sdata>   iter11;
    WCIsvDListIter<str_ddata>   iter12;
    WCIsvSListIter<str_sdata>   iter13;
    WCIsvDListIter<str_ddata>   iter14;

#ifdef __386__
    WCPtrSListIter<String>      biter1( iter1 );
    WCPtrDListIter<String>      biter2( iter2 );
    WCPtrSListIter<String *>    biter3( iter3 );
    WCPtrDListIter<String *>    biter4( iter4 );

    WCValSListIter<String>      biter5( iter5 );
    WCValDListIter<String>      biter6( iter6 );
    WCValSListIter<String *>    biter7( iter7 );
    WCValDListIter<String *>    biter8( iter8 );

    WCIsvSListIter<str_sdata>   biter11( iter11 );
    WCIsvDListIter<str_ddata>   biter12( iter12 );
    WCIsvSListIter<str_sdata>   biter13( iter13 );
    WCIsvDListIter<str_ddata>   biter14( iter14 );
#else
    int biter1, biter2, biter3, biter4;
    int biter5, biter6, biter7, biter8;
    int biter11, biter12, biter13, biter14;
#endif

    scout( sizeof( queue1 ) );
    scout( sizeof( queue2 ) );
    scout( sizeof( queue3 ) );

    scout( sizeof( stack1 ) );
    scout( sizeof( stack2 ) );
    scout( sizeof( stack3 ) );

    scout( sizeof( list1 ) );
    scout( sizeof( list2 ) );
    scout( sizeof( list3 ) );
    scout( sizeof( list4 ) );

    scout( sizeof( list5 ) );
    scout( sizeof( list6 ) );
    scout( sizeof( list7 ) );
    scout( sizeof( list8 ) );

    scout( sizeof( list11 ) );
    scout( sizeof( list12 ) );
    scout( sizeof( list13 ) );
    scout( sizeof( list14 ) );

    scout( sizeof( blist1 ) );
    scout( sizeof( blist2 ) );
    scout( sizeof( blist3 ) );
    scout( sizeof( blist4 ) );

    scout( sizeof( blist5 ) );
    scout( sizeof( blist6 ) );
    scout( sizeof( blist7 ) );
    scout( sizeof( blist8 ) );

    scout( sizeof( blist11 ) );
    scout( sizeof( blist12 ) );
    scout( sizeof( blist13 ) );
    scout( sizeof( blist14 ) );

    scout( sizeof( iter1 ) );
    scout( sizeof( iter2 ) );
    scout( sizeof( iter3 ) );
    scout( sizeof( iter4 ) );

    scout( sizeof( iter5 ) );
    scout( sizeof( iter6 ) );
    scout( sizeof( iter7 ) );
    scout( sizeof( iter8 ) );

    scout( sizeof( iter11 ) );
    scout( sizeof( iter12 ) );
    scout( sizeof( iter13 ) );
    scout( sizeof( iter14 ) );

    scout( sizeof( biter1 ) );
    scout( sizeof( biter2 ) );
    scout( sizeof( biter3 ) );
    scout( sizeof( biter4 ) );

    scout( sizeof( biter5 ) );
    scout( sizeof( biter6 ) );
    scout( sizeof( biter7 ) );
    scout( sizeof( biter8 ) );

    scout( sizeof( biter11 ) );
    scout( sizeof( biter12 ) );
    scout( sizeof( biter13 ) );
    scout( sizeof( biter14 ) );

    cout.flush();
    return 0;
}



static void scout( unsigned class_size ) {
    cout << "[" << (class_size!=0) << "]" << endl;
}
