#include "fail.h"

struct B {};
struct D : B {};
struct E : D {};

void fooD() {
    throw D();
}

void fooE() {
    throw E();
}

int main() {
#if __WATCOM_REVISION__ >= 8
    int count = 0;

    try {
	fooD();
    } catch( E ) {
	fail(__LINE__);
    } catch( B ) {
	++count;
    } catch( D ) {
	fail(__LINE__);
    } catch(...) {
	fail(__LINE__);
    }
    try {
	fooE();
    } catch( E ) {
	++count;
    } catch( B ) {
	fail(__LINE__);
    } catch( D ) {
	fail(__LINE__);
    } catch(...) {
	fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
#endif
    _PASS;
}
