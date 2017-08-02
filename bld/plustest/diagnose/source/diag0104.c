template <class T>
    T main( T argc, T **argv )
    {
    }

template <class T>
    T WinMain( T argc, T **argv )
    {
    }

template <class T>
    T LibMain( T argc, T **argv )
    {
    }

void x16( void )
{
    for( int a = 0; a < 9; a++ ) {
	if( a == 0 ) continue;
	const int b = 2;
    }
}

extern "C" {
    template <class T>
	T max( T x, T y )	// forced to C++ linkage
	{
	    return ( x > y ) ? x : y;
	}
};

int q( int a, int b )
{
    return max(a,b);
}

int r( char a, char b )
{
    return max(a,b);
}
