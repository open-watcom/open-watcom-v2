struct S9{
    int a;
    int b( int );
};

void main( void )
{
    const S9 cs9 = { 0 };
    volatile S9 vs9;

    // function:        b( S9 *, int )
    // passed:           ( S9 const *, int )
    cs9.b( 5 );                                 // should be error
    // function:        b( S9 *, int )
    // passed:           ( S9 volatile *, int )
    vs9.b( 7 );                                 // is ok
}
