#include <iostream.h>
#include <wchash.h>
#include "strdata.h"

int test() {
    WCPtrHashDict<str_data, int> hash( str_data::hash_fn );
    hash.exceptions( WCExcept::check_all );
    str_data temp = "hello";
    int x = 5;

    try {
	hash[ &temp ] = &x;
	cout << *hash[ &temp ] << endl;
    } catch( ... ) {
	cout << "PASS " << __FILE__ << endl;
    }
    return 0;
};

int main() {
    int ret = 1;
    try {
	ret = test();
    } catch(...) {
	--ret;
    }
    return ret;
}
