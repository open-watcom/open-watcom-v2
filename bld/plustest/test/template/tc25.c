int a(void);
template <int (*p)(void) = a,int s = 2>
    struct D {
	int (*f1)(void);
	int f2;
	D()
	{
	    f1 = p;
	    f2 = s;
	}
    };

int b(void);

D<> p;
D<b> q;
D<b,3> r;
