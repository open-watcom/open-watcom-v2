struct A {
    int a;
};
struct D : A {
    int a;
};

void foo( D *p )
{
    struct A {
        int x;
    };
    struct D : A {
        int a;
    };
    p->A::a = 1;        // works under C7 and JPI
}
