/* Generate T x T x T matrix for binary operator tests, using
 * integer types only.
 */

#define FN_SET_1( T1, T2 )  \
    ONE_FN( T1, T2, U1 )    \
    ONE_FN( T1, T2, I1 )    \
    ONE_FN( T1, T2, U2 )    \
    ONE_FN( T1, T2, I2 )    \
    ONE_FN( T1, T2, U4 )    \
    ONE_FN( T1, T2, I4 )    \
    ONE_FN( T1, T2, U8 )    \
    ONE_FN( T1, T2, I8 )

#define FN_SET_2( T1 )  \
    FN_SET_1( T1, U1 )  \
    FN_SET_1( T1, I1 )  \
    FN_SET_1( T1, U2 )  \
    FN_SET_1( T1, I2 )  \
    FN_SET_1( T1, U4 )  \
    FN_SET_1( T1, I4 )  \
    FN_SET_1( T1, U8 )  \
    FN_SET_1( T1, I8 )

#define GEN_TESTS   \
    FN_SET_2( U1 )  \
    FN_SET_2( I1 )  \
    FN_SET_2( U2 )  \
    FN_SET_2( I2 )  \
    FN_SET_2( U4 )  \
    FN_SET_2( I4 )  \
    FN_SET_2( U8 )  \
    FN_SET_2( I8 )

