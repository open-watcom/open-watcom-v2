// [] -> * transformation combined with const
// what should we do?

//              errors
//              ======
// MS
// Zortech
// MetaWare     ++x
// Borland      *x and ++x
// WATCOM       *x
// CFRONT       *x
// GNU          *x

typedef char A[32];

void foo( const A x )
{
    *x = 'a';   // const array contents?
    ++x;        // const array pointer?
}
