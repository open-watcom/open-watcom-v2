class C {
    friend class Q;
    int a;
public:
    C();
};

class Q {
    void foo( C *p );
public:
    Q();
};

class R {
    void foo( C *p );
public:
    R();
};

void Q::foo( C *p )
{
    p->a = 1;
}

void R::foo( C *p )
{
    p->a = 1;
}

typedef int BAD;

class W {
    friend class BAD;
    int a;
public:
    W();
};

class BAD {
    void foo( W *p );
public:
    BAD();
};

void BAD::foo( W *p )
{
    p->a = 1;
}
