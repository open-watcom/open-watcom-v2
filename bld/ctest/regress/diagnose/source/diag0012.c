/* Test pointer assignment warnings, especially null pointer assignments
 */


/* A litle extract from Windows 3.x headers */
#define NEAR                    __near
#define DECLARE_HANDLE(name)    struct name##__ { int unused; }; \
                                typedef const struct name##__ NEAR* name
DECLARE_HANDLE(HMENU);


#define NVOID   (void __near *)
#define FVOID   (void __far *)

/* Check pointers to array handling, too */
typedef int ia_t[3];
typedef int iaa_t[3][3];

int ai1[] = { 1, 2 };
int ai2[] = { 1, 2, 3 };
int ai3[3] = { 1, 2, 3 };
int aai1[][3] = { { 1, 2, 3 }, { 4, 5, 6 } };
int aai2[][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
int aai3[3][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };

void foo( HMENU *hm )
{
}

int main( int argc, char **argv )
{
    HMENU   hm;
    ia_t    *pai;
    iaa_t   *paai;

    hm = 0;                 // OK
    hm = 0L;                // OK
    hm = NVOID 0;           // OK
    hm = NVOID (1 - 1);     // OK
    hm = NVOID 1;           // OK
    hm = FVOID 0;           // OK - in MSC since 6.0
    hm = FVOID (1 - 1);     // OK - also null pointer constant
    hm = FVOID 1;           // pointer truncated

    foo( 0 );               // OK
    foo( 0L );              // OK
    foo( NVOID 0 );         // OK
    foo( NVOID (1 - 1) );   // OK
    foo( NVOID 1 );         // OK
    foo( FVOID 0 );         // OK - in MSC since 6.0
    foo( FVOID (1 - 1) );   // OK - also null pointer constant
    foo( FVOID 1 );         // pointer truncated

    pai = &ai1;             // array sizes don't match
    pai = &ai2;             // OK
    pai = &ai3;             // OK
    paai = &aai1;           // array sizes don't match
    paai = &aai2;           // OK
    paai = &aai3;           // OK

    return( 0 );
}
