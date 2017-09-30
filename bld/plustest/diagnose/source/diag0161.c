template<class P>
    struct B {
    };

template<class P>
    struct D : B<P> {
    };

template<class P>
    D<P> func(const B<P>& src)
    {
	D<P> dest;
	func(src, dest);
	func(src, dest, 0, P(0));
	return dest;
    }

template<class S, class X>
    void func(B<S>, D<X>, int, S)
    {
    }

template<class S, class X>
    inline void func(const B<S>& src, const D<X>& dest)
    {
	func(src, dest, 0, S(0));
    }

extern D<int> func(const B<int>&);

#error at least one error
