#include "fail.h"
// TESTREF2.C -- TEST REFERENCES USING PARAMETER PASSING TO INITIALIZE
//
// 91/11/06     -- Ian McHardy          -- defined

static int static_int;          // static integer for 5.2.2

int & return_int_ref()
{
    return static_int;
}

class BASE
{
    public:
    int a;
};

class INHERIT: public BASE
{
    public:
    int b;
};

INHERIT i;                  // - inherit structure 

void init_inherit(              // INITIALIZE a, b OF OBJECT to 1, 1
    INHERIT *object )           // - ptr to objec to initialize
{
    object->a = 1;
    object->b = 1;
}

void fbr(                       // FUNCTION WITH REF TO BASE PARAMETER
    BASE& br )
{
    br.a = -br.a;
}

void fir(                       // FUNCTION WITH REF TO INHERIT PARAMETER
    INHERIT& ir )
{
    ir.a = -ir.a;
    ir.b = -ir.b;
}

void test(
    INHERIT *ip,                // - pointer to INHERIT structure
    BASE& br,                   // - reference to BASE of INHERIT structure
    INHERIT& ir )               // - reference to INHERITE structure
{
    BASE *bp;

// 4.7
    init_inherit( ip );
    if( ip->b != 1 ) fail(__LINE__);
    fir( ir );
    if( ip->a != -1 ) fail(__LINE__);
    if( ip->b != -1 ) fail(__LINE__);
    init_inherit( ip );
    fbr( br );
    if( ip->a != -1 ) fail(__LINE__);
    if( ip->b != 1 ) fail(__LINE__);

    init_inherit( &i );
    fir( i );
    if( i.a != -1 ) fail(__LINE__);
    if( i.b != -1 ) fail(__LINE__);
    init_inherit( &i );
    bp = &i;
    fbr( *bp );
    if( i.a != -1 ) fail(__LINE__);
    if( i.b != 1 ) fail(__LINE__);

// 5
    init_inherit( &i );
    br.a++;
    ir.a += 5;
    ir.b *= 3;
    if( i.a != 7 ) fail(__LINE__);
    if( i.b != 3 ) fail(__LINE__);

// 5.2.2
    static_int = 0;
    return_int_ref() = 7;
    if( static_int != 7 ) fail(__LINE__);
}


int main( void )
{
    test( &i, i, i );
    _PASS;
} 
