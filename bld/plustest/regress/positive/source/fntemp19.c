// reported by Tikhonov Victor in Message-ID: <eqjlol$33o$1@www.openwatcom.org>
#include "fail.h"

template< typename T >
struct Base;

template< typename T >
struct Base< T() >
{ };

template< typename R >
struct Derived
  : public Base< R() >
{ };

struct V
{
    int f()
    {
        return 0;
    }
};

void g( const Base< int() > & )
{ }

template< typename T, typename R >
Derived< R > h( R ( T::* )() )
{
    return Derived< R >();
}

int main()
{
    g( h( &V::f ) );

    _PASS;
}
