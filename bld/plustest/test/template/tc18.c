// Borland barfs
template <class G>
    struct S {
	typedef int T;
	void foo( int ****(T,T) );
    };
template <class G>
void S<G>::foo( int ****(T,T) )
{
}
