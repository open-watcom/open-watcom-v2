// address overloads also select functions
template <class T>
    int eq( T x, T y )
    {
	return x == y;
    }

int (*f())( int, int )
{
    return eq;
}
