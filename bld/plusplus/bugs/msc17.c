struct S {
    S(int);
    operator int();
    int v;
};

int x;

int foo( S *p )
{
    return x ? *p : 0 ;
}
