struct S {
    int *ret_c1( int x )
    {
	static int COMDAT1;
	COMDAT1 = x;
	return &COMDAT1;
    }
};

static int *ret_c1( int x )
{
    static S __dummy;
    return __dummy.ret_c1( x );
}

static int *ret_s1( int x )
{
    static int STATIC1;

    STATIC1 = x;
    return &STATIC1;
}

inline int *ret_c2( int x )
{
    static int COMDAT2;

    COMDAT2 = x;
    return &COMDAT2;
}

static inline int *ret_s2( int x )
{
    static int STATIC2 = '2';

    STATIC2 = x;
    return &STATIC2;
}
