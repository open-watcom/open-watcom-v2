#include <wclist.h>
#include <iostream.h>

static void test1( void );

void data_ptr_prt( int * data, void * str ) {
    cout << (char *)str << "[" << *data << "]\n";
}

void main() {
    try {
        test1();
    } catch( ... ) {
        cout << "we caught an unexpected exception\n";
    }
    cout.flush();
}

void test1 ( void ) {
    WCPtrDList<int>         list;
    int                     data1(1);
    int                     data2(2);
    int                     data3(3);
    int                     data4(4);
    int                     data5(5);

    list.append( &data2 );
    list.append( &data3 );
    list.append( &data4 );

    list.insert( &data1 );
    list.append( &data5 );
    cout << "<pointer double list for int>\n";
    list.forAll( data_ptr_prt, "" );
    data_ptr_prt( list.find( 3 ), "<the fourth element>" );
    data_ptr_prt( list.get( 2 ), "<the third element>" );
    data_ptr_prt( list.get(), "<the first element>" );
    list.clear();
    cout.flush();
}
