struct V {
    V();
    V(V&);
    V & operator = ( V & );
};
struct S : virtual V {
    S();
    S(S&);
};
struct T : S {
    T();
    T(T&);
    S a[5];
    int b[5];
    double c;
};

void foo( T *x, T *y )
{
    *x = *y;
}
