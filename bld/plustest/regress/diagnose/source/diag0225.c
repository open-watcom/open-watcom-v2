// BADEXPR.C    -- CHECK FOR ERRORS IN EXPRESSIONS
//
// 91/09/16     -- Ian McHardy          -- defined

#define COLON_COLON           // :: defined
#define PTR_TO_MEM            // pointers to members defined
#define INLINE                // inline functions allowed
#define MEM_FUNC              // member functions

class complex{
    float real;
    float imaginary;
};

void function( char *string )
{
    string = string;
}

void function1( char *string )
{
    string = string;
}

// 5.2.2

void f5_2_2( const int ci, int *const icp ){
    int i;

    ci++;                       // changing const
    (*icp)++;                     // OK
    icp = &i;                   // changing const
}

class X5_2_2{ public: int i; }; // OK

X5_2_2 f5_2_2_2();

X5_2_2 *f5_2_2_2p();

// 5.4

class C5_4;

// 5.5

#ifdef PTR_TO_MEM

struct C5_5{
    int a;

#ifdef MEM_FUNCT
    int f( int a ){ a = a; };
#endif // MEM_FUNC
    enum{
        ZERO,
        ONE,
        TWO
    };

}

#endif

void main( void )
{
    int bi;
    float bf;
    float *pf;
    long double bld;
    double bd;
    long int *pli;
    long int bli;
    long int far *fpli;
    class complex bc;
    class complex *pc;
    void *pv;
typedef int int_type;

// 5.1
#ifdef COLON_COLON
    bli::a = 17;                // bli not a class
    pc::b = 18;                 // pc ditto
    int_type::c = 19;           // int_type ditto
    int::d = 20;                // int ditto
    function::e = 21;           // function ditto
    complex::f = 22;            // f not a member, not an lvalue
    complex::real = 23;         // not an lvalue
    bc::a = 24;                 // a is not a base of complex
#endif // COLON_COLON

// 5.2.1
    bli = 12;
    pli = &bli;
    bf = 1;
    bi = 1;
    bli = pli[ bf ];            // bf is not integral
    bli = bli[ bi ];            // neither bi or bli are pointer
    bli = function[ bi ];       // pointer to function cannot be used as array
    bli = bli[ function ];      // ditto

// 5.2.2
    f5_2_2_2().i = 1;           // return is not an lvalue
    f5_2_2_2p()->i = 1;         // OK
    bi = undeclared_function( bi, bli );// not declared

// 5.2.3
    bc++;                       // ++ not defined for class
    pv++;                       // cannot ++ void *, since no sizeof( void )
    bc--;                       // -- not def
    pv--;                       // cannot -- void *
    function++;                 // cannot ++ or -- function
    function--;

// 5.3
    bi = *bli;                  // bli is not a ptr
    bi = * *pli;                // ditto
    bi = &bli;                  // bi not an int long *
    bi = -pli;                  // cannot - ptr
    bc = !bc;                   // ! not defined on class
    bc = &bc;                   // cannot assign class * to bc
    bi = &function;             // cannot assign void (*)( char *)
    bc = *bc;                   // cannot * non_ptr
    bc = -bc;                   // cannot - class
    bf = ~bf;                   // cannot ~ float
    pli = ~pli;                 // cannot ~ ptr
    bli = ~function;            // cannot ~ function
    bi = & &bi;                 // & results in lvalue, operand must be rvalue
// 5.3.1
    ++pv;                       // cannot ++ or -- void *, class or function
    ++bc;                       // ditto
    --pv;                       // ditto
    --bc;                       // ditto
    ++function;                 // ditto
    --function;                 // ditto

// 5.3.2
    bi = sizeof( void );        // no such thing
    bli = sizeof( function );   // ditto

// 5.4
    void (*pf1)(char *);
    void (*pf2)(char *);
    class C5_4 c5_4;

    pli = pv;                   // not an implicid cast
    bi = (int)fpli;             // OK
    pf1 = &function;            // OK
    pf2 = pf1;                  // OK
    c5_4 = (class C5_4{ int a; int b; })c5_4;// cannot defined type in cast
    ( (int)bi )++;              // cast value not an lvalue

// 5.5

#ifdef PTR_TO_MEM
    {
        C5_5 c;

#ifdef MEM_FUNC
        int (C5_5::* pmf)( int ) = &C5_5::f;
        int (*pf)( int );
        pf = &( c.*pmf );       // can not take address of result
        pf = &( &c ->*pmf );    // ditto
#endif

        const int::* pmi = &C5_5::i;

        c.*pmi = 1;             // error: const
    }
#endif

// 5.6
    bli = pli * bli;            // pli is a ptr (illegal)
    bli = bli * pli;            // ditto
    bf = bf / pli;              // ditto
    bf = pli / bf;              // ditto
    bi = bf % bi;               // cannot use floats with %
    bi = bi % bf;               // ditto
    bi = pli % bi;              // cannot use ptrs with %
    bi = bi % pli;              // ditto

// 5.7
    bli = pv + bi;              // cannot add with void *
    bli = pli + pf;             // cannot add two ptrs
    bli = pf + pli;             // ditto
    bi = pf - pli;              // undefined behavior (it's an error)

// 5.8
    bi = bi << bf;              // cannot use floats or pointers
    bi = bf << bi;              // ditto
    bli = bld >> bli;           // ditto
    bli = bli >> bd;            // ditto
    bli = pli >> 2;             // ditto
    bli = 2 << pli;             // ditto

// 5.11
    bli = bi & bf;              // ditto
    bi = bd & bli;              // ditto
    bli = pf1 & bi;             // ditto
    bli = bli & pf;             // ditto

// 5.12
    bli = bi ^ bf;              // ditto
    bi = bd ^ bli;              // ditto
    bli = pf1 ^ bi;             // ditto
    bli = bli ^ pf;             // ditto

// 5.13
    bli = bi | bf;              // ditto
    bi = bd | bli;              // ditto
    bli = pf1 | bi;             // ditto
    bli = bli | pf;             // ditto

// 5.16
    bli = bi ? pv : bli;        // no conversion to a common type
    bli = bi ? function : bf;   // ditto
    bi = bc ? bi : bf;          // bc not arithmetic or ptr

// 5.17
    const int bci = 12;
    const float bcf = 3.14159;
    int const* pci = &bci;

    bci = bi;                   // make sure assignment to const is illegal
    bcf *= bf;                  // ditto
    bci /= bi;                  // ditto
    bci %= bi;                  // ditto
    pci += bi;                  // ditto
    pci -= bi;                  // ditto
    bci >>= bi;                 // ditto
    bci <<= bi;                 // ditto
    bci &= bi;                  // ditto
    bci ^= bi;                  // ditto
    bci |= bi;                  // ditto

    pli *= bi;                  // no ptrs or classes
    bi *= pli;                  // ditto
    bc *= bi;                   // ditto
    bi *= bc;                   // ditto

    pli /= bi;                  // no ptrs or classes
    bi /= pli;                  // ditto
    bc /= bi;                   // ditto
    bi /= bc;                   // ditto

    bc += bi;                   // no classes allowed
    bi += bc;                   // ditto

    bc -= bi;                   // ditto
    bi -= bc;                   // ditto

    bi %= bf;                   // no floats, ptrs, or classes allowed
    bf %= bi;                   // ditto
    bd %= bli;                  // ditto
    bli %= bd;                  // ditto
    pli %= bi;                  // ditto
    bi %= pli;                  // ditto
    bi %= bc;                   // ditto
    bc %= bi;                   // ditto

    bi >>= bf;                  // ditto
    bf >>= bi;                  // ditto
    bd >>= bli;                 // ditto
    bli >>= bd;                 // ditto
    pli >>= bi;                 // ditto
    bi >>= pli;                 // ditto
    bi >>= bc;                  // ditto
    bc >>= bi;                  // ditto

    bi <<= bf;                  // ditto
    bf <<= bi;                  // ditto
    bd <<= bli;                 // ditto
    bli <<= bd;                 // ditto
    pli <<= bi;                 // ditto
    bi <<= pli;                 // ditto
    bi <<= bc;                  // ditto
    bc <<= bi;                  // ditto

    bi &= bf;                   // ditto
    bf &= bi;                   // ditto
    bd &= bli;                  // ditto
    bli &= bd;                  // ditto
    pli &= bi;                  // ditto
    bi &= pli;                  // ditto
    bi &= bc;                   // ditto
    bc &= bi;                   // ditto

    bi ^= bf;                   // ditto
    bf ^= bi;                   // ditto
    bd ^= bli;                  // ditto
    bli ^= bd;                  // ditto
    pli ^= bi;                  // ditto
    bi ^= pli;                  // ditto
    bi ^= bc;                   // ditto
    bc ^= bi;                   // ditto

    bi |= bf;                   // ditto
    bf |= bi;                   // ditto
    bd |= bli;                  // ditto
    bli |= bd;                  // ditto
    pli |= bi;                  // ditto
    bi |= pli;                  // ditto
    bi |= bc;                   // ditto
    bc |= bi;                   // ditto

// 5.18
    ( bi, 3 ) = 7;              // 3 is not an lvalue
}
