struct S {
    S( int i );
};

struct INTER {
    INTER( S );
};

struct T {
    T( INTER );
};

void goo( T );

void main()
{
    S s(2);	
    goo( s ); 
    goo( S(2) );	
}

// this is to check that for copy initialization used in argument passing
// that src->inter->tgt via two ctors is not allowed (to match Microsoft)
