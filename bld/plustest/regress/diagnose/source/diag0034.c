class Q;
class C {
    int a;
    enum Q { Q1, Q2, Q3 };
    friend class Q Q( struct C * );
public:
    C();
};
class D {
    int a;
    typedef int Z;
    friend class Z;
public:
    D();
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
