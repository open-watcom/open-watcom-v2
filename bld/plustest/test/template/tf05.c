// check overloading
template <class T>
    int eq( T x, T y ) { return x == y; }

int eq(int x,int y)	// optimized version
{
    return x ^ y;
}

void f( int a, int b, char c, char d )
{
    int m1 = eq( a, b );
    int m2 = eq( c, d );
    int m3 = eq( a, c );
}
