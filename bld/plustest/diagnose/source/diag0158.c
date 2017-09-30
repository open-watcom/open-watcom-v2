template <class T>
    struct S {
	typedef T DD;
	operator DD *();
    };

S<int> x;
S<char> y;

template <class T>
    S<T>::operator DD *()
    {
	return 0;
    }
