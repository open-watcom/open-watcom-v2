// is this allowed?
template <class T,T x, T y>
    struct S {
	T a[x];
	T b[y];
    };

S<int,1,2> zz;
