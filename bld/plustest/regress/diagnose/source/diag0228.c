// BADDTOR.C -- CHECK ERROR CASES OF CHAPTER 8, DECLATORS IN C++ Ref
//
// 91/10/29	-- Ian McHardy		-- defined

class with_static_a{ public: static int a; };

#define PTR_TO_MEMBER		// pointer to members implemented

void main( void )
{

// 8.2
    int ok_int,			// OK
        ok_int2,		// OK
        auto bad_auto_i;	// cannot int <qualifier>
    int	static bad_static_i;	// ditto
    int	extern bad_extern_i;	// ditto
    int	register bad_register_i;	// ditto
    int	friend bad_friend_i;	// ditto
    int	inline bad_inline_i( void );	// ditto
    int	virtual bad_virtual_i( void );	// ditto
    int	typedef bad_typedef_i;	// ditto

// 8.2.1
    const ci = 10, *pc = &ci, *const cpc = pc;	// OK
    int i, *p, *const cp = &i;	// OK

    ci = 1;			// modifying a constant value
    ci++;			// ditto
    *pc = 2;			// ditto
    cp = &ci;			// ditto
    cpc++;			// ditto
    p = pc;			// ditto

#ifdef PTR_TO_MEMBER
// 8.2.3
    with_static_a wsa;		// OK
    with_static_a *wsap;
    int with_static_a::*pmi;

    pmi = &with_static_a::a;	// cannot point to static member
    wsa.*pmi = 7;		// cannot access a static member
    wsap = &wsa;		// OK
    was->*pmi = 8;		// cannot access a static member
#endif

// 8.2.4

    double bad_doub_array[ 0 ];	// must have subscript > 0
    char * bad_char_ptr_2d[ 12 ][ -1 ][ 7 ];	// must have subscript > 0
    void bad_void_array[ 3 ];	// cannot have array of void

// 8.2.5
    typedef int function( int a );// OK
    function funct_array[ 5 ];	// cannot have an array of functions
}

typedef char char_array[ 5 ];	// OK

char_array bad_funct1()		// test for 8.2.5: cannot return array
{
    char_array a;
    a[ 3 ] = 3;
    return( a );		// cannot return array
}

typedef int function( int a ); 	// OK

function bad_funct2()		// test for 8.2.5: cannot return function
{
    return( int b( int a ){ return( a ); }; );// cannot return function
}

void bad_funct3( struct S{ int a, b; }arg ) // test for 8.2.5: types may not
{
    arg = arg;			// be defined as argument types
}

enum E{ A, B, C }bad_funct4()	// test for 8.2.5: ditto
{
    return A;			// ditto
}

void good_function( int ){};	// should be legal (8.2.5)
