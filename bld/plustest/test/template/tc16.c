template <class T>
    struct X;
template <class T>
    struct Y;
template <class T>
    struct Y {
	int a[0x10];
	friend Y<T> *foo( X<T> * );
    };
template <class T>
    struct X {
	int b[0x100];
	friend Y<T> *foo( X<T> * );
    };

template <class T>
	Y<T> *foo( X<T> * )
	{
	    static int a;

	    ++a;
	    return 0;
	}

X<int> q;
