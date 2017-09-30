#include "fail.h"

#if __WATCOM_REVISION__ >= 7
int do_something;
struct err {
    err(int){}
    ~err(){
	++do_something;
    }
};
struct B {
    int b;
    virtual void *foo();
};
struct D : virtual B {
    int d;
    virtual void *foo() { ++d; throw err(0); /*return 0;*/ }
};

D x;
#endif

int main() {
    _PASS;
}
#if __WATCOM_REVISION__ >= 7
void *B::foo() {
    return &x;
}
#endif
