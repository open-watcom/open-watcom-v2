#include "fail.h"

struct ONE {
    int a;
    union {
        signed char b;
        int c;
        double d;
    };
};
struct ONE test_one = {
    1,
    -1,
};
struct TWO {
    int a;
    union {
        signed char b;
        int c;
        double d;
    };
    union {
        int e;
        double f;
    };
};
struct TWO test_two = {
    1,
    -1,
    -1,
};

int main()
{
    if( test_one.a != 1 ) fail(__LINE__);
    if( test_one.b != -1 ) fail(__LINE__);
    if( test_one.c == -1 ) fail(__LINE__);
    if( test_two.a != 1 ) fail(__LINE__);
    if( test_two.b != -1 ) fail(__LINE__);
    if( test_two.c == -1 ) fail(__LINE__);
    if( test_two.e != -1 ) fail(__LINE__);
    // strictly speaking this may load invalid FP values into FP regs
    // so we shouldn't do it (faults on AXP)
    // if( test_one.d == -1 ) fail(__LINE__);
    // if( test_two.d == -1 ) fail(__LINE__);
    // if( test_two.f == -1 ) fail(__LINE__);
    _PASS;
}
