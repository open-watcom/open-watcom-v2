struct INTER {
    INTER(int);
};

struct S {
    S( int );
    operator INTER();
};

struct T {
    T( INTER );
};

void foo( T );
void main()
{
    S s(2);	

    foo(s); 

    foo( S( 2 ) );
}


// this is to check that for copy initialization used in argument passing
// that src->inter->tgt via a ctor and a UDCF is not allowed
// (to match Microsoft)
