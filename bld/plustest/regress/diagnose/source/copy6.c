struct S {
    S();
    S(S&);
    S & operator = ( S & );
};
struct T : S {
    S a[5];
    int b[5];
    double c;
};

void foo( T x )
{
    foo( x );
}

void foo( T *a, T *b )
{
    *a = *b;
}
