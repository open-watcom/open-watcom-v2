#include <wcskip.h>
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
    WCValSkipListSet<str_data> skip_list;
    skip_list.exceptions( WCExcept::check_all );
    str_data s = "hello";

    skip_list.insert( s );
    test_except( skip_list.insert( s ), not_unique, "1" );
    skip_list.clear();
    return 0;
}
