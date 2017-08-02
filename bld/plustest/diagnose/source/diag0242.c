// DIAG0242.c -- preprocessor test for constant expressions
//
//

// defined

#define a 5

#if 03412.231 < 1
#endif

#if sizeof(b)
#endif

#if defined ( asdf
#endif

#if 5 < 6 )
#endif

#if ()
#endif

#if (5
#endif

#if 5)
#endif

#if defined( a ) defined (b)
#endif

// unary

#if --a == +a
#endif

#if 4~ < 0
#endif

// binary

#if a*=b
#endif

#define b 3

#if a <
#endif

#if > b (0-4)
#endif

#if (1==2) b >=
#endif

#if a < = b
#endif

// conditional

#if a ? b
#endif

#if a : b 
#endif

#if a : b ? c
#endif

#if a ? b ? c
#endif

#if ? b : c
#endif

#if a ? : c
#endif

#if a ? b :
#endif

#if a : b : c
#endif

#if (1==2) b ? : c
#endif

#if (1==4) b : c
#endif

#if a ? : b c
#endif

main()
{
}
