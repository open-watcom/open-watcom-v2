template <class T>
    struct S;

template <class T>
    void S<T>::foo( int x )
    {
	x.a = 1;
    }

S<int> *qq;

struct A {
    virtual void pure() = 0;
};

A *p1 = new A;
A **p2 = new A *;
A *p3 = new A[10][10];

