#include "fail.h"

template< class T >
struct A
{
    typedef A AA;

    A();
    A( const AA &a );

    ~A();

    bool operator ! ();

    operator bool ();

    void f( const AA &a );

    struct B {
        B();

        ~B();

        void g();
    };
};


template< class T >
inline A< T >::A()
{ }

template< class T >
inline A< T >::A( const AA & )
{ }

template< class T >
inline A< T >::~A()
{ }

template< class T >
inline bool A< T >::operator ! ()
{
    return true;
}

template< class T >
inline A< T >::operator bool ()
{
    return true;
}

template< class T >
inline void A< T >::f( const AA & )
{ }


template< typename T >
A< T >::B::B( )
{ }

template< typename T >
A< T >::B::~B( )
{ }

template< typename T >
void A< T >::B::g( )
{ }


int main()
{
    A< int > a1;
    a1.f( a1 );

    if( !( !a1 ) ) fail( __LINE__ );
    if( !static_cast< bool >( a1 ) ) fail( __LINE__ );


    A< int > a2( a1 );
    a2.f( a2 );

    if( !( !a2 ) ) fail( __LINE__ );
    if( !static_cast< bool >( a2 ) ) fail( __LINE__ );


    A< int >::B b;
    b.g();


    _PASS;
}
