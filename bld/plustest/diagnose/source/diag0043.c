class A2B22F;
class A2B23F;
struct R {
    int : 2;
    void foo( void );
};

struct A1 : public R {
    int : 2;
    void foo( void );
};

struct A2 : private R {
    int : 2;
    class A2B22F friend;
    class A2B23F friend;
    void foo( void );
};

struct A3 : protected R {
    int : 2;
};

struct B11 : public A1 {
    int : 2;
    void foo( void );
};

struct B12 : private A1 {
    int : 2;
    void foo( void );
};

struct B13 : protected A1 {
    int : 2;
};

struct B21 : public A2 {
    int : 2;
};

struct B22 : private A2 {
    int : 2;
    class A2B22F friend;
};

struct B23 : protected A2 {
    int : 2;
    class A2B23F friend;
};

struct B31 : public A3 {
    int : 2;
};

struct B32 : private A3 {
    int : 2;
    void foo( void );
};

struct B33 : protected A3 {
    int : 2;
};

struct B13D : public B13 {	// derive from B13
    int : 2;
    void foo( void );
};

struct A2B22F {			// friend of A2 & B22
    int : 2;
    void foo( void );
};

struct A2B23F {			// friend of A2 & B23
    int : 2;
    void foo( void );
};

struct B33D : public B33 {	// derive from B33
    int : 2;
    void foo( void );
};

R *pR;
A1 *pA1;
A2 *pA2;
A3 *pA3;
B11 *pB11;
B21 *pB21;
B31 *pB31;
B12 *pB12;
B22 *pB22;
B32 *pB32;
B13 *pB13;
B23 *pB23;
B33 *pB33;

void foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void R::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void A2::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R (OK)
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R (OK)
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void B32::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R (OK)
    pR = pB33;		// protected B33->A3, protected A3->R
}

void B12::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R (OK)
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void B13D::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void A2B22F::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R (OK)
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R (OK)
    pR = pB22;		// private B22->A2, private A2->R (OK)
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void A2B23F::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R (OK)
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R (OK)
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R (OK)
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}

void B33D::foo( void )
{
    pR = pA1;		// public A1->R (OK)
    pR = pA2;		// private A2->R
    pR = pA3;		// protected A3->R
    pR = pB11;		// public B11->A1, public A1->R (OK)
    pR = pB12;		// private B12->A1, public A1->R
    pR = pB13;		// protected B13->A1, public A1->R
    pR = pB21;		// public B21->A2, private A2->R
    pR = pB22;		// private B22->A2, private A2->R
    pR = pB23;		// protected B23->A2, private A2->R
    pR = pB31;		// public B31->A3, protected A3->R
    pR = pB32;		// private B32->A3, protected A3->R
    pR = pB33;		// protected B33->A3, protected A3->R
}
