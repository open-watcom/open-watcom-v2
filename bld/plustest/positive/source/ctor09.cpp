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

struct CDO {	// ctor/dtor once
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

CDO::CDO( CDO const &s ) : signature(-(s.signature)), index(++count) {
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
};

C1::C1( int v ) : CDO( inc_signature( CDO( v ) ) )
{
    CDO x( __LINE__ );
}

struct C2 : CDO {
    C2(int);
};

C2::C2( int v ) : CDO( inc_signature( CDO( v ) ) )
{
    CDO x( __LINE__ );

    if( v ) {
	CDO y(__LINE__);

	return;
    }
}

struct C3 : CDO {
    C3(int);
};

C3::C3( int v ) : CDO( inc_signature( CDO( v ) ) )
{
    CDO x( __LINE__ );

    if( v ) {
	CDO y(__LINE__);

	return;
    }
    return;
}

struct C4 : CDO {
    C4(int);
};

struct X : CDO {
    X(int v ) : a(v), CDO(v) {}
    ~X() { CDO t(__LINE__); }
    int a;
};

void do_throw( int v )
{
    //printf( "throwing exception\n" );
    throw X(v);
}

C4::C4( int v ) : CDO( inc_signature( CDO( v ) ) )
{
    CDO x( __LINE__ );

    if( v ) {
	CDO y(__LINE__);

	if( v )
	    do_throw( v );
	return;
    }
    return;
}

void check_ctor_temps( int control )
{
    C1 z1(__LINE__);
    C2 z2(__LINE__);
    C3 z3(__LINE__);
    C4 z4(control);
}

int main()
{
    check_ctor_temps( 0 );
    if( ! check.all_dtored() ) fail(__LINE__);
    {
	{
	    CDO x(__LINE__);
	}
	if( ! check.all_dtored() ) fail(__LINE__);
	CDO x(__LINE__);
	try {
	    check_ctor_temps( -5555 );
	} catch( X const &r ) {
	    if( r.a != -5555 ) {
		fail(__LINE__);
	    }
	} catch( ... ) {
	    fail(__LINE__);
	}
    }
    if( ! check.all_dtored() ) fail(__LINE__);
    _PASS;
}
