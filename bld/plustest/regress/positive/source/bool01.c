#include "fail.h"

#ifdef __WATCOM_BOOL__

unsigned const BOOL_TEST = 256+16;

char all_true[BOOL_TEST];

template <class T>
    struct S {
	T v;
	S( T v = 0 ) : v(v) {
	}
	operator T() const {
	    return v;
	}
    };

void check_true( bool v )
{
    if( v == false ) fail(__LINE__);
    if( v != true ) fail(__LINE__);
    if( (++v) == false ) fail(__LINE__);
    if( (v++) != true ) fail(__LINE__);
    if( (v++) != true ) fail(__LINE__);
}

bool always_false = false;

void check_false( bool v )
{
    if( v != false ) fail(__LINE__);
    if( v == true ) fail(__LINE__);
    if( (v++) != false ) fail(__LINE__);
    if( (v++) == false ) fail(__LINE__);
    v = always_false;
    if( (++v) == false ) fail(__LINE__);
    if( (++v) == false ) fail(__LINE__);
}

static bool v;

template <class T>
    bool check_return( T x )
    {
	T i = x;
	T v;
	v = i;
	return v;
    }

void check_ints()
{
    static bool inc_test = false;
    for( int i = 0; i < BOOL_TEST; ++i ) {
	S<int> cc(i);
	if( i != 0 ) {
	    check_true( i );
	    check_true( i != 0 );
	    v = i;
	    check_true( v );
	    bool q = i;
	    check_true( q );

	    {
		check_true( cc );
		v = cc;
		check_true( v );
		bool q = cc;
		check_true( q );
	    }

	    check_true( check_return( i ) );
	    check_true( check_return( cc ) );
	} else {
	    check_false( i );
	    v = i;
	    check_false( v );
	    bool q = i;
	    check_false( q );

	    {
		check_false( cc );
		v = cc;
		check_false( v );
		bool q = cc;
		check_false( q );
	    }

	    check_false( check_return( i ) );
	    check_false( check_return( cc ) );
	}
	++inc_test;
	check_true( inc_test );
    }
}

char *nullp;

void check_ptrs()
{
    char *p;

    for( p = all_true; p < &all_true[BOOL_TEST]; ++p ) {
	S<char *> cc(p);

	check_true( p );
	check_true( p != 0 );
	v = p;
	check_true( v );
	bool q = p;
	check_true( q );

	{
	    check_true( cc );
	    v = cc;
	    check_true( v );
	    bool q = cc;
	    check_true( q );
	}

	check_true( check_return( p ) );
	check_true( check_return( cc ) );
    }
    {
	S<char *> cc(nullp);

	check_false( nullp );
	v = nullp;
	check_false( v );
	bool q = nullp;
	check_false( q );

	{
	    check_false( cc );
	    v = cc;
	    check_false( v );
	    bool q = cc;
	    check_false( q );
	}

	check_false( check_return( nullp ) );
	check_false( check_return( cc ) );
    }
}

struct C {
    int a,b,c;
};

typedef int C::* MPC;

MPC mptrs[] = {
    0,
    &C::a,
    0,
    &C::b,
    0,
    &C::c,
    0,
    &C::a,
    0,
    &C::b,
    0,
    &C::c,
    0
};

void check_mptrs()
{
    MPC *p;

    for( p = mptrs; p < &mptrs[sizeof(mptrs)]; ++p ) {
	S<MPC> cc( *p );
	if( *p != 0 ) {
	    check_true( *p );
	    check_true( *p != 0 );
	    v = *p;
	    check_true( v );
	    bool q = *p;
	    check_true( q );

	    {
		check_true( cc );
		v = cc;
		check_true( v );
		bool q = cc;
		check_true( q );
	    }

	    check_true( check_return( *p ) );
	    check_true( check_return( cc ) );
	} else {
	    check_false( *p );
	    v = *p;
	    check_false( v );
	    bool q = *p;
	    check_false( v );

	    {
		check_false( cc );
		v = cc;
		check_false( v );
		bool q = cc;
		check_false( q );
	    }

	    check_false( check_return( *p ) );
	    check_false( check_return( cc ) );
	}
    }
}

void bool_result( int )
{
    fail(__LINE__);
}

void bool_result( bool v )
{
    check_true( v );
}

int zero;
int one = 1;
int two = 2;

void check_overs()
{
    bool_result( zero == 0 );
    bool_result( bool( one ) );
    bool_result( (bool) two );
    bool_result( one && two );
    bool_result( ! zero );
    bool_result( two || zero );
}

int main()
{
    check_ints();
    check_ptrs();
    check_mptrs();
    check_overs();
    _PASS;
}

#else

ALWAYS_PASS

#endif
