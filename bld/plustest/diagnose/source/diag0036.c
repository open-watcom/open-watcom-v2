class C;

struct S {
    operator C();
    operator int();
public:
    S();
};

class C {
    int a;
public:
    C();
    friend S::operator C();
    friend S::operator double();
};

C *p;

S::operator C()
{
    p->a = 1;
}

S::operator int()
{
    p->a = 1;
}
