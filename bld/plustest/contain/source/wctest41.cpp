#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>

class str_ddata : public WCDLink {
public:
    String info;

    inline ~str_ddata() {};
    inline str_ddata() {};
    inline str_ddata( String datum ) : info( datum ) {};
};

static void test1( void );

static void print_node( str_ddata *node ){
    if( node ){
	cout << "[" << node->info << "]\n";
    }else{
	cout << "[ NULL ]\n";
    }
}

int main() {
    try {
        test1();
    } catch( ... ) {
        cout << "we got caught\n";
    }
    cout.flush();
    return 0;
}


void test1 ( void ) {
    WCIsvDList<str_ddata>  list;
    str_ddata           d1("str#1");
    str_ddata           d2("str#2");
    str_ddata           d3("str#3");

    list.append( &d1 );
    list.append( &d2 );
    list.append( &d3 );
    WCIsvDListIter<str_ddata>  iter( list );
    iter += 3;
    print_node( iter.current() );
    iter();
    print_node( iter.current() );
    iter();
    print_node( iter.current() );
    --iter;
    print_node( iter.current() );
    str_ddata *data_node;
    iter.reset();
    iter.exceptions( WCIterExcept::check_all );
    try {
        data_node = iter.current();
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_item ) {
            cout << "\n**(1) the current value was baaaad **\n";
        }
    }
    iter.reset();
    try {
        iter += 5;
        data_node = iter.current();
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_item ) {
            cout << "\n**(2) the current value was baaaad **\n";
        } else if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(2) the iterator position was baaaad **\n";
        }
    }
    iter.reset();
    try {
        --iter;
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(1) the iterator was baaaad **\n";
        }
    }
    iter.reset();
    try {
        iter += 4;
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(2) the iterator was baaaad **\n";
        }
    }
    iter.reset();
    try {
        iter += -1;
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::iter_range ) {
            cout << "\n**(1) the iterator value was baaaad **\n";
        }
    }
    cout.flush();
}
