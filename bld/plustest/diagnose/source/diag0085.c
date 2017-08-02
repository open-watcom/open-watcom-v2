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
#if 0   // control presence of explicit cast
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
