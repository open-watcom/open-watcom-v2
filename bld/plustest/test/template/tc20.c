// parsed by MetaWare
template <class G>
    struct S {
	typedef int T;
	static T foo;
    };
template <class G>
int S<G>::foo = 1;
