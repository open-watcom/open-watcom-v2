class Q;

class C {
    int a;
    friend class Q Q( struct C * );
public:
    C();
};

class Q {
    void foo( struct C * );
public:
    Q();
};

void Q::foo( struct C *p )
{
    p->a = 1;
}

class Q Q( struct C *p )
{
    p->a = 1;
}
