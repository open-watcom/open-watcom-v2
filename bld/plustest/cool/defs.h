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
// This file contains common definitions, macros, and constants used by header
// and source files under ICE.
//
// Updated: JAM 08/10/92 -- removed NULL and 'overload min/max'
// Updated: JAM 08/10/92 -- now doesn't expect DOS <stdlib.h> to define min/max

#ifndef DEFSH                                   // If no defs header file
#define DEFSH

#ifndef BOOLEANH                                // If Boolean type not defined
#define BOOLEANH
typedef int Boolean;                            // Define Boolean data type
#endif

#undef TRUE                                     // ensure TRUE is defined
#define TRUE (1)

#undef FALSE                                    // ensure FALSE is defined
#define FALSE (0)

#ifdef IV_INSTALLED            // Interviews define these already
#include <InterViews/defs.h>
#else
// min --  Return the minimum of two long integers
// Input:  Two long integers
// Output: Smallest of two longs

inline char min (char a, char b) {return (a < b) ? a : b;}
inline int min (int a, int b) {return (a < b) ? a : b;}
inline long min (long a, long b) {return (a < b) ? a : b;}
inline double min (double a, double b) {return (a < b) ? a : b;}

// max --  Return the maximum of two long integers
// Input:  Two long integers
// Output: Largest of two longs

inline char max(char a, char b) {return (a > b) ? a : b;}
inline int max(int a, int b) {return (a > b) ? a : b;}
inline long max(long a, long b) {return (a > b) ? a : b;}
inline double max(double a, double b) {return (a > b) ? a : b;}
#endif   // IV_INSTALLED

#endif
