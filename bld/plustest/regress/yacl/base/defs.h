

#ifndef _defs_h_
#define _defs_h_





/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */



// Check that a platform is defined:
#if !defined(__UNIX__) && !defined(WINDOWS)
#if !defined(MSDOS) && !defined(__DOS__)
#if !defined(__OS2__)
#error (defs.h) Unsupported platform: define __UNIX__, WINDOWS, MSDOS or __OS2__
#endif
#endif
#endif

#ifdef __GNUC__
#pragma interface
#endif

#ifdef __BORLANDC__
#pragma warn -inl // No warnings about inline functions
#endif


// -------------------- Essential typedefs -----------------------

typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned char  uchar;
typedef void*          CL_VoidPtr;
#if !defined(__GNUC__) || (__GNUC_MINOR__ <= 5)
#define bool BOOL
typedef uchar          bool;
#endif



// ---------------------- #define'd symbols -----------------------


#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef __CLASSTYPE /* To allow for Windows-style exports */
#define __CLASSTYPE
#endif

#if defined (__MSDOS__) || defined (__MS_WINDOWS__)
#define __FAR far
#else
#define __FAR
#endif

#ifndef NO_DEBUG
#include "base/error.h"
#define assert(cond,params) if (!(cond)) CL_Error::Fatal params
#else
#define assert(cond, params)
#endif


#ifndef NULL
#define NULL  0L
#endif





// ------------------------- Inline functions -------------------------

#ifdef __GNUC__
#pragma implementation // Force code generation for minl and maxl
#endif

inline long minl (long x, long y)
{
    return (x < y) ? x : y;
}


inline long maxl (long x, long y)
{
    return (x > y) ? x : y;
}



#endif


