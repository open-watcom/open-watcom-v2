template <class T>
    void foo( T const &, T const & );

void foo( int x )
{
    foo(x,x);
}
