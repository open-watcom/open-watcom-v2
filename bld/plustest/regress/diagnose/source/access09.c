struct A {
    int a;
};

struct B : A {
    int a;
};

struct C : B {
    int a;
};

void foo( C *p )
{
    p->a = 1;
}
