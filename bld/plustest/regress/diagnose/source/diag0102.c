class XX;

template <class T>
    struct S {
	void foo( XX *p );
    };

typedef S<int> QQQQ;

class XX {
    friend QQQQ;
    friend class S<double>;
    int bad;
};

S<int> x;
S<double> y;
S<char> z;

template <class T>
    void S<T>::foo( XX *p )
    {
	p->bad = 1;	// only one should be reported (S<char> instantiation)
    }
