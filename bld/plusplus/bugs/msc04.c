// is broken for everything else under the sun
struct S {
    int s;
    operator S *();
    int & operator ->* ( int S::* );
};

void foo( S x, int S::* q )
{
    (x->*q) = 1;        /* should report an ambiguity */
}
