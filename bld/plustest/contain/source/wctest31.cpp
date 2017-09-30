#include <wclist.h>
#include <iostream.h>

static void test1( void );

void data_val_prt( int data, void * str ) {
    cout << (char *)str << "[" << data << "]\n";
}


int main() {
    try {
        test1();
    } catch( ... ) {
        cout << "we caught an unexpected exception\n";
    }
    cout.flush();
    return 0;
}


void test1 ( void ) {
    WCValDList<int>         list;

    list.append( 2 );
    list.append( 3 );
    list.append( 4 );

    list.insert( 1 );
    list.append( 5 );
    cout << "<value double list for int>\n";
    list.forAll( data_val_prt, "" );
    data_val_prt( list.find( 3 ), "<the fourth element>" );
    data_val_prt( list.get( 2 ), "<the third element>" );
    data_val_prt( list.get(), "<the first element>" );
    list.clear();
    cout.flush();
}
