template <class T>
    struct S {
	T w;
	void *foo( int x )
	{
	    S q;
	    return q;
	}
    };

template <class T>
    S<T> foo( T, T * )
    {
    }

void foo( int x )
{
    foo( x, &x );
}
