#include "fail.h"

void foo( const int &r )
{
    if( r != 1 ) fail(__LINE__);
}

struct ONE {
    operator int() { return 1; }
};

int main()
{
    ONE one;

    foo( one );
    _PASS;
}
