struct X1 {
};

void foo( X1 );
void foo( X1 const );

template <class T>
    struct Z2 {
	friend class X;
	private:
	int a;
    };

Z2<int> z8;
Z2<float> z9;

struct X {
    void foo( Z2<int> *p )
    {
	p->a = 1;
    }
};

struct Y {
    void foo( Z2<float> *p )
    {
	p->a = 1;
    }
};

template <class T,class G>
    struct OVER {
	T a;
	void foo( T x );
    };

template <class G,class Q>
    void OVER<G,Q>::foo( G x )
    {
	G y;
	G dm;

	*dm = 1;	// member function's G overrides class template's G
	y = x;
	a = y;
    }

OVER<int,int *> x;

extern int boo();
template <class T>
    struct S {
	S *n;
	S<T> *p;
	void foo( T x )
	{
	    S<T> *q;
	    int S;
	    S<T> *p; error

	    S = boo();
	    if( S < x ) {
		S += boo();
	    }
	}
    };

S<int> d;

void foo()
{
    d.foo(1);
}
