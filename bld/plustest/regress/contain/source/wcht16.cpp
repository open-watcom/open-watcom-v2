#include <iostream.h>
#include <wchash.h>
#include "strdata.h"

static void *alloc_fn( size_t ){
    return( 0 );
};

int main() {
    WCPtrHashDict<str_data, int> hash( str_data::hash_fn, 10, alloc_fn, 0 );
    hash.exceptions( WCExcept::check_all );
    str_data temp = "hello";

    try {
	// is this supposed to be a runtime error (alloc failed on index insert)
	cout << hash[ &temp ];
    } catch( ... ) {
	cout << "PASS " << __FILE__ << endl;
    }
    return 0;
};
