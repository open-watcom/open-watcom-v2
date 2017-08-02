#include "dump.h"

class X {
    public:
    void g() { GOOD };
    void g() const {GOOD};            // Ok: no static g
    void g() volatile {GOOD};         // Ok: no static g
    void g() const volatile {GOOD};   // Ok: no static g
};                                                      

int main()
{
    X x;
    const X xc;
    volatile X xv;
    const volatile X xcv;

    x.g();
    CHECK_GOOD(5)
    xc.g();
    CHECK_GOOD(5+6);
    xv.g();
    CHECK_GOOD(5+6+7);
    xcv.g();
    CHECK_GOOD(5+6+7+8);
    return errors != 0;
}
