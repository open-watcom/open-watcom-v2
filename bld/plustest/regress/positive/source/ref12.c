#include "fail.h"

// TESTREF.C -- TEST REFERENCES
//
// 91/10/30     -- Ian McHardy          -- defined

static int static_int;          // static integer for 5.2.2

static int int_array[ 20 ];     // static array for 8.2.2

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

int& return_int_ref( void )     // FUNTION THAT RETURNS REF TO static_int
{
    return( static_int );
}

void f8_2_2(                    // F FROM SECTION 8.2.2
    double& a )
{
    a += 3.14;
}

int& g8_2_2(                    // G FROM SECTION 8.2.2
    int i )                     // - subscript
{
    return( int_array[ i ] );
}

struct link{                    // FROM SECTION 8.2.2
    link* next;
};

link *first;

void H8_2_2( link*& p )         // 'p' is a ref to pointer (FROM 8.2.2)
{
    p->next = first;
    first = p;
    p = 0;
}

int main( void )
{
    INHERIT i;

    BASE& br = i;// = i;
    INHERIT& ir = i;// = i;
    BASE *bp;

// 4.7
    init_inherit( &i );
    if( i.b != 1 ) fail(__LINE__);
    fir( ir );
    if( i.a != -1 ) fail(__LINE__);
    if( i.b != -1 ) fail(__LINE__);
    init_inherit( &i );
    fbr( br );
    if( i.a != -1 ) fail(__LINE__);
    if( i.b != 1 ) fail(__LINE__);

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

// 5.3
    int integer;
    int& intr = integer;

//    *(int * *)intr = &integer;
    integer = 0;
    if( &intr != &integer ) fail(__LINE__);

// 5.4
//    INHERIT& ir2; = (INHERIT&)br;


// 8.2.2
    double double_var;

    double_var = 0;
    f8_2_2( double_var );
    if( ( double_var < 3.1 )
      ||( double_var > 3.2 ) ) fail(__LINE__);

    int_array[ 3 ] = 0;
    g8_2_2( 3 ) = 27;
    if( int_array[ 3 ] != 27 ) fail(__LINE__);

    link* q;
    link* copy_q;

    q = new link;
    copy_q = q;
    H8_2_2( q );
    if( q != 0 ) fail(__LINE__);
    if( first != copy_q ) fail(__LINE__);
    _PASS;
}
