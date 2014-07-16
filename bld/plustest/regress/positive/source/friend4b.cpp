
template<class T>
class B{
protected:
    T i;
    template< class U >
    friend U fn(const B<U>&);
};

class A : public B< int > {
public:
    A( int ii ) { i = ii;}
};

template< class T >
T fn( const B<T>& b )
{
    return b.i;
}

int x()
{
    A a(8);
    return fn(a);
}
