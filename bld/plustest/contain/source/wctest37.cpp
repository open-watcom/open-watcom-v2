#include <wclist.h>
#include <wclistit.h>
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

    WCIsvDListIter<int_ddata> liter(list);

    list.append( &data1 );
    ++liter;
    liter.append( &data2 );

    list.forAll( data_isv_prt, "" );
    list.clear();
    cout.flush();
}
