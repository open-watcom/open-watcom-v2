// page 204 ARM
struct V {
    int v;
};

struct A {
    int a;
};

struct B : A, virtual V {
};

struct C : A, virtual V {
};

struct D : B, C {
};

void foo( D *p )
{
    p->a = 1;		// ambiguous
    p->v = 1;		// OK
}
