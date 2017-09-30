#include "fail.h"

// TSTSCOP3.C -- TEST NESTED CLASS SCOPE
//
// 91/10/25	-- Ian McHardy		-- defined

// class structure built:( "[ class_name" means class is nested within above
//			  class)
// 	T( a, c ) -----\
//	|              |
//	C( c )         D( c )
//	| [  D( c )     [ T( d )


class T
{
public:
    int a;
    int *c;
};

class C:public T
{
public:
    class D
    {
    public:
	float c;
    };
    char * *c;
};

class D:public T
{
    public: float c;
    class T
    {
	public: char *d;
    };
};

int main( void ){
    C c;
    C::D cd;
    D d;
    int a;
    char *char_ptr;


    a = 12;
    char_ptr = "Hello";
    c.c = &char_ptr;
    d.c = .5;

    c.a = 1;
    c.T::c = &a;
    cd.c = 12.7;

    d.::T::c = &a;
    if( c.c != &char_ptr ) fail(__LINE__);
    if( d.c != .5 ) fail(__LINE__);
    _PASS;
}
