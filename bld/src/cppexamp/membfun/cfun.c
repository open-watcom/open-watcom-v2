// CFUN.C
//
// These two functions call the member functions in T.
//

int FOO_FUN( void*, int, int );
int BAR_FUN( int, int );

#pragma aux FOO_FUN "FOO";  // these set the same names as were used in
#pragma aux BAR_FUN "BAR";  // the C++ main-line

int CfunFoo( void* a1, int a2, int a3 )
{
    return FOO_FUN( a1, a2, a3 );
}


int CfunBar( char c1, char c2 )
{
    return BAR_FUN( c1, c2 );
}
