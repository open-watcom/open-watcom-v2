#include "fail.h"

#if __WATCOM_REVISION >= 8
struct S {
    int & foo( int & i1 = j, int i2 = fn(), int i3= 4 );
    int goo( int & i1 = k, int i2 = fn(), int i3= 4 );
    static int & j;	// 4
    static int fn( void ) { return k; }; // 2
    static int k;	// 2
    int i;
    S(int in) { i = in;};
    int loo( S  s = 1 );
    int moo( S const & s = 1 ); 
};

int & S::j = m;
int S::k = n;


int S::loo( S s )
{
    return s.i;
}

int S::moo( S const & s )
{
    return s.i;
}

//             4          2
int & S::foo( int & i, int j, int i3 )
{
    return i; //4
}

//           4          2
int S::goo( int & i, int j, int i3 )
{
    return j; //2
}

struct Integer {
    Integer(int val) { _value = val; }
    ~Integer() { _value = 1234; }

    int _value;
};

int Bug(const Integer &x = 0)
{
    return x._value;
}

int main()
{
    S s(1);
    int & j = s.foo(); // calls foo(S::j,S::fn(),4)
    		       // which is foo(4,2,4) returning 4
    if( j != 4 ) fail(__LINE__);

    int k = s.goo(); // calls goo(S::j,S::fn(),4) 
    		      // which is goo(4,2,4) returning 2
    if( k != 2 ) fail(__LINE__);

    int i = s.loo();	// calls loo( S s = 1 );
    if( i != 1 ) fail(__LINE__);

    i = s.moo();	// calls moo( S const & s = 1 );
    if( i != 1 ) fail(__LINE__);

    i = Bug(1);
    if( i != 1 ) fail(__LINE__);
    i = Bug();
    if( i != 0 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
