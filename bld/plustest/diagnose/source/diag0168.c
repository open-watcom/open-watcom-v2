#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

// test that autos are treated as __near
// compile -ml

struct S {
    int s;
};

void foo( int __near * );

int k;

S sf;

void goo( int i, S sp )
{
    S sa;
    
    foo( &i );          // ok
    foo( &sp.s );       // truncation (may be ok in the future)

    int j;
    foo( &j );          // ok
    foo( &sa.s );       // ok

    foo( &k );          // truncation
    foo( &sf.s );       // truncation

}

#endif
