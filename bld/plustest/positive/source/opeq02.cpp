#include "fail.h"
#include <stdio.h>


struct BP {
    int b;
    virtual BP & operator =( BP const & ) = 0;
    BP(int v) : b(v) {}
};
struct DP : BP {
    int d;
    virtual BP & operator =( BP const & );
    DP(int v) : BP(v), d(v) {}
};

BP & DP::operator =( BP const &s )
{
    if( this != &s ) {
	b = s.b + 1;
    }
    return *this;
}

struct B {
    int b;
    virtual B & operator =( B const & );
    B(int v) : b(v) {}
};

B & B::operator =( B const &s )
{
    if( this != &s ) {
	b = s.b - 1;
    }
    return *this;
}

struct D : B {
    int d;
    virtual B & operator =( B const & );
    D(int v) : B(v), d(v) {}
};

B & D::operator =( B const &s )
{
    if( this != &s ) {
	b = s.b + 1;
    }
    return *this;
}

void assign_D( D &d, D const &s )
{
    d = s;
}

void assign_DP( DP &d, DP const &s )
{
    d = s;
}

int main()
{
    DP x(1);
    DP nx(3);
    D *y = new D(3);
    D *ny = new D(5);

    assign_DP( nx, x );
    assign_DP( nx, nx );
    assign_D( *ny, *y );
    assign_D( *ny, *ny );
    if( x.b != 1 && x.d != 1 ) fail(__LINE__);
    if( nx.b != 2 && nx.d != 3 ) fail(__LINE__);
    if( y->b != 3 && y->d != 3 ) fail(__LINE__);
    if( ny->b != 4 && ny->d != 5 ) fail(__LINE__);
    _PASS;
}
