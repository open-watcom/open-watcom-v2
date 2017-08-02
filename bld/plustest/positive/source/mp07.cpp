#include "fail.h"

struct S {
    int a,b,c;
    int v;
    S(int v):v(v){;}
};

int S::* mp = &S::v;

int main()
{
    if( (S(3)).*mp != 3 ) fail(__LINE__);
    _PASS;
}
