#include "fail.h"
struct V1 { int v1; virtual void fV1(){}; virtual void fV2(){}; };
struct V2 : virtual V1 { int v2; virtual void fV2(){}; virtual void fV3(){}; };
struct V3 : virtual V1, virtual V2 { int v3; virtual void fV3(){}; virtual void fV4(){}; };
struct V4 : virtual V1, virtual V2, virtual V3 { int v4; virtual void fV4(){}; virtual void fV5(){}; };
struct V5 : virtual V1, virtual V2, virtual V3, virtual V4 { int v5; virtual void fV5(){}; virtual void fV6(){}; };
struct C : virtual V5 {
    C();
    C(int);
    C(char*);
};
C::C()
{
}

C::C(int)
{
}

C::C(char*)
{
}

void accv( C *pC, V1 *pV1, V2 *pV2, V3 *pV3, V4 *pV4, V5 *pV5 )
{
    if( pC->v1 != 1 ) fail(__LINE__);
    if( pC->v2 != 2 ) fail(__LINE__);
    if( pC->v3 != 3 ) fail(__LINE__);
    if( pC->v4 != 4 ) fail(__LINE__);
    if( pC->v5 != 5 ) fail(__LINE__);
    if( pV1->v1 != 1 ) fail(__LINE__);
    if( pV2->v1 != 1 ) fail(__LINE__);
    if( pV2->v2 != 2 ) fail(__LINE__);
    if( pV3->v1 != 1 ) fail(__LINE__);
    if( pV3->v2 != 2 ) fail(__LINE__);
    if( pV3->v3 != 3 ) fail(__LINE__);
    if( pV4->v1 != 1 ) fail(__LINE__);
    if( pV4->v2 != 2 ) fail(__LINE__);
    if( pV4->v3 != 3 ) fail(__LINE__);
    if( pV4->v4 != 4 ) fail(__LINE__);
    if( pV5->v1 != 1 ) fail(__LINE__);
    if( pV5->v2 != 2 ) fail(__LINE__);
    if( pV5->v3 != 3 ) fail(__LINE__);
    if( pV5->v4 != 4 ) fail(__LINE__);
    if( pV5->v5 != 5 ) fail(__LINE__);
}

void test( C *p )
{
    p->v1 = 1;
    p->v2 = 2;
    p->v3 = 3;
    p->v4 = 4;
    p->v5 = 5;
    accv( p, p, p, p, p, p );
}

int main() {
    C *p = new C;
    C *q = new C(0);
    C *r = new C("");
    test( p );
    test( q );
    test( r );
    _PASS;
}
