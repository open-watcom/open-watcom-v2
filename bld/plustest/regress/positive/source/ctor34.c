#include "fail.h"

enum cd_progress {
    C_V,
    C_DV,
    C_DU3,
    D_DU3,
    D_DV,
    D_V,
    D__
};

cd_progress check[] = {
    C_V,
    C_DV,
    C_V,
    C_DV,
    C_DU3,
    D_DU3,
    D_DV,
    D_V,
    D_DV,
    D_V,
    D__,
};
int c = 0;

struct V {
    int v[5];
    V();
    ~V();
};
V::V() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != C_V ) _fail;
}
V::~V() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != D_V ) _fail;
}
struct DV : virtual V {
    int dv[10];
    DV();
    ~DV();
};
DV::DV() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != C_DV ) _fail;
}
DV::~DV() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != D_DV ) _fail;
}

struct U3 : DV {
   DV u;
};
struct DU3 : public U3 {
    DU3();
    ~DU3();
};
DU3::DU3() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != C_DU3 ) _fail;
}
DU3::~DU3() {
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c++] != D_DU3 ) _fail;
}

int main() {
    {
	DU3 x;
    }
    if( c >= (sizeof(check)/sizeof(check[0])) ) _fail;
    if( check[c] != D__ ) _fail;
    _PASS;
}
