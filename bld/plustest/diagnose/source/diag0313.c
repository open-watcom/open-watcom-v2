template <class T>
class A
{
    A( );
};

template <class T>
A< class T >::A( )
{ }

template <class T>
A< int >::A( )
{ }

template <class T>
A< short >::A( )
{ }
