
// check that the correct scope is used for the template parameter.

#include "fail.h"

const int c = 0;

template<int n = c>
struct X {
    static const int val = n;
};

int main()
{
    static const int c = 1;

    X<> object;
    if( object.val != 0 ) fail(__LINE__);

    X<c> object2;
    if( object2.val != 1 ) fail(__LINE__);

    _PASS;
}
