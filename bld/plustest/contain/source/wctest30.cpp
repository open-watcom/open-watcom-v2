#include <wclist.h>
#include <iostream.h>

class int_ddata : public WCDLink {
public:
    inline int_ddata() {};
    inline ~int_ddata() {};
    inline int_ddata( int datum ) : info( datum ) {};

    int     info;
};

static void test1( void );

void data_isv_prt( int_ddata * data, void * str ) {
    cout << (char *)str << "[" << data->info << "]\n";
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
    WCIsvDList<int_ddata>   list;
    int_ddata               data1(1);
    int_ddata               data2(2);
    int_ddata               data3(3);
    int_ddata               data4(4);
    int_ddata               data5(5);

    list.append( &data2 );
    list.append( &data3 );
    list.append( &data4 );

    list.insert( &data1 );
    list.append( &data5 );
    cout << "<intrusive double list for int_ddata>\n";
    list.forAll( data_isv_prt, "" );
    data_isv_prt( list.find( 3 ), "<the fourth element>" );
    data_isv_prt( list.get( 2 ), "<the third element>" );
    data_isv_prt( list.get(), "<the first element>" );
    list.clear();
    cout.flush();
}
