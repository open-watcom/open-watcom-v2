#include "fail.h"

struct xa { xa(); int x; };
xa::xa() : x(0xaaaa) {
}
struct xb { xb(); int x; };
xb::xb() : x(0xbbbb) {
}
struct xc { xc(); int x; };
xc::xc() : x(0xcccc) {
}

int seq;

void sequence( int x, unsigned line )
{
    if( x != seq ) fail(line);
    ++seq;
}

void test() {
    try {
        throw;
    } catch (xa& a) {
	sequence( 0, __LINE__ );
    } catch (xb& b) {
	sequence( 1, __LINE__ );
    } catch (...) {
	sequence( 2, __LINE__ );
    }
}

int main() {
    try {
        throw xa();
    } catch (...) {
        test();
    }
    try {
        throw xb();
    } catch (...) {
        test();
    }
    try {
        throw xc();
    } catch (...) {
        test();
    }
    _PASS;
}
