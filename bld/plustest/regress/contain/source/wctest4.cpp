#include <wclist.h>
#include <stdlib.h>
#include <iostream.h>
#include <strdata.h>


static void test1();
static void test2();



int main() {
    test1();
    cout << "I guess no error was set\n\n";
    cout.flush();
    try {
        test2();
        cout << "I guess no error was set again\n";
        cout.flush();
    }
    catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::not_empty ) {
            cout << "\n** oh no, the list is not empty **\n";
            cout.flush();
        }
    }
    return 0;
}



void test1() {
    WCPtrSList<str_data>    slist;
    WCPtrDList<str_data>    list;
// I've elliminated any unnecessay unfreed chuncks by just having each list
// containing one element
    str_data s1("6:item 1");
//    str_data s2("6:data 2");
//    str_data s3("6:append data");
//    str_data s4("6:insert data");
//    str_data s5("6:item 1");
    str_data s6("6:data 2");
//    str_data s7("6:append data");

    slist.insert( &s1 );
//    slist.insert( &s2 );
//    slist.append( &s3 );
//    slist.insert( &s4 );
//    list.insert( &s5 );
    list.insert( &s6 );
//    list.append( &s7 );
    cout << "Finished doing a single and double linked list(1)\n";
}



void test2() {
    WCPtrSList<str_data>    slist;
    WCPtrDList<str_data>    list;
// I've elliminated any unnecessay unfreed chuncks by just having each list
// containing one element
    str_data s1("6:item 1");
//    str_data s2("6:data 2");
//    str_data s3("6:append data");
//    str_data s4("6:insert data");
//    str_data s5("6:item 1");
    str_data s6("6:data 2");
//    str_data s7("6:append data");

    slist.exceptions( WCListExcept::not_empty );
    //list.exceptions( WCListExcept::not_empty );
    slist.insert( &s1 );
//    slist.insert( &s2 );
//    slist.append( &s3 );
//    slist.insert( &s4 );
//    list.insert( &s5 );
    list.insert( &s6 );
//    list.append( &s7 );
    cout << "Finished doing a single and double linked list(2)\n";
}
