// Borland barfs
// MetaWare barfs
template <class G>
    struct S {
	typedef G T;
	void foo( T * );
    };
template <class G>
void S<G>::foo( T * )
{
}
