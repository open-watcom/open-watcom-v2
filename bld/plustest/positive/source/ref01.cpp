#include "fail.h"

struct S {
    char &r;
    char a[3];
    S(int x ) : r(a[1]) { a[1] = x; }
};

int x;

int main()
{
    S z(1);

    if( z.r = x, z.r != z.a[1] || z.r != 0 || z.a[1] != 0 ) fail(__LINE__);
    ++x;
    if( z.r = x, z.r != z.a[1] || z.r != 1 || z.a[1] != 1 ) fail(__LINE__);
    ++x;
    if( z.r = x, z.r != z.a[1] || z.r != 2 || z.a[1] != 2 ) fail(__LINE__);
    _PASS;
}
