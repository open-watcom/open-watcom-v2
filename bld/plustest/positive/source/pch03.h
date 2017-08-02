#include "fail.h"

class C : public _CD {
    public:
    int a;
    C();
    ~C();
};

static C x;

int doo( int x ) {
    static C y;
    int z = y.a;
    y.a = x;
    return z;
}

C::C() {
    a = -34;
}

C::~C() {
    a = 34;
}
