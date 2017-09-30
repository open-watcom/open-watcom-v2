template <class T>
    struct S;

template <class T>
    void S<T>::foo( int x )
    {
	x.a = 1;
    }

template <class T>
    struct S {
	int foo( int );
    };

S<int> *qq;

template class S<int>;
