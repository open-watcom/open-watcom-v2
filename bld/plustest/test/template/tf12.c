template <class T>
    int eq( T x, T y )
    {
	return x == y;
    }

int eq(int x,int y)
{
    return x ^ y;
}

int (*f())( int, int )
{
    return eq;
}
