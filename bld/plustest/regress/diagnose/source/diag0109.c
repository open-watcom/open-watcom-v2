template <class T>
    class XX {
	int a;
	int a;
    };

template <class T>
    XX<T> foo( T, T );

void foo( int *(*x)( int * ) )
{
    foo( x, x );
}
