// updated template class explicit instatiation syntax

template< class T >
class X{
public:
    void fn( T x );
    T v;
};

template< class T >
void X<T>::fn( T x )
{
    v = x+2;
}

template< class T >
class Y{
public:
    T fnb( T x );
};

template< class T >
T Y<T>::fnb( T x )
{
    return( x*x );
}

// explicitly instatiate, they are used in cltemp21.c
template class X< long >;
template struct Y< double >;

// more explicit template type stuff can go here once it is implemented...
// template function
// template member function
