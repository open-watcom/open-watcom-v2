#include "dump.h"

struct A {
    A() {};
    ~A() {};
    void g( int );
    void g( int ) const;
    void h( int ) const;
    operator int();
    operator int() const;
};
void A::g( int )	BAD;
void A::g( int ) const	GOOD;
void A::h( int ) const  GOOD;
A::operator int()	{ BAD; return 0; };
A::operator int() const { GOOD; return 0; };

void foo( int ) GOOD;
void bar( A const & a ) {
    a.g( 1 );   //        g(int)        -> rank:(exact, [NO_MATCH] )
		// select g(int) const  -> rank:(exact, [EXACT] )
    a.h( 1 );   // select h(int) const  -> rank:(exact, [EXACT] )
    foo( a );	// select op int() const-> rank:([EXACT] )
		//        op int()      -> rank:([NO_MATCH] )
}
int main( void ) {
    A a;
    bar( a );
    CHECK_GOOD( 61 );
    return errors != 0;
}
