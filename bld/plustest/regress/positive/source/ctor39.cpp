#include "fail.h"

int p;

struct O {
    int s;
    O();
    ~O();
    O( O const & );
};

struct V : O {
    int s;
    O o;
    V();
    V( V const & );
    ~V();
};

struct DV : virtual V, O {
    int s;
    O o;
    DV();
    DV( DV const & );
    ~DV();
};

struct U3 : DV, O {
   O q;
   DV u;
};

struct DU3 : U3, O {
    int s;
   U3 a;
   O z;
    DU3();
    ~DU3();
};

O::~O() {
    if( p > 0 ) {
	--p;
	if( s != p ) _fail;
    } else {
	_fail;
    }
}
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
O::O() {
    s = p++;
}
V::V() : O(), o() {
    s = p++;
}
DV::DV() : V(), O(), o() {
    s = p++;
}
DU3::DU3() : U3(), O(), a(), z() {
    s = p++;
}
int main() {
    {
	DU3 x;
    }
    if( p != 0 ) _fail;
    _PASS;
}
