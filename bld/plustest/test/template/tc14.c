template <class T,int size>
    struct X {
	static int qw[size];
	void foo( int a[][size] );
    };

template <class T,int size>
    int X<T,size>::qw[size] = {1,2,3};

template <class T,int size>
    void X<T,size>::foo( int a[][size] )
    {
    }

X<int,10> z;
