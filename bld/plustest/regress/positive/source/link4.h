template <class T>
    struct S {
	struct N {
	    int foo();
	};
	int foo();
    };

template <class T>
    int S<T>::foo()
    {
	return 1 * sizeof( T );
    }

template <class T>
    int S<T>::N::foo()
    {
	return 2 * sizeof( T );
    }
