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

static void print_str( String *str ){
    if( str ){
	cout << "[" << *str << "]\n";
    }else{
	cout << "[ NULL ]\n";
    }
}


void test1 ( void ) {
    WCPtrDList<String>  list;
    String              &d1= *new String("str#1");
    String              &d2= *new String("str#2");
    String              &d3= *new String("str#3");



    list.append( &d1 );
    list.append( &d2 );
    list.append( &d3 );
    WCPtrDListIter<String>  iter( list );
    iter += 3;
    print_str( iter.current() );
    iter();
//    print_str( iter.current() );
    iter();
//    print_str( iter.current() );
    --iter;
    print_str( iter.current() );
    String *data;
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
    list.clearAndDestroy();
    cout.flush();
}
