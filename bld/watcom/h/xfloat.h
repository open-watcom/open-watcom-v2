/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  C/C++ run-time library floating-point definitions.
*
****************************************************************************/


#ifndef _XFLOAT_H_INCLUDED
#define _XFLOAT_H_INCLUDED

#include <stddef.h>     // for wchar_t
#include <float.h>      // for LDBL_DIG

#ifndef _WMRTLINK
    #define _WMRTLINK   // This SUCKS!
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WATCOMC__ ) && defined( _M_IX86 )
 #define _LONG_DOUBLE_
#endif

#if defined( __WATCOMC__ )
typedef unsigned long   u4;
typedef long            i4;
#else
typedef unsigned int    u4;
typedef int             i4;
#endif

typedef struct {                // This layout matches Intel 8087
  #ifdef _LONG_DOUBLE_
    unsigned long low_word;     // - low word of fraction
    unsigned long high_word;    // - high word of fraction
    unsigned short exponent;    // - exponent and sign
  #else                         // use this for all other 32-bit RISC
    union {
        double  value;          // - double value
        u4      word[2];        // - so we can access bits
    } u;
  #endif
} long_double;

typedef struct {                // Layout of IEEE 754 double (FD)
    union {
        double  value;          // - double value
        u4      word[2];        // - so we can access bits
    } u;
} float_double;

typedef struct {                // Layout of IEEE 754 single (FS)
    union {
        float   value;          // - double value
        u4      word;           // - so we can access bits
    } u;
} float_single;

/* NB: The following values *must* match FP_ macros in math.h! */
enum    ld_classification {
    __ZERO      = 0,
    __DENORMAL  = 1,
    __NONZERO   = 2,
    __NAN       = 3,
    __INFINITY  = 4
};

enum    ldcvt_flags {
    E_FMT       = 0x0001,       // 'E' format
    F_FMT       = 0x0002,       // 'F' format
    G_FMT       = 0x0004,       // 'G' format
    F_CVT       = 0x0008,       // __cvt routine format rules
    F_DOT       = 0x0010,       // always put '.' in result
    LONG_DOUBLE = 0x0020,       // number is true long double
    NO_TRUNC    = 0x0040,       // always provide ndigits in buffer
    IN_CAPS     = 0x0080,       // 'inf'/'nan' is uppercased
    IS_INF_NAN  = 0x0100,       // number is inf/nan (output flag)
};

typedef struct cvt_info {
      int       ndigits;        // INPUT: number of digits
      int       scale;          // INPUT: FORTRAN scale factor
      int       flags;          // INPUT/OUTPUT: flags (see ldcvt_flags)
      int       expchar;        // INPUT: exponent character to use
      int       expwidth;       // INPUT/OUTPUT: number of exponent digits
      int       sign;           // OUTPUT: 0 => +ve; otherwise -ve
      int       decimal_place;  // OUTPUT: position of '.'
      int       n1;             // OUTPUT: number of leading characters
      int       nz1;            // OUTPUT: followed by this many '0's
      int       n2;             // OUTPUT: followed by these characters
      int       nz2;            // OUTPUT: followed by this many '0's
} CVT_INFO;

/* Depending on the target, some functions expect near pointer arguments
 * to be pointing into the stack segment, while in other cases they must
 * point into the data segment.
 */

#if !defined( _M_IX86 ) || defined( __FLAT__ )
typedef long_double                                     *ld_arg;
typedef double                                          *dbl_arg;
typedef float                                           *flt_arg;
typedef char                                            *buf_arg;
typedef void                                            *i8_arg;
typedef void                                            *u8_arg;
#else
typedef long_double __based( __segname( "_STACK" ) )    *ld_arg;
typedef double      __based( __segname( "_STACK" ) )    *dbl_arg;
typedef float       __based( __segname( "_STACK" ) )    *flt_arg;
typedef char        __based( __segname( "_STACK" ) )    *buf_arg;
typedef void        __based( __segname( "_STACK" ) )    *i8_arg;
typedef void        __based( __segname( "_STACK" ) )    *u8_arg;
#endif

_WMRTLINK extern void __LDcvt(
                         long_double *pld,      // pointer to long_double
                         CVT_INFO  *cvt,        // conversion info
                         char      *buf );      // buffer
#if defined( __WATCOMC__ )
_WMRTLINK extern int __Strtold(
                        const char *bufptr,
                        long_double *pld,
                        char **endptr );
_WMRTLINK extern int __wStrtold(
                        const wchar_t *bufptr,
                        long_double *pld,
                        wchar_t **endptr );
#endif
extern  int     __LDClass( long_double * );
extern  void    __ZBuf2LD( buf_arg, ld_arg );
extern  void    _LDScale10x( ld_arg, int );
#ifdef _LONG_DOUBLE_
extern  void    __iLDFD( ld_arg, dbl_arg );
extern  void    __iLDFS( ld_arg, flt_arg );
extern  void    __iFDLD( dbl_arg, ld_arg );
extern  void    __iFSLD( flt_arg, ld_arg );
extern  i4      __LDI4( ld_arg );
extern  void    __I4LD( i4, ld_arg );
extern  void    __U4LD( u4, ld_arg);
//The 64bit types change depending on what's being built.
//(u)int64* (un)signed_64* don't seem suitable, and we use void* instead.
extern  void    __LDI8( ld_arg, i8_arg );
extern  void    __I8LD( i8_arg, ld_arg );
extern  void    __U8LD( u8_arg, ld_arg );
extern  void    __FLDA( ld_arg, ld_arg, ld_arg );
extern  void    __FLDS( ld_arg, ld_arg, ld_arg );
extern  void    __FLDM( ld_arg, ld_arg, ld_arg );
extern  void    __FLDD( ld_arg, ld_arg, ld_arg );
extern  int     __FLDC( ld_arg, ld_arg );
#endif

#ifdef __WATCOMC__
#if defined(__386__)
 #pragma aux    __ZBuf2LD       "*"  parm caller [eax] [edx];
 #if defined(__FPI__)
  extern unsigned __Get87CW(void);
  extern void __Set87CW(unsigned short);
  #pragma aux   __Get87CW = \
                "push 0"\
        float   "fstcw [esp]"\
        float   "fwait"\
                "pop eax"\
                value [eax];
  #pragma aux   __Set87CW = \
                "push eax"\
        float   "fldcw [esp]"\
                "pop eax"\
                parm caller [eax];
  #pragma aux   __FLDA = \
        float   "fld tbyte ptr [eax]"\
        float   "fld tbyte ptr [edx]"\
        float   "fadd"\
        float   "fstp tbyte ptr [ebx]"\
                parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDS = \
        float   "fld tbyte ptr [eax]"\
        float   "fld tbyte ptr [edx]"\
        float   "fsub"\
        float   "fstp tbyte ptr [ebx]"\
                parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDM = \
        float   "fld tbyte ptr [eax]"\
        float   "fld tbyte ptr [edx]"\
        float   "fmul"\
        float   "fstp tbyte ptr [ebx]"\
                parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDD = \
        float   "fld tbyte ptr [eax]"\
        float   "fld tbyte ptr [edx]"\
        float   "fdiv"\
        float   "fstp tbyte ptr [ebx]"\
                parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDC = \
                /* ST(1) */\
        float   "fld tbyte ptr [edx]"\
                /* ST(0) */\
        float   "fld tbyte ptr [eax]"\
                /* compare ST(0) with ST(1) */\
        float   "fcompp"\
        float   "fstsw  ax"\
                "sahf"\
                "sbb  edx,edx"\
                "shl  edx,1"\
                "shl  ah,2"\
                "cmc"\
                "adc  edx,0"\
                /* edx will be -1,0,+1 if [eax] <, ==, > [edx] */\
                parm caller [eax] [edx] value [edx];
  #pragma aux   __LDI4 = \
        float   "fld tbyte ptr [eax]"\
                "push  eax"\
                "push  eax"\
        float   "fstcw [esp]"\
        float   "fwait"\
                "pop eax"\
                "push eax"\
                "or ah,0x0c"\
                "push eax"\
        float   "fldcw [esp]"\
                "pop eax"\
        float   "fistp dword ptr 4[esp]"\
        float   "fldcw [esp]"\
                "pop   eax"\
                "pop   eax"\
                parm caller [eax] value [eax];
  #pragma aux   __I4LD = \
                "push  eax"\
        float   "fild  dword ptr [esp]"\
                "pop   eax"\
        float   "fstp tbyte ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __U4LD = \
                "push  0"\
                "push  eax"\
        float   "fild  qword ptr [esp]"\
                "pop   eax"\
                "pop   eax"\
        float   "fstp tbyte ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __LDI8 = \
        float   "fld tbyte ptr [eax]"\
                "push  eax"\
                "push  eax"\
        float   "fstcw [esp]"\
        float   "fwait"\
                "pop eax"\
                "push eax"\
                "or ah,0x0c"\
                "push eax"\
        float   "fldcw [esp]"\
                "pop eax"\
        float   "fistp qword ptr [edx]"\
        float   "fldcw [esp]"\
                "pop   eax"\
                "pop   eax"\
                parm caller [eax] [edx];
  #pragma aux   __I8LD = \
        float   "fild  qword ptr [eax]"\
        float   "fstp  tbyte ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __U8LD = \
                "push  [eax+4]"\
                "and   [esp],0x7fffffff"\
                "push  [eax]"\
        float   "fild  qword ptr [esp]"\
                "push  [eax+4]"\
                "and   [esp],0x80000000"\
                "push  0"\
        float   "fild  qword ptr [esp]"\
        float   "fchs"\
        float   "faddp st(1),st"\
        float   "fstp  tbyte ptr [edx]"\
                "lea   esp,[esp+16]"\
                parm caller [eax] [edx];
  #pragma aux   __iFDLD = \
        float   "fld  qword ptr [eax]"\
        float   "fstp tbyte ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __iFSLD = \
        float   "fld  dword ptr [eax]"\
        float   "fstp tbyte ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __iLDFD = \
        float   "fld  tbyte ptr [eax]"\
        float   "fstp qword ptr [edx]"\
                parm caller [eax] [edx];
  #pragma aux   __iLDFS = \
        float   "fld  tbyte ptr [eax]"\
        float   "fstp dword ptr [edx]"\
                parm caller [eax] [edx];
 #else  // floating-point calls
  #pragma aux   __FLDA  "*"  parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDS  "*"  parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDM  "*"  parm caller [eax] [edx] [ebx];
  #pragma aux   __FLDD  "*"  parm caller [eax] [edx] [ebx];
  #pragma aux   __LDI4  "*"  parm caller [eax] value [eax];
  #pragma aux   __I4LD  "*"  parm caller [eax] [edx];
  #pragma aux   __U4LD  "*"  parm caller [eax] [edx];
  #pragma aux   __LDI8  "*"  parm caller [eax] [edx];
  #pragma aux   __I8LD  "*"  parm caller [eax] [edx];
  #pragma aux   __U8LD  "*"  parm caller [eax] [edx];
  #pragma aux   __iFDLD "*"  parm caller [eax] [edx];
  #pragma aux   __iFSLD "*"  parm caller [eax] [edx];
  #pragma aux   __iLDFD "*"  parm caller [eax] [edx];
  #pragma aux   __iLDFS "*"  parm caller [eax] [edx];
  #pragma aux   __FLDC  "*"  parm caller [eax] [edx] value [eax];
 #endif
#elif defined( _M_I86 )            // 16-bit pragmas
 #pragma aux     __ZBuf2LD      "*"  parm caller [ax] [dx];
 #if defined(__FPI__)
  extern unsigned __Get87CW(void);
  extern void __Set87CW(unsigned short);
  #pragma aux   __Get87CW = \
                "push ax"\
                "push bp"\
                "mov  bp,sp"\
        float   "fstcw 2[bp]"\
        float   "fwait"\
                "pop bp"\
                "pop ax"\
                value [ax];
  #pragma aux   __Set87CW = \
                "push ax"\
                "push bp"\
                "mov  bp,sp"\
        float   "fldcw 2[bp]"\
                "pop bp"\
                "pop ax"\
                parm caller [ax];
  #pragma aux   __FLDA = \
                "push bp"\
                "mov  bp,ax"\
        float   "fld  tbyte ptr [bp]"\
                "mov  bp,dx"\
        float   "fld  tbyte ptr [bp]"\
        float   "fadd"\
                "mov  bp,bx"\
        float   "fstp tbyte ptr [bp]"\
                "pop  bp"\
                parm caller [ax] [dx] [bx];
  #pragma aux   __FLDS = \
                "push bp"\
                "mov  bp,ax"\
        float   "fld  tbyte ptr [bp]"\
                "mov  bp,dx"\
        float   "fld  tbyte ptr [bp]"\
        float   "fsub"\
                "mov  bp,bx"\
        float   "fstp tbyte ptr [bp]"\
                "pop  bp"\
                parm caller [ax] [dx] [bx];
  #pragma aux   __FLDM = \
                "push bp"\
                "mov  bp,ax"\
        float   "fld  tbyte ptr [bp]"\
                "mov  bp,dx"\
        float   "fld  tbyte ptr [bp]"\
        float   "fmul"\
                "mov  bp,bx"\
        float   "fstp tbyte ptr [bp]"\
                "pop  bp"\
                parm caller [ax] [dx] [bx];
  #pragma aux   __FLDD = \
                "push bp"\
                "mov  bp,ax"\
        float   "fld  tbyte ptr [bp]"\
                "mov  bp,dx"\
        float   "fld  tbyte ptr [bp]"\
        float   "fdiv"\
                "mov  bp,bx"\
        float   "fstp tbyte ptr [bp]"\
                "pop  bp"\
                parm caller [ax] [dx] [bx];
  #pragma aux   __FLDC = \
                "push bp"\
                "mov  bp,dx"\
                /* ST(1) */\
        float   "fld  tbyte ptr [bp]"\
                "mov  bp,ax"\
                /* ST(0) */\
        float   "fld  tbyte ptr [bp]"\
                /* compare ST(0) with ST(1) */\
        float   "fcompp"\
                "push ax"\
                "mov  bp,sp"\
        float   "fstsw 0[bp]"\
        float   "fwait"\
                "pop  ax"\
                "sahf"\
                "sbb  dx,dx"\
                "shl  dx,1"\
                "shl  ah,1"\
                "shl  ah,1"\
                "cmc"\
                "adc  dx,0"\
                "pop  bp"\
                parm caller [ax] [dx] value [dx];
  #pragma aux   __LDI4 = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   tbyte ptr [bp]"\
                "push  dx"\
                "push  ax"\
                "push  ax"\
                "mov   bp,sp"\
        float   "fstcw [bp]"\
        float   "fwait"\
                "pop   ax"\
                "push  ax"\
                "or    ah,0x0c"\
                "mov   2[bp],ax"\
        float   "fldcw 2[bp]"\
        float   "fistp dword ptr 2[bp]"\
        float   "fldcw [bp]"\
                "pop   ax"\
                "pop   ax"\
                "pop   dx"\
                "pop   bp"\
                parm caller [ax] value [dx ax];
  #pragma aux   __I4LD = \
                "push  bp"\
                "push  dx"\
                "push  ax"\
                "mov   bp,sp"\
        float   "fild  dword ptr [bp]"\
                "pop   ax"\
                "pop   dx"\
                "mov   bp,bx"\
        float   "fstp  tbyte ptr [bp]"\
                "pop   bp"\
                parm caller [dx ax] [bx];
  #pragma aux   __U4LD = \
                "push  bp"\
                "push  ax"\
                "push  ax"\
                "push  dx"\
                "push  ax"\
                "mov   bp,sp"\
                "sub   ax,ax"\
                "mov   4[bp],ax"\
                "mov   6[bp],ax"\
        float   "fild  qword ptr [bp]"\
                "pop   ax"\
                "pop   dx"\
                "pop   bp"\
                "pop   bp"\
                "mov   bp,bx"\
        float   "fstp  tbyte ptr [bp]"\
                "pop   bp"\
                parm caller [dx ax] [bx];
  #pragma aux   __LDI8 = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   tbyte ptr [bp]"\
                "push  ax"\
                "push  ax"\
                "mov   bp,sp"\
        float   "fstcw [bp]"\
        float   "fwait"\
                "pop   ax"\
                "push  ax"\
                "or    ah,0x0c"\
                "push  ax"\
        float   "fldcw [bp-2]"\
                "pop   ax"\
                "mov   bp,dx"\
        float   "fistp qword ptr [bp]"\
                "mov   bp,sp"\
        float   "fldcw [bp]"\
                "pop   ax"\
                "pop   ax"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __I8LD = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fild  qword ptr [bp]"\
                "mov   bp,dx"\
        float   "fstp  tbyte ptr [bp]"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __U8LD = \
                "push  bp"\
                "mov   bp,ax"\
                "push  6[bp]"\
                "push  4[bp]"\
                "push  2[bp]"\
                "push  [bp]"\
                "push  6[bp]"\
                "xor   bp,bp"\
                "push  bp"\
                "push  bp"\
                "push  bp"\
                "mov   bp,sp"\
                "and   byte ptr [bp+8+7],0x7f"\
        float   "fild  qword ptr [bp+8]"\
                "and   word ptr [bp+6],0x8000"\
        float   "fild  qword ptr [bp]"\
        float   "fchs"\
        float   "faddp st(1),st"\
                "mov   bp,dx"\
        float   "fstp  tbyte ptr [bp]"\
                "add   sp,16"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __iFDLD = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   qword ptr [bp]"\
                "mov   bp,dx"\
        float   "fstp  tbyte ptr [bp]"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __iFSLD = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   dword ptr [bp]"\
                "mov   bp,dx"\
        float   "fstp  tbyte ptr [bp]"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __iLDFD = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   tbyte ptr [bp]"\
                "mov   bp,dx"\
        float   "fstp  qword ptr [bp]"\
                "pop   bp"\
                parm caller [ax] [dx];
  #pragma aux   __iLDFS = \
                "push  bp"\
                "mov   bp,ax"\
        float   "fld   tbyte ptr [bp]"\
                "mov   bp,dx"\
        float   "fstp  dword ptr [bp]"\
                "pop   bp"\
                parm caller [ax] [dx];
 #else  // floating-point calls
  #pragma aux   __FLDA  "*"  parm caller [ax] [dx] [bx];
  #pragma aux   __FLDS  "*"  parm caller [ax] [dx] [bx];
  #pragma aux   __FLDM  "*"  parm caller [ax] [dx] [bx];
  #pragma aux   __FLDD  "*"  parm caller [ax] [dx] [bx];
  #pragma aux   __LDI4  "*"  parm caller [ax] value [dx ax];
  #pragma aux   __I4LD  "*"  parm caller [dx ax] [bx];
  #pragma aux   __U4LD  "*"  parm caller [dx ax] [bx];
  #pragma aux   __LDI8  "*"  parm caller [ax] [dx];
  #pragma aux   __I8LD  "*"  parm caller [ax] [dx];
  #pragma aux   __U8LD  "*"  parm caller [ax] [dx];
  #pragma aux   __iFDLD "*"  parm caller [ax] [dx];
  #pragma aux   __iFSLD "*"  parm caller [ax] [dx];
  #pragma aux   __iLDFD "*"  parm caller [ax] [dx];
  #pragma aux   __iLDFS "*"  parm caller [ax] [dx];
  #pragma aux   __FLDC  "*"  parm caller [ax] [dx] value [ax];
 #endif
#endif
#endif

// define number of significant digits for long double numbers (80-bit)
// it will be defined in float.h as soon as OW support long double
// used in mathlib/c/ldcvt.c

#ifdef _LONG_DOUBLE_
#if LDBL_DIG == 15
#undef LDBL_DIG
#define LDBL_DIG        19
#else
#error LDBL_DIG has changed from 15
#endif
#endif

// floating point conversion buffer length definition
// is defined in lib_misc/h/cvtbuf.h
// used by various floating point conversion routines
// used in clib/startup/c/cvtbuf.c, lib_misc/h/thread.h
// and mathlib/c/efcvt.c
// it must be equal maximum FP precision ( LDBL_DIG )
// hold lib_misc/h/cvtbuf.h in sync with LDBL_DIG

#ifdef __cplusplus
};
#endif
#endif
