class Q {
    class C friend;
    friend class D;
    int a;
public:
    void foo( C *p );
};

class R {
    class P {};
public:
    int a;
};

class D : public R {
    friend class Q;
    class C friend;
    friend class P;
    int a;
public:
    void foo( Q *p );
};

class C {
    friend class Q;
    int a;
public:
    void foo( Q *p );
};

void Q::foo( C *p )
{
    p->a = 1;
}

void C::foo( Q *p )
{
    p->a = 1;
}

void D::foo( Q *p )
{
    p->a = 1;
}
