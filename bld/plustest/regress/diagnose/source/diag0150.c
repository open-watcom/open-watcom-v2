template <class T>
    struct S {
	void foo( T );
    };

template <class T>
    void S<T>::foo( T x )
    {
	*x = 1;
    }

S<int *> a;
S<char *> b;
S<int> c;
S<double *> d;

template <class Q>
    S<Q> ack( Q x );

void foo()
{
    char x;
    int y;

    ack(x);
}
