struct V {
};

struct A {
};

struct B : A, virtual V {
};

struct C : A, virtual V {
};

struct D : B, C {
};

void foo( void )
{
    D d;
    B *pb;
    A *pa;
    V *pv;

    pb = &d;		// OK; one B
    pa = &d;		// error; two A's
    pv = &d;		// OK; one V
}
