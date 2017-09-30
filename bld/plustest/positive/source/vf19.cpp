#include "fail.h"

char called;

struct A {
    virtual void opn () const = 0;
};

struct B : public virtual A {
    virtual void opn () const;
};

void B::opn() const {
    called = 'B';
}

struct D1 : public virtual B {
    virtual void opn () const;
};

void D1::opn() const {
    called = 'D';
}

struct D2 : public virtual B {
};

struct E : public virtual D1, public virtual D2 {
    E();
};

E:: E(){
}

int main () {
    E v1;
    
    A *pA = &v1;
    pA->opn();
#if __WATCOM_REVISION__ >= 8
    if( called != 'D' ) _fail;
#else
    if( called != 'B' ) _fail;
#endif
    B *pB = &v1;
    pB->opn();
    if( called != 'D' ) _fail;
    D1 *pD1 = &v1;
    pD1->opn();
    if( called != 'D' ) _fail;
    D2 *pD2 = &v1;
    pD2->opn();
    if( called != 'D' ) _fail;
    E *pE = &v1;
    pE->opn();
    if( called != 'D' ) _fail;
    _PASS;
}
