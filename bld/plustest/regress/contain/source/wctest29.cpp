#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>
#include <strdata.h>

static void test1( void );
static void test2( void );
static void test3( void );


void data_val_prt( String data, void * str ) {
    cout << ">" << (char *)str << "< [" << data << "]\n";
    cout.flush();
}

void data_ptr_prt( String * data, void * str ) {
    cout << ">" << (char *)str << "< [" << *data << "]\n";
    cout.flush();
}

void data_str_ddata_prt( str_ddata * data, void * str ) {
    cout << ">" << (char *)str << "< [" << *data << "]\n";
    cout.flush();
}

int main() {
    try {
        test1();
        test2();
        test3();
    } catch( ... ) {
        cout << "we got caught\n";
    }
    cout.flush();
    return 0;
}


void test1 ( void ) {
    WCValDList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");
    String              d4("str#4");
    String              d5("str#5");

    list.append( d1 );
    list.append( d2 );
    list.append( d3 );
    WCValDListIter<String>  iter( list );
    iter += 2;
    cout << "[" << iter.current() << "]\n";
    iter.append( d4 );
    iter.insert( d5 );
    list.forAll( data_val_prt, "valdlist<String>" );
    cout.flush();
}


void test2 ( void ) {
    WCPtrDList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");
    String              d4("str#4");
    String              d5("str#5");

    list.append( &d1 );
    list.append( &d2 );
    list.append( &d3 );
    WCPtrDListIter<String>  iter( list );
    iter += 2;
    cout << "[" << *iter.current() << "]\n";
    iter.append( &d4 );
    iter.insert( &d5 );
    list.forAll( data_ptr_prt, "ptrdlist<String>" );
    cout.flush();
}


void test3 ( void ) {
    WCIsvDList<str_ddata>  list;
    str_ddata              d1("str#1");
    str_ddata              d2("str#2");
    str_ddata              d3("str#3");
    str_ddata              d4("str#4");
    str_ddata              d5("str#5");

    list.append( &d1 );
    list.append( &d2 );
    list.append( &d3 );
    WCIsvDListIter<str_ddata>  iter( list );
    iter += 2;
    cout << "[" << *iter.current() << "]\n";
    iter.append( &d4 );
    iter.insert( &d5 );
    list.forAll( data_str_ddata_prt, "isvdlist<str_ddata>" );
    cout.flush();
}
