#include <iostream.h>
#include <wcskip.h>
#include "strdata.h"

static void *alloc_fn( size_t ){
    return( 0 );
};

int main() {
    WCPtrSkipListDict<str_data, int> skip_list( WCSKIPLIST_PROB_HALF, 10, alloc_fn, 0 );
    skip_list.exceptions( WCExcept::check_all );
    str_data temp = "hello";

    try {
	// this is supposed to be a runtime error (alloc failed on index insert)
	cout << skip_list[ &temp ];
    } catch( ... ) {
	cout << "PASS " << __FILE__ << endl;
    }
    return 0;
};
