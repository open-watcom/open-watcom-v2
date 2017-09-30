// Encapsulation of CAST and non-CAST conversion of various forms
// of 'this' pointers to various forms of 'void *'
// The compiler says that none of these are legal
// I think the draft standard says that it should be as marked
// bad - means won't work without the cast, will work with the cast
// GRB
// My interpretation of the conversions is implemented w.r.t. conversions.
// See EXPRERRC.C in EXPRERR.TST.
// The philosophy, in general, is that CONST can be added at will but cannot
// be taken away. VOLATILE is ignored, except at code generation. We should
// try this with MicroSoft and Metaware compilers.
// JWW
#if 1   // control presence of explicit cast
#define CAST   ( void                * )
#define CASTC  ( void const          * )
#define CASTV  ( void       volatile * )
#define CASTCV ( void const volatile * )
#else
#define CAST
#define CASTC
#define CASTV
#define CASTCV
#endif

extern void hit  ( void                * );
extern void hitc ( void const          * );
extern void hitv ( void       volatile * );
extern void hitcv( void const volatile * );
struct A {
    void foo  ( void );
    void fooc ( void ) const;
    void foov ( void )       volatile;
    void foocv( void ) const volatile;
};
void A::foo( void ) {           // GRB  BCC TCC GNU JWW MSC MET
    hit  ( this );              //
    hit  ( CAST this );         //
    hitc ( CASTC this );        //
    hitv ( CASTV this );        //
    hitcv( CASTCV this );       //
}
void A::fooc( void ) const {
    hit  ( CAST this );         // bad  bad bad bad bad bad bad
    hitc ( this );              //
    hitc ( CASTC this );        //
    hitv ( CASTV this );        // bad  bad bad bad bad bad bad
    hitcv( CASTCV this );       //
}
void A::foov( void ) volatile {
    hit  ( CAST this );         // bad  bad     bad     bad bad
    hitc ( CASTC this );        // bad  bad     bad     bad bad
    hitv ( this );              //
    hitv ( CASTV this );        //
    hitcv( CASTCV this );       //
}
void A::foocv( void ) const volatile {
    hit  ( CAST this );         // bad  bad bad bad bad bad bad
    hitc ( CASTC this );        // bad  bad     bad     bad bad
    hitv ( CASTV this );        // bad  bad bad bad bad bad bad
    hitcv( this );              //
    hitcv( CASTCV this );       //
}
#error at least one error for DIAGNOSE test stream

// test overloading with const volatile this pointer across derivations
// in each of the following 'foo_..' functions, the selection of
// S::operator[] is driven by the modifiers on the 'this' parameter

struct S {
                   int & operator[](int i)               ;
    const          int & operator[](int i) const         ;
          volatile int & operator[](int i)       volatile;
    const volatile int & operator[](int i) const volatile;
};
struct T : S {
};

int &foo_c( S const &x )
{
    return( x[1] ); // cannot strip const
}

int &foo_v( S volatile &y )
{
    return( y[1] ); // cannot strip volatile
}

int &foo_cv( S const volatile &z )
{
    return( z[1] ); // cannot strip const or volatile
}

int &foo_c( T const &x )
{
    return( x[1] ); // cannot strip const
}

int &foo_v( T volatile &y )
{
    return( y[1] ); // cannot strip volatile
}

int &foo_cv( T const volatile &z )
{
    return( z[1] ); // cannot strip const or volatile
}
