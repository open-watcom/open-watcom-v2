// check use of a function declaration
template <class T>
    int eq( T x, T y );

int (*f())( int, int )
{
    return eq;
}
