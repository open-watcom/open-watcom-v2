#include "fail.h"

struct V {
    V & operator =( const V &src );
};

struct R : virtual V {
    int r;
};

struct S : virtual V {
    int s;
};

struct T : R, S {
    int t;
};

unsigned assignments;

V & V::operator =( const V &src ) {
    ++assignments;
    return *this;
}

void assign_R( R &s, R &d )
{
    s = d;
}

void assign_S( S &s, S &d )
{
    s = d;
}

void assign_T( T &s, T &d )
{
    s = d;
}
    
T t;

int main()
{
    assign_R( t,t );
    assign_S( t,t );
    assign_T( t,t );
    if( assignments != 3 ) fail(__LINE__);
    _PASS;
}
