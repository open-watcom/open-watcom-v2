template <class T,int size>
    struct Y {
	int a[0x10];
    };
template <class T,int size>
    struct X {
	int b[0x100];
	static Y<T,size> *a[];
    };

template <class T,int size>
    Y<T,size> *X<T,size>::a[] = { (Y<T,size> *) 1, (Y<T,size> *) 2 };

X<int,10> q;
