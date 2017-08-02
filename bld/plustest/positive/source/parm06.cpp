#include "fail.h"

struct POD1 { int x, y; };
struct POD2 { int x, y; };
struct POD3 { int x, y; };

struct NONPOD : POD1, POD2, virtual POD3 {
    NONPOD();
};

POD1 *p1;
POD2 *p2;
POD3 *p3;

NONPOD::NONPOD() {
    p1 = this;
    p1->x = __LINE__;
    p1->y = __LINE__;

    p2 = this;
    p2->x = __LINE__;
    p2->y = __LINE__;

    p3 = this;
    p3->x = __LINE__;
    p3->y = __LINE__;
};

void want_POD1( POD1 v )
{
    if( v.x != p1->x ) fail(__LINE__);
    if( v.y != p1->y ) fail(__LINE__);
}

void want_POD2( POD2 v )
{
    if( v.x != p2->x ) fail(__LINE__);
    if( v.y != p2->y ) fail(__LINE__);
}

void want_POD3( POD3 v )
{
    if( v.x != p3->x ) fail(__LINE__);
    if( v.y != p3->y ) fail(__LINE__);
}

int main()
{
    NONPOD v;

    want_POD1( v );
    want_POD2( v );
    want_POD3( v );
    _PASS;
}
