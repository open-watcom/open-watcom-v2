#include "fail.h"

struct A {
    struct B;

  private:
    static const int CONST = 1;
};

struct A::B {
    B();

    int array[CONST];
};

A::B::B()
{ }


int main() {
    A::B b;

    _PASS;
}
