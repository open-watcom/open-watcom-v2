#include <wchash.h>
#include <iostream.h>
#include "strdata.h"

#define test_except( code, except, string )			\
    try{							\
	code;							\
	cout << "Did not throw except\n";			\
    } catch( WCExcept::failure oops ) {			\
        WCExcept::wc_state cause = oops.cause();	\
        if( cause & WCExcept::except ) {			\
            cout << string << " (supposed to happen)\n";	\
        } else {						\
	    cout << "wrong except\n";				\
	}							\
    }

int main() {
    WCValHashSet<str_data> hash( str_data::hash_fn, 0 );
    hash.exceptions( WCExcept::check_all );
    str_data s = "hello";

    if( hash.buckets() == 0 ) cout << "Buckets should not be 0!!\n";
    hash.insert( s );
    test_except( hash.insert( s ), not_unique, "1" );
    test_except( hash.resize( 0 ), zero_buckets, "2" );
    hash.clear();
    return 0;
}
