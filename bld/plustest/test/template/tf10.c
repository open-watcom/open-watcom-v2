template <class T>
    int eq( T x, T y )
    {
	return x == y;
    }

int eq( char, char );

char eq( double, double );	// dbl_eq should bind to this one
				// (it should not be instantiated because
				//  it returns a 'char')

int int_eq( int x, int y )
{
    return eq(x,y);
}

int dbl_eq( double x, double y )
{
    return eq(x,y);
}
