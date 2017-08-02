struct U {
    U();
    U(U&);
    U & operator = ( U & );
};
struct V : virtual U {
    V();
    V(V&);
    V & operator = ( V & );
};
struct S : virtual V {
    S();
    S(S&);
    S & operator = ( S & );
};
struct T : S {
    S a[5];
    int b[5];
    double c;
};

T x;
T y;

extern void bar( T );

void foo( void )
{
     bar( x );
}
