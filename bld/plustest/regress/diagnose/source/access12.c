struct Y { int a; };
struct Q : Y {};
struct X : Q, virtual Y { int a; };
struct Z : virtual Y {};
struct W : X, Z {};

void foo( W *p )
{
    p->a = 1;	// OK
}
