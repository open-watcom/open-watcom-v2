template <class T>
    struct S1 {
	S2<T> x;
    };

template <class T>
    struct S2 {
	void operator =( int );
	void foo( S1<T> *p )
	{
	    p->x = 1;
	}
    };

S2<double> xx;
