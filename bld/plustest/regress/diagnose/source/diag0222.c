// BADREF.C -- TEST ILLEGAL REFERENCE EXPRESSIONS
//
// 91/10/30     -- Ian McHardy          -- defined

class BASE
{
    public:
    int a;
};

class INHERIT:BASE
{
    public:
    int b;
};

void fbr(                       // FUNCTION WITH REF TO BASE PARAMETER
    BASE& br )
{
    br = br;
}

void fir(                       // FUNCTION WITH REF TO INHERIT PARAMETER
    INHERIT& ir )
{
    ir = ir;
}



void main( void )
{
    BASE b;
    INHERIT i;

    BASE& br;
    INHERIT& ir;

    INHERIT& ir2 = b;		// ERROR
    void& bad_ref_void;         // ERROR

    br = i;                     // ERROR: BASE is a private base of INHERIT
    ir = i;                     // OK

    fir( br );                  // ERROR
    fir( b );                   // ERROR
    ir = b;                     // ERROR
}
