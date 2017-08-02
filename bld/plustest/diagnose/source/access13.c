struct Y { int a; };
struct Q : Y { int a; };
struct X : Q, virtual Y {};
struct Z : virtual Y {};
struct W : X, Z {};

void foo( W *p )
{
    p->a = 1;		// OK; Q::a dominates Y::a
    p->Y::a = 1;	// ambiguous
    p->Q::a = 1;	// OK
}
