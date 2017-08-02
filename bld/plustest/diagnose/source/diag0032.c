class Q;

class C {
    int a;
    friend class Q *foo( C * );
public:
    C();
};

class Q {
    void foo( C * );
public:
    Q();
};

void Q::foo( C *p )
{
    p->a = 1;
}

class Q *foo( C *p )
{
    p->a = 1;
    return( 0 );
}
