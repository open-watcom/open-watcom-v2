#include "fail.h"

// test for virtual base optimizations


struct V {
    int a;
    int rdbuf() {return a;}
    void set_a( int val ) { a = val; }
};
struct DV : virtual V {
    int a[2];
    DV() { a[0] = 1; a[1] = 2; set_a( 19 ); }
};

inline int foo(DV& _I) {
    return _I.rdbuf();
}

// can be optimized
void inlined_opt() {
    DV ins;
    int kkk = foo( ins );
    if( kkk != 19 ) fail(__LINE__);
    ins.set_a( 17 );
    kkk = foo( ins );
    if( kkk != 17 ) fail(__LINE__);
}

// cannot be optimized
void no_opt( DV & ins ) {
    int kkk = foo( ins );
    if( kkk != 23 ) fail(__LINE__);
    ins.set_a( 29 );
    kkk = foo( ins );
    if( kkk != 29 ) fail(__LINE__);
}

int main()
{
    inlined_opt();
    DV ins;
    ins.set_a( 23 );
    no_opt( ins );
    _PASS;
}

//#pragma on ( dump_exec_ic )
//#pragma dump_object_model DV;
