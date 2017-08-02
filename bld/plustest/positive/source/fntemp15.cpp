#include "fail.h"

template <class T>
    void tf(T) {
	return;
    }

struct A { } a;

struct B { } b;

extern "C" void bar() {
    tf(a);
    tf(b);
}

int main() {
    bar();
    _PASS;
}
