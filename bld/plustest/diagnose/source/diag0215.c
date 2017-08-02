// compile -e3 -wx
struct S {
    void foo();
};

void foo( S const *p, int q1, int q2, int q3, int q4, int q5 )
{
    p->foo();
}

void bar( S const *p, int q1, int q2, int q3, int q4, int q5 )
{
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
    p->foo();
}
