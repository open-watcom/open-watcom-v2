#include "fail.h"

struct B1 {
};
struct B2 {
};
struct D : B1, B2 {
};

static void foo( D &p ) {
    throw p;
}
static void foo() {
    D x;
    try {
    foo( x );
    } catch( B1 &p ) {
    }
}
