template <class T>
    struct S {
	operator T();
    };

template <class T>
    inline S<T>::operator T()
    {
	return 0;
    }

S<int> x;
