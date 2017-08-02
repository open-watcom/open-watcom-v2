struct S { int a; };
struct T : S { int a; };
struct W : T { int a; };

void foo( W *p )
{
    p->a = 1;		// OK
    p->T::a = 1;	// OK
    p->S::a = 1;	// OK
}
