// This test checks that the virtual function foo need not be defined
// and stuff will still link.
//
// compile with -zv
// link with option vfr (virtual functions removal)

#include "fail.h"

struct root {};

struct vb : public root {
    virtual int foo() = 0;
};

struct vb1 : public vb {

    void memb();
    
    };

typedef int ( root::*mpf )();

void pass( mpf )
{
}

void vb1::memb()
{
#if __WATCOM_REVISION__ >= 8
    pass( (mpf)&foo );
#else
    pass( (mpf)&vb1::foo );
#endif
}


struct bot : public vb1 {
   int foo();
};

int bot::foo()
{
    return 17;
}

int main()
{
    bot B;
    _PASS;
}
