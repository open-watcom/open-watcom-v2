#include <iostream.h>
#include <wcskip.h>
#include "strdata.h"

int test() {
    const WCPtrSkipListDict<str_data, int> skip_list;
    str_data temp = "hello";
    int * const *p;

    try {
	p = &skip_list[ &temp ];
	cout << int( p == 0 ) << endl;
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
