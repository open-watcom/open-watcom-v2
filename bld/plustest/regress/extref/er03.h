#include <stdio.h>
#include <stdlib.h>

struct B {
    B();
    virtual ~B();
};
struct D : B {
    D();
};
