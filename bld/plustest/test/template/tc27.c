// we used to have trouble parsing this
template <class T>
    struct S {
	T v;
	S();
	~S();
	operator int();
	foo();
    };

template <class T>
    inline S<T>::S()
    {
    }

template <class T>
    inline S<T>::~S()
    {
    }

template <class T>
    inline S<T>::operator int()
    {
	return 0;
    }

template <class T>
    inline S<T>::foo()
    {
    }

S<int> x;
