// diagnose some dangling references

struct S {
    S( int );
    S( S const & );
    ~S();
};

S const & foo( int x )
{
    return S(x);                // dangling reference!
}

S ack( int x )
{
    return S(x);
}

S const & foo( char x )
{
    return ack(x);              // dangling reference!
}

typedef S (*PF)( int );

S const & goo1( PF pf, int x )
{
    return pf( x );             // dangling reference!
}

S const & goo2( PF pf, int x )
{
    return (*pf)( x );          // dangling reference!
}

S const goo3( PF pf, int x )
{
    return pf( x );
}

S const goo4( PF pf, int x )
{
    return (*pf)( x );
}
