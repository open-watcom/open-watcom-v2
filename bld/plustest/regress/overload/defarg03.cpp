#include "dump.h"

// order shouldn't matter!
// 3 2 1
struct B {
    void operator() (int i1, int i2); // (3)
    // add (int,int)
    void operator() (int i1, int i2, int i3 = 6); // (2)
    // add (int,int,int) and (int, int, [int] )
    void operator() (int i1, int i2 = 3); // (1) 
    // don't re-add (int,int) ( redeclaration of (1) )
    // add (int, [int] )
};
void B::operator() (int i1, int i2) GOOD;
void (B::*pmfb)(int, int) = &B::operator();

// 3 1 2
struct C {
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2, int i3 = 6); // (2)
};
void C::operator() (int i1, int i2) GOOD;
void (C::*pmfc)(int, int) = &C::operator();

// 1 2 3
struct D {
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2); // (3)
};
void D::operator() (int i1, int i2) GOOD;
void (D::*pmfd)(int, int) = &D::operator();

// 1 3 2
struct E {
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2, int i3 = 6); // (2)
};
void E::operator() (int i1, int i2) GOOD;
void (E::*pmfe)(int, int) = &E::operator();

// 2 1 3
struct F {
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2); // (3)
};
void F::operator() (int i1, int i2) GOOD;
#if __WATCOM_REVISION__ >= 8
void (F::*pmff)(int, int) = &F::operator();
#endif

// 2 3 1
struct G {
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2 = 3); // (1) 
};
void G::operator() (int i1, int i2) GOOD;
#if __WATCOM_REVISION__ >= 8
void (G::*pmfg)(int, int) = &G::operator();
#endif

int main()
{
    B b;
    C c;
    D d;
    E e;
#if __WATCOM_REVISION__ >= 8
    F f;
    G g;
#endif

    (b.*pmfb)(3,4);
    (c.*pmfc)(3,4);
    (d.*pmfd)(3,4);
    (e.*pmfe)(3,4);
#if __WATCOM_REVISION__ >= 8
    (f.*pmff)(3,4);
    (g.*pmfg)(3,4);
#else
    FORCE_GOOD(50);
    FORCE_GOOD(61);
#endif
    CHECK_GOOD(221);
    return errors != 0;
}

