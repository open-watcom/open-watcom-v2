struct S {
    int a[4];
    S( int, int = 1, int = 2, int );
};

void foo( void )
{
    /* look ma, no parms */
    S(3);
}

S::S( int x, int y, int z, int w )
{
    a[0] = x;
    a[1] = y;
    a[2] = z;
    a[3] = w;
}
