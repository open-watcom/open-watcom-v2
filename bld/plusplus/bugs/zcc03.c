void foo( int x )
{
    void *p;

    p = (int*) (int&) &x;
}
