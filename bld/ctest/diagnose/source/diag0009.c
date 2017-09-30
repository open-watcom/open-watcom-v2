/* Check bitfield size restrictions */

struct S1 {
    char    f0 : 5;
    char    f1 : 3;
    long    f2 : 20;
    long    f3 : 20;
};

struct S1 s1 = {
    0x20,
    0x07,
    0x100000,
    0x07ffff
};

void asgn( void )
{
    s1.f0 = 0x20;
    s1.f0 = 0x1F + 1;
    s1.f0 = 0x1f;
    s1.f2 = 0x080000;
    s1.f2 = 0x100000L;
}

int comp( void )
{
    // TODO: this should check for exact bitfield size,
    // not just the size of underlying type
    if( s1.f0 < 0x20 )
        return( 0 );
    if( s1.f0 < 0x100 )
        return( 0 );
    else if( s1.f2 < 0x100000 )
        return( 0 );
    else if( s1.f2 < 0x100000000 )
        return( 0 );
    return( 1 );
}
