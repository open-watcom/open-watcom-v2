struct T {
    T();
    T(T&);
    int t;
};
T::T()
{
    t = 'd';
}
T::T(T&)
{
    t = 'c';
}
struct P : T {
    P();
    int p;
};
P::P()
{
    p = 'd';
}
struct S : P {
    static S &x;
    S( S&    , int      );
    S( S&    , int = -2 );	/* copy ctor */
    S( S& = x, int      );	/* default ctor */
    int s;
};

S::S( S&, int )
{
}

S q[10];
S r = q[5];
