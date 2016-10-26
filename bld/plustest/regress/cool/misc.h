//
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// This file  contains  useful definitions,  macros, and constants used through
// out most header and source files.
//
// Updated: JAM 08/10/92 -- removed DOS definitions/specifics
// Updated: JAM 08/11/92 -- added BITS definition
// Updated: JAM 08/11/92 -- added <stdio.h> for printf() during error handling
// Updated: JAM 08/12/92 -- made to use <limits.h> and <float.h> instead of
//                          <values.h> for increased standards compliance
//                          (and to fix a BC++ bug with MAXDOUBLE)

#ifndef MISCELANEOUSH                           // If no miscelaneous header
#define MISCELANEOUSH

#include <limits.h>             // platform specific constants
#include <float.h>              // platform specific constants
//#include <values.h>           // platform specific constants (deprecated)
#undef MAXDOUBLE  //## temporary hack for BC++ bug until compiled, then fix
#define MAXFLOAT FLT_MAX
#define MAXDOUBLE DBL_MAX
#define MINFLOAT FLT_MIN
#define MINDOUBLE DBL_MIN
#include <stdio.h>              // because temporarily using printf/abort for errors
#include <stdlib.h>             // because temporarily using printf/abort for errors

#ifndef BITS   // at least BC++'s <value.h> didn't have it
#define BITS(type) (CHAR_BIT*(int)sizeof(type))
#endif

#ifndef DEFSH
#include <cool/defs.h>                          // Include the defs header
#endif

#ifndef INVALID                                 // Define INVALID for curpos
#define INVALID (-1)
#endif

#ifndef END_OF_STRING                           // If END_OF_STRING not defined
#define END_OF_STRING (0)
#endif

#ifndef NEWLINE                                 // If Newline char not defined
#define NEWLINE '\n'
#endif

#ifndef SENSITIVE                               // If case flags not defined
#define SENSITIVE TRUE
#define INSENSITIVE FALSE
#endif

#ifndef NUMBER_STATES
#define NUMBER_STATES
enum N_status { N_OK, N_MINUS_INFINITY, N_PLUS_INFINITY, N_OVERFLOW,
                N_UNDERFLOW, N_NO_CONVERSION, N_DIVIDE_BY_ZERO };
#endif

// use of these deprecated in favor of standard <limits.h> macros
#undef MINSHORT
#undef MININT
#undef MINLONG
#undef MAXSHORT
#undef MAXINT
#undef MAXLONG
#define MINSHORT SHRT_MIN
#define MININT INT_MIN
#define MINLONG LONG_MIN
#define MAXSHORT SHRT_MAX
#define MAXINT INT_MAX
#define MAXLONG LONG_MAX

/*
  In values.h
#define MAXDOUBLE       1.79769313486231470e+308
#define MAXFLOAT        ((float)3.40282346638528860e+38)
#define MINDOUBLE       4.94065645841246544e-324
#define MINFLOAT        ((float)1.40129846432481707e-45)
#define DMINEXP (-(DMAXEXP + DSIGNIF - _HIDDENBIT - 3))
#define FMINEXP (-(FMAXEXP + FSIGNIF - _HIDDENBIT - 3))
#define _IEEE           1
#define _DEXPLEN        11
#define _HIDDENBIT      1
*/

#ifndef ABS
#define ABS(x) (((x) >= 0) ? (x) : (-(x)))
#endif

// even --  Determine if long integer is odd of even
// Input:   long integer
// Output:  Boolean TRUE/FALSE

inline Boolean even (long n) {
  return ((n & 1) ? FALSE : TRUE);
}

// odd --  Determine if long integer is odd of even
// Input:  long integer
// Output: Boolean TRUE/FALSE

inline Boolean odd (long n) {
  return ((n & 1) ? TRUE : FALSE);
}

// The "#pragma defmacro" is a COOL extension to the standard  ANSI C processor
// that allows  a programmer to  define macro  extensions to the  language. All
// COOL  macros   have  been  incorporated  into  the preprocessor   itself  to
// facilitate extra speed and efficiency. User defined  extensions are searched
// for on the include file path.

#pragma defmacro MACRO "macro" delimiter=} recursive
#pragma defmacro template "template" delimiter=}
#pragma defmacro DECLARE "declare" delimiter=> recursive lines
#pragma defmacro IMPLEMENT "implement" delimiter=> recursive lines

#endif

