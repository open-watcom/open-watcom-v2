class S {
    struct T friend;
    friend struct Q;
    int a;
public:
    S();
};

struct T {
    static void foo( S *p );
};

struct Q {
    static void bar( S *p );
};

void T::foo( S *p )
{
    p->a = 1;
}

void Q::bar( S *p )
{
    p->a = 1;
}

class NEST {
    void foo( int );
    void foo( double );
    class NESTED {
	friend void foo( NESTED * );
	int q;
    public:
	NESTED();
    };
    int a;
public:
    NEST();
};

void foo( NEST::NESTED *p )
{
    p->q = 2;
}

void NEST::foo( NEST::NESTED *p )
{
    p->q = 1;
}
