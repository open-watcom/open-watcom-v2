struct S {
    S(S&);
    S&operator=(S&);
};

struct C : S {
    const int c;
    C();
};

struct R : S {
    int &r;
    R();
};

R *x, *y;
extern void foo( C x );

void foo( void )
{
    C i;

    foo( i );
    *x = *y;
}
