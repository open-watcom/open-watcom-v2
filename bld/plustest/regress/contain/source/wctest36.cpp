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
    int_ddata               data3(3);

    list.append( &data1 );
    list.append( &data2 );
    list.append( &data3 );

    cout << "<intrusive double list for int_ddata>\n";
    list.forAll( data_isv_prt, "" );
    WCIsvDListIter<int_ddata> liter(list);
    int_ddata * data;
    ++liter;
    cout << "[" << liter.current()->info << "]\n";
    ++liter;
    cout << "[" << liter.current()->info << "]\n";
    ++liter;
    cout << "[" << liter.current()->info << "]\n";
    --liter;
    cout << "[" << liter.current()->info << "]\n";
    ++liter;
    ++liter;
    data = liter.current();
    if( data != NULL ) {
        cout << "say what! -- this should be null\n";
    }
    list.clear();
    cout.flush();
}
