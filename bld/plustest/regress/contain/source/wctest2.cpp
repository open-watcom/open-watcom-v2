#include <wcqueue.h>
#include <wcstack.h>
#include <iostream.h>
#include <strdata.h>


static void test1();
static void test2();



int main() {
    test1();
    cout.flush();
    cout << "\n";
    test2();
    cout.flush();

    return( 0 );
}



void test1() {
    WCQueue<str_data,WCValSList<str_data> >   list1;
    WCQueue<int,WCValSList<int> >             list2;

    list1.insert( str_data("1:item 1") );
    list1.insert( str_data("1:data 2") );
    list1.insert( str_data("1:append data") );
    list2.insert( 7 );
    list2.insert( 8 );
    list2.insert( 9 );
    list2.insert( 10 );

    str_data data;
    cout << "Number of queue 1 entries: " << list1.entries() << "\n";
    data = list1.first();
    cout << "First entry = [" << data << "]\n";
    data = list1.last();
    cout << "Last entry = [" << data << "]\n";
    while( !list1.isEmpty() ) {
        data = list1.get();
        cout << data << "\n";
    };
    int idata;
    cout << "\nNumber of queue 2 entries: " << list2.entries() << "\n";
    idata = list2.first();
    cout << "First entry = [" << idata << "]\n";
    idata = list2.last();
    cout << "Last entry = [" << idata << "]\n";
    while( !list2.isEmpty() ) {
        idata = list2.get();
        cout << idata << "\n";
    };
}



void test2() {
    WCStack<str_data,WCValSList<str_data> >    list1;
    WCStack<int,WCValSList<int> >              list2;

    list1.push( str_data("1:item 1") );
    list1.push( str_data("1:data 2") );
    list1.push( str_data("1:append data") );
    list2.push( 7 );
    list2.push( 8 );
    list2.push( 9 );
    list2.push( 10 );

    str_data data;
    cout << "Number of stack 1 entries: " << list1.entries() << "\n";
    data = list1.top();
    cout << "Top entry = [" << data << "]\n";
    while( !list1.isEmpty() ) {
        data = list1.pop();
        cout << data << "\n";
    };
    int idata;
    cout << "\nNumber of stack 2 entries: " << list2.entries() << "\n";
    idata = list2.top();
    cout << "Top entry = [" << idata << "]\n";
    while( !list2.isEmpty() ) {
        idata = list2.pop();
        cout << idata << "\n";
    };
}
