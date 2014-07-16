#include "fail.h"

unsigned count;

struct C {
    C() { ++count; }
};

struct CA {
    C a[5];
    CA() {}
};

struct ICA {
    CA f;
    ICA() {}
};

int main()
{
    if( count != 0 ) fail(__LINE__);
    ICA x;
    if( count != 5 ) fail(__LINE__);
    _PASS;
}
