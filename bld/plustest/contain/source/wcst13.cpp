#include <iostream.h>
#include <wcskip.h>
#include "strdata.h"

int test() {
    const WCValSkipListDict<str_data, int> skip_list;
    //skip_list.exceptions( WCExcept::check_all );
    str_data temp = "hello";
    int const *p;

    try {
	p = &(skip_list[ temp ]);
	cout << int(p==0) << endl;
    } catch( WCExcept::failure ) {
	cout << "PASS" << endl;
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
