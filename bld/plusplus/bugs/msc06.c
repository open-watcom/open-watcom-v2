/* should give only one error */
struct S {
protected:
    int s;
};
struct T : virtual S {
    int t;
    void foo();
};
struct R : virtual S {
    int r;
    void foo();
};
struct Q : R, T {
    int q;
};

Q *x;

void T::foo()
{
    x->s = 1;           /* OK (choose Q -> T -> S) */
}

void R::foo()
{
    x->s = 1;           /* OK (choose Q -> R -> S) */
}

void foo( void )
{
    x->s = 1;           /* BAD */
}
