class A;
template <class T>
    struct B;

struct X {
    void foo(B<A>*);
};

inline void X::foo(B<A>* bar) {
}

struct S {
    typedef int T;
    void foo( void (*f)( void ), T );
};
void S::foo( void (*f)( void ), T x )
{
}

class X20 {
    enum E { SIZE = 100 };
    static char a[SIZE];
    operator E();
};

char X20::a[SIZE];	// access to 'SIZE' should be OK

X20::operator E()
{
    return X20::SIZE;
}

X20::E x33( X20*p )
{
    return p->X20::operator E();
}
