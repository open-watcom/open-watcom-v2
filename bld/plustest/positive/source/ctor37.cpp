#include "fail.h"

int p;

struct V {
    int s;
    V();
    ~V();
};

struct DV : virtual V {
    int s;
    DV();
    ~DV();
};

struct U3 : DV {
   DV u;
};

struct DU3 : U3 {
    U3 m;
    int s;
    DU3();
    ~DU3();
};

V::~V() {
    if( p > 0 ) {
	--p;
	if( s != p ) _fail;
    } else {
	_fail;
    }
}
DV::~DV() {
    if( p > 0 ) {
	--p;
	if( s != p ) _fail;
    } else {
	_fail;
    }
}
DU3::~DU3() {
    if( p > 0 ) {
	--p;
	if( s != p ) _fail;
    } else {
	_fail;
    }
}
V::V() {
    s = p++;
}
DV::DV() : V() {
    s = p++;
}
DU3::DU3() : V(), U3(), m() {
    throw 1;
    s = p++;
}
int main() {
    try {
	DU3 x;
    } catch( ... ) {
    }
    _PASS;
}
