// causes MetaWare to fail
// Zortech can throw away 'x' and alias 'x' to the return struct!!!
struct S {
    S(S&);
    S();
    int a;
};

S foo(int a)
{
    S x;
    S y;
    x.a = a;
    y.a = ++a;
    return x;
}

S x = foo(2);
