// Test case: overload\udcnv13.cpp
// Status: work complete
#include "dump.h"

struct X
{
    X(...) {GOOD}; // line 5
    X(X const &) { GOOD }; // line 6
};

int main()
{
    X x2 = 2;
    CHECK_GOOD(5);
    return errors != 0;
}
