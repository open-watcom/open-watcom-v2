// Borland barfs
template <class G>
    struct S {
	typedef int T;
	void foo( T );
    };
template <class G>
void ((((S<G>::foo))))( T += T )
{
}
