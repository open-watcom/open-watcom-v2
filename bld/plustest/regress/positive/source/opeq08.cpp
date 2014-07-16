#include "fail.h"

unsigned C_count;
unsigned NC_count;

struct C {
    C() {}
    C( C const & ) {
	++C_count;
    }
    void operator =( C const & ) {
	++C_count;
    }
};

struct NC {
    NC() {}
    NC( NC & ) {
	++NC_count;
    }
    void operator =( NC & ) {
	++NC_count;
    }
};

struct CNC {
    CNC() {}
    CNC( CNC & ) {
	++NC_count;
    }
    CNC( CNC const & ) {
	++C_count;
    }
    void operator =( CNC & ) {
	++NC_count;
    }
    void operator =( CNC const & ) {
	++C_count;
    }
};

struct XC : C {
};

struct XNC : NC {
};

struct XCNC : CNC {
};

void try_XC( XC const &rc, XC &rnc )
{
    XC xc(rc);
    XC xnc(rnc);

    xc = rc;
    xnc = rnc;
}

void try_XNC( XNC const &, XNC &rnc )
{
    //XNC xc(rc); // not allowed
    XNC xnc(rnc);

    //xc = rc;	// not allowed
    xnc = rnc;
}

void try_XCNC( XCNC const &rc, XCNC &rnc )
{
    XCNC xc(rc);
    XCNC xnc(rnc);

    xc = rc;
    xnc = rnc;
}

int main()
{
    XC vc1, vc2;

    try_XC( vc1, vc2 );
    if( C_count != 4 ) fail(__LINE__);
    if( NC_count != 0 ) fail(__LINE__);
    XNC vnc1, vnc2;

    try_XNC( vnc1, vnc2 );
    if( C_count != 4 ) fail(__LINE__);
    if( NC_count != 2 ) fail(__LINE__);

    XCNC vcnc1, vcnc2;

    try_XCNC( vcnc1, vcnc2 );
    if( C_count != 8 ) fail(__LINE__);
    if( NC_count != 2 ) fail(__LINE__);

    _PASS;
}
