#include "fail.h"
#include <except.h>

static int flag;

void terminate() {
    fail(__LINE__);
}
void unexpected() {
    fail(__LINE__);
}

int main() {
#if __WATCOM_REVISION__ >= 7
    for( int cnt = 0; cnt < 1200; ++cnt ) {
        try {
            if (!flag) {
                throw 1;
            }
            flag = 0;
        } catch (...) {
            while (1) {
		try {
		    flag = 1;
		    break;
		} catch (...) {
		    fail(__LINE__);
		}
            }
        }
    }
#endif
    _PASS;
}
