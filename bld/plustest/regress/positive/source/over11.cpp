#include "fail.h"

struct XS {
    char *p;
    XS( char * );
    XS( XS const & );
};
XS::XS( char *p )
    : p(p)
    {
}
XS::XS( XS const &s )
    : p(s.p)
    {
}

struct E {
    char *p;
    E( XS const & );
    E( E const & );
};

E::E( XS const &s ) : p( s.p ) {
}
E::E( E const &s ) : p( s.p ) {
}

struct D : E {
    D();
};

D::D() : E( __FILE__ ) {
}

D x;

int main() {
    if( x.p == NULL || strcmp( x.p, __FILE__ ) != 0 ) fail(__LINE__);
    _PASS;
}
