struct V {
    int v;
    V();
    V(V&){v=__LINE__;}
};
struct A : virtual V {
    int a;
};
struct B : A {
    int b;
};
struct P {
    int p;
};
struct C : P, A, B {
    int c;
};
struct D : C {
    int d;
};

void bar( C,  D )
{
}

void main( void )
{
    C c;
    D d;

    bar( c, d );
}
