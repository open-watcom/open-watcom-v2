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
struct S : T {
    static S &x;
#if 0
    S( S&    , int      );
    S( S&    , int = -2 );      /* copy ctor */
    S( S& = x, int      );      /* default ctor */
#else
    S( S& = x, int = -2 );      /* default or copy ctor? */
#endif
    int s;
};

S::S( S&, int )
{
}

S q[10];
S r = q;
