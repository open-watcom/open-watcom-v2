// 14.1p15: "When parsing a default template-argument for a non-type
// template-parameter, the first non-nested > is taken as the end of
// the template-parameter-list rather than a greater-than operator."

#include "fail.h"

template<int n = (1 > 2) >
struct X {
    int member;
    X( ) : member( n ) { }
};

int main()
{
    X<> object;

    if( object.member != 0 ) fail(__LINE__);


    _PASS;
}
