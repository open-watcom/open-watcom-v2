#include <wclist.h>
#include <wclistit.h>
#include <iostream.h>
#include <string.hpp>

static void test1( void );

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
    WCValDList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    list.append( d1 );
    list.append( d2 );
    list.append( d3 );
    WCValDListIter<String>  iter( list );
    iter += 3;
    cout << "[" << iter.current() << "]\n";
    iter();
    cout << "[" << iter.current() << "]\n";
    iter();
    cout << "[" << iter.current() << "]\n";
    --iter;
    cout << "[" << iter.current() << "]\n";
    String data;
    iter.reset();
    iter.exceptions( WCIterExcept::check_all );
    try {
        data = iter.current();
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_item ) {
            cout << "\n**(1) the current value was baaaad **\n";
        }
    }
    iter.reset();
    try {
        iter += 5;
        data = iter.current();
	cout << "Should not be here\n";
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_item ) {
            cout << "\n**(2) the current value was baaaad **should not happen\n";
        } else if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(2) the iterator position was baaaad **should not happen\n";
        } else if( cause & WCIterExcept::iter_range ) {
	    cout << "\n**(2) iter range bad\n";
	}
    }
    iter.reset();
    try {
        --iter;
	cout << "Should not be here 2\n";
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(1) the iterator was baaaad **\n";
        }
    }
    iter.reset();
    try {
        iter += 4;
	cout << "Should be here 3\n";
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::undef_iter ) {
            cout << "\n**(2) the iterator was baaaad ** should not happen\n";
        }
    }
    iter.reset();
    try {
        iter += -1;
	cout << "Should not be here 4\n";
    } catch( WCIterExcept::failure oops ) {
        WCIterExcept::wciter_state cause = oops.cause();
        if( cause & WCIterExcept::iter_range ) {
            cout << "\n**(1) the iterator value was baaaad **\n";
        }
    }
    cout.flush();
}
