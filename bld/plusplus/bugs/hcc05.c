struct S {
    void foo( S &, int = 0 );
};

void foo( S const &r )
{
    S x(r);
}
