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
    WCQueue<str_sdata *,WCIsvSList<str_sdata> >    list1;

    list1.insert( new str_sdata("1:item 1") );
    list1.insert( new str_sdata("1:data 2") );
    list1.insert( new str_sdata("1:append data") );

    str_sdata * data;
    cout << "Number of queue 1 entries: " << list1.entries() << "\n";
    while( !list1.isEmpty() ) {
        data = list1.get();
        cout << *data << "\n";
        delete data;
    };
}



void test2() {
    WCStack<str_ddata *,WCIsvDList<str_ddata> >   list1;
    
    list1.push( new str_ddata("1:item 1") );
    list1.push( new str_ddata("1:data 2") );
    list1.push( new str_ddata("1:append data") );

    str_ddata * data;
    cout << "Number of stack 1 entries: " << list1.entries() << "\n";
    while( !list1.isEmpty() ) {
        data = list1.pop();
        cout << *data << "\n";
        delete data;
    };
}
