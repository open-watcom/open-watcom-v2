/* Generate T x T x T matrix for binary operator tests, using
 * integer and floating point types.
 */

#define FN_SET_1( T1, T2 )  \
    ONE_FN( T1, T2, U1 )    \
    ONE_FN( T1, T2, I1 )    \
    ONE_FN( T1, T2, U2 )    \
    ONE_FN( T1, T2, I2 )    \
    ONE_FN( T1, T2, U4 )    \
    ONE_FN( T1, T2, I4 )    \
    ONE_FN( T1, T2, U8 )    \
    ONE_FN( T1, T2, I8 )    \
    ONE_FN( T1, T2, FS )    \
    ONE_FN( T1, T2, FD )    \
    ONE_FN( T1, T2, FL )

#define FN_SET_2( T1 )  \
    FN_SET_1( T1, U1 )  \
    FN_SET_1( T1, I1 )  \
    FN_SET_1( T1, U2 )  \
    FN_SET_1( T1, I2 )  \
    FN_SET_1( T1, U4 )  \
    FN_SET_1( T1, I4 )  \
    FN_SET_1( T1, U8 )  \
    FN_SET_1( T1, I8 )  \
    FN_SET_1( T1, FS )  \
    FN_SET_1( T1, FD )  \
    FN_SET_1( T1, FL )

#if defined(TREE_PARTS)

#if defined(THIRD_PART)

#define GEN_TESTS   \
    FN_SET_2( FS )  \
    FN_SET_2( FD )  \
    FN_SET_2( FL )

#elif defined(SECOND_PART)

#define GEN_TESTS   \
    FN_SET_2( U4 )  \
    FN_SET_2( I4 )  \
    FN_SET_2( U8 )  \
    FN_SET_2( I8 )

#else

#define GEN_TESTS   \
    FN_SET_2( U1 )  \
    FN_SET_2( I1 )  \
    FN_SET_2( U2 )  \
    FN_SET_2( I2 )

#endif

#else

#define GEN_TESTS   \
    FN_SET_2( U1 )  \
    FN_SET_2( I1 )  \
    FN_SET_2( U2 )  \
    FN_SET_2( I2 )  \
    FN_SET_2( U4 )  \
    FN_SET_2( I4 )  \
    FN_SET_2( U8 )  \
    FN_SET_2( I8 )  \
    FN_SET_2( FS )  \
    FN_SET_2( FD )  \
    FN_SET_2( FL )

#endif
