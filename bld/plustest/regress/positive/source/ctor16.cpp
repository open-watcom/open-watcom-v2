#include "fail.h"

struct S {
    char af;
    int bf : 3;
    char cf;
    S() : af('a'), bf(2), cf('c') { }
};

S x;

int main()
{
    if( x.af != 'a' ) fail(__LINE__);
    if( x.bf != 2 ) fail(__LINE__);
    if( x.cf != 'c' ) fail(__LINE__);
    _PASS;
}
