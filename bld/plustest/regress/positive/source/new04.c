#include "fail.h"
// #pragma on ( dump_exec_ic )

// test NEW operator
//
//

#include <stddef.h>


int ctored;

#define checkInt( p, r ) if( (p)->s != r ) fail( __LINE__ );
#define checkCtor( v ) if( v != ctored ) fail( __LINE__ );

char alloc[1024];

struct S {                  // NO CTOR/DTOR
    int s;
};

struct SC {                 // CTOR, NO DTOR
    int s;
    SC() :s(19) {};
    SC( int v ) :s(v) {};
};

struct SCD {                // CTOR, DTOR
    int s;
    SCD() : s(23) { ++ ctored; };
    SCD( int v ) : s(v) { ++ ctored; };
    ~SCD() { -- ctored; };
};

struct SCVD {               // CTOR, VIRTUAL DTOR
    int s;
    SCVD() : s(27) { ++ ctored; };
    SCVD( int v ) : s(v) { ++ ctored; };
    virtual ~SCVD() { -- ctored; };
};

struct S_p : S {            // NO CTOR/DTOR, placement
    void* operator new( size_t, void* area ) { return area; }
    void operator delete( void* ) {}
    void* operator new[]( size_t, void* area ) { return area; }
    void operator delete[]( void* ) {}
};

struct SC_p : SC {          // CTOR, NO DTOR, placement
    SC_p( int v ) : SC(v) {}
    SC_p() : SC() {}
    void* operator new( size_t, void* area ) { return area; }
    void operator delete( void* ) {}
    void* operator new[]( size_t, void* area ) { return area; }
    void operator delete[]( void* ) {}
};


struct SC_r : SC {          // CTOR, DTOR, operators delete, all __pascal
    __pascal SC_r( int v ) : SC(v) {}
    __pascal SC_r() : SC() {}
    void* __pascal operator new( size_t, void* area ) { return area; }
    void __pascal operator delete( void* ) {}
    void* __pascal operator new[]( size_t, void* area ) { return area; }
    void __pascal operator delete[]( void* ) {}
};

int main()
{

                            // int
    
    int* p0 = new int;      // - element
    delete p0;
    p0 = new int[9];        // - array
    delete p0;

                            // STRUCT S
    
    S* p1 = new S;          // - element
    delete p1;

    S* p3 = new S[3];       // - array
    delete[] p3;

                            // STRUCT SC

    SC* p4 = new SC;        // - element
    checkInt( p4, 19 );
    delete p4;

    SC* p5 = new SC(17);    // - element, inited
    checkInt( p5, 17 );
    delete p5;

    SC* p6 = new SC[3];     // - array
    checkInt( &p6[0], 19 );
    checkInt( &p6[1], 19 );
    checkInt( &p6[2], 19 );
    delete[] p6;

                            // STRUCT SCD

    SCD* p7 = new SCD;      // - element
    checkInt( p7, 23 );
    checkCtor( 1 );
    delete p7;
    checkCtor( 0 );

    SCD* p8 = new SCD(19);  // - element, inited
    checkInt( p8, 19 );
    checkCtor( 1 );
    delete p8;
    checkCtor( 0 );

    SCD* p9 = new SCD[3];   // - array
    checkInt( &p9[0], 23 );
    checkInt( &p9[1], 23 );
    checkInt( &p9[2], 23 );
    checkCtor( 3 );
    delete[] p9;
    checkCtor( 0 );

                            // STRUCT SCVD

    SCVD* pa = new SCVD;    // - element
    checkInt( pa, 27 );
    checkCtor( 1 );
    delete pa;
    checkCtor( 0 );

    SCVD* pb = new SCVD(19);// - element, inited
    checkInt( pb, 19 );
    checkCtor( 1 );
    delete pb;
    checkCtor( 0 );

    SCVD* pc = new SCVD[3]; // - array
    checkInt( &pc[0], 27 );
    checkInt( &pc[1], 27 );
    checkInt( &pc[2], 27 );
    checkCtor( 3 );
    delete[] pc;
    checkCtor( 0 );

                                        // STRUCT S_p

    S_p* pd = new (alloc) S_p;
    delete pd;

    S_p* pe = new (alloc) S_p[3];
    delete[] pd;

                                        // STRUCT SC_p
    SC_p* pf = new (alloc) SC_p;        // - element
    checkInt( pf, 19 );
    delete pf;

    SC_p* pg = new (alloc )SC_p(17);    // - element, inited
    checkInt( pg, 17 );
    delete pg;

    SC_p* ph = new (alloc) SC_p[3];     // - array
    checkInt( &ph[0], 19 );
    checkInt( &ph[1], 19 );
    checkInt( &ph[2], 19 );
    delete[] ph;

                                        // STRUCT SC_r
    SC_r* pi = new (alloc) SC_r;        // - element
    checkInt( pi, 19 );
    delete pi;

    SC_r* pj = new (alloc )SC_r(17);    // - element, inited
    checkInt( pj, 17 );
    delete pj;

    SC_r* pk = new (alloc) SC_r[3];     // - array
    checkInt( &pk[0], 19 );
    checkInt( &pk[1], 19 );
    checkInt( &pk[2], 19 );
    delete[] pk;

    _PASS;
}
