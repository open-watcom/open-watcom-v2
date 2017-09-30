#include "fail.h"

// GPFs if compiled -xs, -xst but ok if -xss
struct A {
    virtual int base_equiv( int ) const = 0;	// remove and works
    virtual ~A() {	// remove and works
    };
};
struct D1 : public A {
    virtual ~D1() {
	call_method();	// remove and works
    };
    void call_method();
};
void D1::call_method(){
}
struct D2 : public D1 {
    D2() : D1() {};	// remove and works
};
struct D3 : public D2 {
    virtual int base_equiv( int ) const {
	return 0;
    };
    D3();
};
D3::D3() {
}

ALWAYS_PASS
