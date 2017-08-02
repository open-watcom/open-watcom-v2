struct S {};
int comp(S const &,S const &) {
    return 0;
}
template <class T,int size>
    class xyz {};

template <class T>
    void foo( xyz<T,-1>, int(*)( const T &, const T & ) );

void bar( xyz<S,1> xx ) {
    foo( xx, comp );
}
