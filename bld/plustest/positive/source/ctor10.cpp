// 94/06/14 -- J.W.Welch        -- changed CDO copy ctor to remove side effect
#include "fail.h"

struct CDO;

struct SIGS {
    struct ITEM {
        ITEM *next;
        CDO *addr;
        ITEM( ITEM *next, CDO *addr ) : next(next), addr(addr) {}
    };
    ITEM *list;
    SIGS() : list(NULL) {}
    int already_ctored( CDO * );
    int already_dtored( CDO * );
    int all_dtored( void ) { return list == NULL; }
};

unsigned count;

struct CDO {    // ctor/dtor once
    int signature;
    unsigned index;
    CDO( int );
    CDO( CDO const & );
    ~CDO();
};

SIGS check;

int SIGS::already_ctored( CDO *addr )
{
    // printf( "checking ctor of %d %u\n", addr->signature, addr->index );
    for( ITEM *curr = list; curr != NULL; curr = curr->next ) {
        if( curr->addr == addr ) {
            fail(__LINE__);
            return( 1 );
        }
    }
    ITEM *item = new ITEM( list, addr );
    list = item;
    return( 0 );
}

int SIGS::already_dtored( CDO *addr )
{
    ITEM **prev = &list;

    // printf( "checking dtor of %d %u\n", addr->signature, addr->index );
    for( ITEM *curr = list; curr != NULL; curr = curr->next ) {
        if( curr->addr == addr ) {
            *prev = curr->next;
            delete curr;
            return( 0 );
        }
        prev = &(curr->next);
    }
    fail(__LINE__);
    return( 1 );
}

CDO::CDO( int v ) : signature(v), index(++count) {
    if( check.already_ctored( this ) ) {
        printf( "failure signature: %d\n", signature );
    }
}

CDO::CDO( CDO const &s ) : signature(s.signature), index(++count) {
    if( check.already_ctored( this ) ) {
        printf( "failure signature: %d\n", signature );
    }
}

CDO::~CDO() {
    if( check.already_dtored( this ) ) {
        printf( "failure signature: %d\n", signature );
    }
}

CDO inc_signature( CDO x )
{
    return CDO( x.signature + 1 );
}

struct C1 : CDO {
    C1(int);
    int val() const { return signature + 100; }
};

C1::C1( int v ) : CDO( inc_signature( CDO( v ) ) )
{
    CDO x( __LINE__ );
}

struct S : CDO {
    int x;
    S() : x(C1(45).val()), CDO(23) {}
};

int foo()
{
    S v;
    return v.x;
}

int main()
{
    if( foo() != 146 ) fail(__LINE__);
    if( ! check.all_dtored() ) fail(__LINE__);
    _PASS;
}
