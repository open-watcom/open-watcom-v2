// conversion operators must be parseable without proper tokenization
// this also is applicable to "." and "->" calls
template <int size> class C
{
    typedef int T;
    operator T();
};
template <int size>
    C<size>::operator T******()
    {
    }
