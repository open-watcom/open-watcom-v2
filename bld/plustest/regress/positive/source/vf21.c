#include "fail.h"

struct B1 {
    virtual ~B1();
};

struct B2 {
    virtual ~B2();
};

struct D : B1, B2 {
    D()
    { }

    virtual ~D()
    { }
};

int main() {
    // this shouldn't result in references to B1::~B1 and B2::~B2
    // being generated
    D *d = 0;

    _PASS;
}
