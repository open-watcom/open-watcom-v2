// there is nothing preventing this from being legal
struct S {
    static operator++( S &, int );
};

S x;

void foo( void )
{
    x.operator ++(x,2);
}
