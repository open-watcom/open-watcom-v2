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
* Description:  Validates types of expressions and the resultant types or
*               error codes.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "i64.h"

#include "mngless.h"

#define CHR     TYPE_CHAR
#define UCH     TYPE_UCHAR
#define SHT     TYPE_SHORT
#define USH     TYPE_USHORT
#define INT     TYPE_INT
#define UIN     TYPE_UINT
#define LNG     TYPE_LONG
#define ULN     TYPE_ULONG
#define LN8     TYPE_LONG64
#define UL8     TYPE_ULONG64
#define FLT     TYPE_FLOAT
#define DBL     TYPE_DOUBLE
#define LDB     TYPE_LONG_DOUBLE
#define FIM     TYPE_FIMAGINARY
#define DIM     TYPE_DIMAGINARY
#define LIM     TYPE_LDIMAGINARY
#define BOL     TYPE_BOOL
#define PTR     TYPE_POINTER
#define ARR     TYPE_ARRAY
#define STC     TYPE_STRUCT
#define FCX     TYPE_FCOMPLEX
#define DCX     TYPE_DCOMPLEX
#define LCX     TYPE_LDCOMPLEX

#define ERR     TYPE_LAST_ENTRY  /* no real type behind this value */

/* define macros for promoted types */
#if TARGET_INT == 4

    /* Promoted Unsigned Short is Signed Int */
    #define PUS     TYPE_INT
    /* Promoted Unsigned Int is Unsigned Long */
    #define PUI     TYPE_ULONG

#else /* 16-bit ints */

    /* Promoted Unsigned Short is Unsigned Int */
    #define PUS     TYPE_UINT
    /* Promoted Unsigned Int is Signed Long */          /* 20-sep-89 */
    #define PUI     TYPE_LONG

#endif

/* matches enum DATA_TYPE in ctypes.h */
local  const unsigned char  AddResult[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*  +       BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* CHR */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UCH */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* SHT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* USH */ { PUS,PUS,PUS,PUS,PUS,PUS,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* INT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UIN */ { UIN,UIN,UIN,UIN,UIN,UIN,UIN,PUI,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* LNG */ { LNG,LNG,LNG,LNG,LNG,LNG,PUI,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* ULN */ { ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* LN8 */ { LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UL8 */ { UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* FLT */ { FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,DBL,LDB,FCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DBL */ { DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,LDB,DCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LDB */ { LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LCX,LCX,LCX,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LCX,LCX,LCX, },
/* FIM */ { FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,DCX,LCX,FIM,DIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DIM */ { DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,LCX,DIM,DIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LIM */ { LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LIM,LIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LCX,LCX,LCX, },
/* PTR */ { PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ARR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* STC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UNI */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FNC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* VOD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ENM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* TDF */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UFD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ... */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* WCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FCX */ { FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,DCX,LCX,FCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DCX */ { DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,LCX,DCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LCX */ { LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LCX,LCX,LCX, },
};

/* matches enum DATA_TYPE in ctypes.h */
local  const unsigned char  SubResult[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*  +       BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* CHR */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UCH */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* SHT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* USH */ { PUS,PUS,PUS,PUS,PUS,PUS,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* INT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UIN */ { UIN,UIN,UIN,UIN,UIN,UIN,UIN,PUI,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* LNG */ { LNG,LNG,LNG,LNG,LNG,LNG,PUI,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* ULN */ { ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* LN8 */ { LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* UL8 */ { UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,FLT,DBL,LDB,FCX,DCX,LCX,PTR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* FLT */ { FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,DBL,LDB,FCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DBL */ { DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,LDB,DCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LDB */ { LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LCX,LCX,LCX,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LCX,LCX,LCX, },
/* FIM */ { FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,DCX,LCX,FIM,DIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DIM */ { DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,LCX,DIM,DIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LIM */ { LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LIM,LIM,LIM,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LCX,LCX,LCX, },
/* PTR */ { PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,PTR,ERR,ERR,LDB,ERR,ERR,ERR,INT,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ARR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* STC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UNI */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FNC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* VOD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ENM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* TDF */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UFD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ... */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* WCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FCX */ { FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,FCX,DCX,LCX,FCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FCX,DCX,LCX, },
/* DCX */ { DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,DCX,LCX,DCX,DCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,DCX,DCX,LCX, },
/* LCX */ { LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,LCX,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,LCX,LCX,LCX, },
};

/* matches enum DATA_TYPE in ctypes.h */
local  const unsigned char  IntResult[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*  +       BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX,  */
/* BOL */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* CHR */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UCH */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* SHT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* USH */ { PUS,PUS,PUS,PUS,PUS,PUS,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* INT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UIN */ { UIN,UIN,UIN,UIN,UIN,UIN,UIN,PUI,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LNG */ { LNG,LNG,LNG,LNG,LNG,LNG,PUI,LNG,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ULN */ { ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LN8 */ { LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UL8 */ { UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLT */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DBL */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LDB */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PTR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ARR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* STC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UNI */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FNC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* VOD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ENM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* TDF */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UFD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ... */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* WCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
};

/* matches enum DATA_TYPE in ctypes.h */
local char ShiftResult[TYPE_LAST_ENTRY] = {
/* >>      op2 */
/* BOL */  INT,
/* CHR */  INT,
/* UCH */  INT,         /* 20-may-88 AFS (used to be UIN) */
/* SHT */  INT,
/* USH */  PUS,
/* INT */  INT,
/* UIN */  UIN,
/* LNG */  LNG,
/* ULN */  ULN,
/* LN8 */  LN8,
/* UL8 */  UL8,
/* FLT */  ERR,
/* DBL */  ERR,
/* LDB */  ERR,
/* FIM */  ERR,
/* DIM */  ERR,
/* LIM */  ERR,
/* PTR */  ERR,
/* ARR */  ERR,
/* STC */  ERR,
/* UNI */  ERR,
/* FNC */  ERR,
/* FLD */  ERR,
/* VOD */  ERR,
/* ENM */  ERR,
/* TDF */  ERR,
/* UFD */  ERR,
/* ... */  ERR,
/* PCH */  ERR,
/* WCH */  ERR,
/* FCX */  ERR,
/* DCX */  ERR,
/* LCX */  ERR,
};

/* matches enum DATA_TYPE in ctypes.h */
local  const unsigned char  BinResult[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*  +       BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { BOL,INT,UCH,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* CHR */ { INT,CHR,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UCH */ { UCH,INT,UCH,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* SHT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* USH */ { PUS,PUS,PUS,PUS,PUS,PUS,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* INT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UIN */ { UIN,UIN,UIN,UIN,UIN,UIN,UIN,PUI,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LNG */ { LNG,LNG,LNG,LNG,LNG,LNG,PUI,LNG,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ULN */ { ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LN8 */ { LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UL8 */ { UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLT */ { FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DBL */ { DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LDB */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LIM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PTR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ARR */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* STC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UNI */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FNC */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* VOD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ENM */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* TDF */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UFD */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ... */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* PCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* WCH */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LCX */ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
};


enum    conv_types {
    NIL = 0,
    C2S,    /* char to short        */
    C2I,    /* char to int          */
    C2L,    /* char to long         */
    C2M,    /* char to long long    */
    C2U,    /* char to unsigned     */
    C2F,    /* char to float        */
    C2D,    /* char to double       */
    S2C,    /* short to char        */
    S2I,    /* short to int         */
    S2L,    /* short to long        */
    S2M,    /* short to long long   */
    S2U,    /* short to unsigned    */
    S2F,    /* short to float       */
    S2D,    /* short to double      */
    I2C,    /* int to char          */
    I2S,    /* int to short         */
    I2U,    /* int to unsigned      */
    I2L,    /* int to long          */
    I2M,    /* int to long long     */
    I2F,    /* int to float         */
    I2D,    /* int to double        */
    L2C,    /* long to char         */
    L2S,    /* long to short        */
    L2I,    /* long to integer      */
    L2M,    /* long to long long    */
    L2U,    /* long to unsigned     */
    L2F,    /* long to float        */
    L2D,    /* long to double       */
    M2C,    /* long long to char    */
    M2S,    /* long long to short   */
    M2I,    /* long long to integer */
    M2L,    /* long long to long    */
    M2U,    /* long long to unsigned*/
    M2F,    /* long long to float   */
    M2D,    /* long long to double  */
    U2C,    /* unsigned to char     */
    U2L,    /* unsigned to long     */
    U2M,    /* unsigned to long long*/
    U2F,    /* unsigned to float    */
    U2D,    /* unsigned to double   */
    F2C,    /* float to char        */
    F2S,    /* float to short       */
    F2I,    /* float to int         */
    F2L,    /* float to long        */
    F2M,    /* float to long long   */
    F2D,    /* float to double      */
    D2C,    /* double to char       */
    D2S,    /* double to short      */
    D2I,    /* double to int        */
    D2L,    /* double to long       */
    D2M,    /* double to long long  */
    D2F,    /* double to float      */
    P2P,    /* pointer to pointer   */
    P2A,    /* pointer to arithmetic*/
    A2P,    /* arithmetic to pointer*/
    S2B,    /* scalar to _Bool      */
    B2S,    /* _Bool to scalar      */
    CER,    /* conversion error     */
};

/* matches enum DATA_TYPE in ctypes.h */
/* When indexing, row is the source type, column is the target. */
static enum  conv_types const CnvTable[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*          BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { NIL,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* CHR */ { S2B,NIL,C2U,C2S,C2S,C2I,C2U,C2L,C2L,C2M,C2M,C2F,C2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UCH */ { S2B,U2C,NIL,C2S,C2S,C2I,C2U,C2L,C2L,C2M,C2M,C2F,C2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* SHT */ { S2B,S2C,S2C,NIL,S2U,S2I,S2U,S2L,S2L,S2M,S2M,S2F,S2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* USH */ { S2B,S2C,S2C,S2U,NIL,S2I,S2U,S2L,S2L,S2M,S2M,S2F,S2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* INT */ { S2B,I2C,I2C,I2S,I2S,NIL,I2U,I2L,I2L,I2M,I2M,I2F,I2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UIN */ { S2B,I2C,I2C,I2S,I2S,I2U,NIL,U2L,U2L,U2M,U2M,U2F,U2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LNG */ { S2B,L2C,L2C,L2S,L2S,L2I,L2U,NIL,L2U,L2M,L2M,L2F,L2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* ULN */ { S2B,L2C,L2C,L2S,L2S,L2I,L2U,L2U,NIL,L2M,L2M,L2F,L2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LN8 */ { S2B,M2C,M2C,M2S,M2S,M2I,M2U,M2L,M2L,NIL,M2U,M2F,M2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UL8 */ { S2B,M2C,M2C,M2S,M2S,M2I,M2U,M2L,M2L,M2U,NIL,M2F,M2D,CER,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FLT */ { S2B,F2C,F2C,F2S,F2S,F2I,F2I,F2L,F2L,F2M,F2M,NIL,F2D,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* DBL */ { S2B,D2C,D2C,D2S,D2S,D2I,D2I,D2L,D2L,D2M,D2M,D2F,NIL,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LDB */ { S2B,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FIM */ { S2B,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* DIM */ { S2B,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LIM */ { S2B,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* PTR */ { S2B,P2A,P2A,P2A,P2A,P2A,P2A,P2A,P2A,P2A,P2A,CER,CER,CER,CER,CER,CER,P2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* ARR */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* STC */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UNI */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FNC */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FLD */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* VOD */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* ENM */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* TDF */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UFD */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* ... */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* PCH */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* WCH */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FCX */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* DCX */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LCX */ { CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
};

static  char    Operator[] = {
    #define pick(token,string,class,oper) oper,
    #include "_ctokens.h"
    #undef  pick
};

opr_code TokenToOperator( TOKEN token )
{
    return( Operator[ token ] );
}

static DATA_TYPE BinExprTypeDT( DATA_TYPE typ1, DATA_TYPE typ2 )
{
    DATA_TYPE   data_type;

    data_type = BinResult[ typ1 ][ typ2 ];
    return( data_type );
}

DATA_TYPE BinExprType( TYPEPTR typ_op1, TYPEPTR typ_op2 )
{
    DATA_TYPE   data_type;

    data_type = BinExprTypeDT( DataTypeOf( typ_op1 ), DataTypeOf( typ_op2 ) );
    return( data_type );
}

TYPEPTR TypeOf( TREEPTR node )
{
    TYPEPTR     typ;

    typ = node->expr_type;
    SKIP_TYPEDEFS( typ );
    return( typ );
}

DATA_TYPE DataTypeOf( TYPEPTR typ )
{
    DATA_TYPE data_type;

    data_type = typ->decl_type;
    switch( data_type ) {
    case TYPE_ENUM:
        return( typ->object->decl_type );  /* true size of enum */
    case TYPE_FIELD:
    case TYPE_UFIELD:
        return( typ->u.f.field_type );     /* true bitfield type */
    default:
        break;
    }
    return( data_type );
}

// 0 - near data
// 1 - based data
// 2 - far data
// 3 - far16 data
// 4 - huge data
// 5 - near func
// 6 - based func
// 7 - far func
// 8 - far16 func
// 9 - interrupt func

pointer_class PointerClass( TYPEPTR typ )
{
    type_modifiers    flags;
    pointer_class     class;

    flags = typ->u.p.decl_flags;
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    class = PTR_NEAR;                   // assume NEAR
    if( (flags & FLAG_INTERRUPT) == FLAG_INTERRUPT ) {
        class = PTR_INTERRUPT;          // can't have huge functions
    } else if( flags & FLAG_BASED ) {
        class = PTR_BASED;
    } else if( flags & FLAG_FAR ) {
        class = PTR_FAR;
    } else if( flags & FLAG_FAR16 ) {
        class = PTR_FAR16;
    } else if( flags & FLAG_HUGE ) {
        class = PTR_HUGE;
    }
    if( typ->decl_type == TYPE_FUNCTION )
        class += PTR_FUNC;
    return( class );
}

pointer_class ExprTypeClass( TYPEPTR typ )
{
    TYPEPTR     savtyp;

    savtyp = typ;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_POINTER ) {
        return( PointerClass( savtyp ) );
    }
    return( PTR_NOT );               // indicate not a pointer type
}



#define Convert( opnd, opnd_type, result_type )     opnd

// return 0 not a num, else number of bits | SIGN_BIT if signed
static int NumSize( DATA_TYPE op_type )
{
    int     size;

    size = 0;
    switch( op_type ) {
    case TYPE_BOOL:
        size = 1;
        break;
    case TYPE_CHAR:
        size = SIGN_BIT;
    case TYPE_UCHAR:
        size |= 8;
        break;
    case TYPE_SHORT:
        size = SIGN_BIT;
    case TYPE_USHORT:
        size |= 16;
        break;
    case TYPE_LONG:
        size = SIGN_BIT;
    case TYPE_ULONG:
    case TYPE_POINTER:
        size |= 32;
        break;
    case TYPE_LONG64:
        size = SIGN_BIT;
    case TYPE_ULONG64:
        size |= 64;
        break;
    case TYPE_INT:
        size = SIGN_BIT;
    case TYPE_UINT:
#if TARGET_INT == 2
        size |= 16;
#else
        size |= 32;
#endif
        break;
    }
    return( size );
}

// return 0 not a num, else number of bits | SIGN_BIT if signed
static int NumSizeType( TYPEPTR typ )
{
    int     size;

    size = NumSize( typ->decl_type );
    if( typ->decl_type == TYPE_FIELD ) {
        size = SIGN_BIT | typ->u.f.field_width ;
    } else if( typ->decl_type == TYPE_UFIELD ) {
        size = typ->u.f.field_width;
    }
    return( size );
}

static cmp_result IsMeaninglessCompare( signed_64 val, TYPEPTR typ_op1, TYPEPTR typ_op2, int opr )
{
    signed_64   high;
    signed_64   low;
    rel_op      rel;
    cmp_result  ret;
    int         result_size;
    int         op1_size;
    bool        rev_ret;
    bool        isBitField;

    op1_size = NumSizeType( typ_op1 );
    if( op1_size == 0 ) {
        return( CMP_VOID );
    }
    result_size = NumSize( BinExprType( typ_op1, typ_op2 ) );
    if( result_size == 0 ) {
        return( CMP_VOID );
    }
    rev_ret = FALSE;
    switch( opr ) { // mapped rel ops to equivalent cases
    case T_NE:
        rev_ret = TRUE;
    case T_EQ:
        rel = REL_EQ;
        break;
    case T_GE:
        rev_ret = TRUE;
    case T_LT:
        rel = REL_LT;
        break;
    case T_GT:
        rev_ret = TRUE;
    case T_LE:
        rel = REL_LE;
        break;
    default:
        assert( 0 );
        rel = REL_EQ;
        break;
    }
    isBitField = ( typ_op1->decl_type == TYPE_FIELD || typ_op1->decl_type == TYPE_UFIELD );
    ret = CheckMeaninglessCompare( rel, op1_size, result_size, isBitField, val, &low, &high );

    if( ret != CMP_VOID ) {
        if( rev_ret ) {
            if( ret == CMP_FALSE ) {
                ret = CMP_TRUE;
            } else {
                ret = CMP_FALSE;
            }
        }
    } else {
        if( rel == REL_LE && !rev_ret && !NumSign( op1_size ) && U64Test( &val ) == 0 ) {
            // special case for unsigned <= 0
            ret = CMP_COMPLEX;
        }
    }
    return( ret );
}


// map opr to commuted operand equivalent
static int CommRelOp( int opr )
{
    switch( opr ) {
    case T_NE:  // a != b => b != a
    case T_EQ:
        break;
    case T_GE:  // a >= b => b <= a
        opr = T_LE;
        break;
    case T_LT:
        opr = T_GT;
        break;
    case T_GT:
        opr = T_LT;
        break;
    case T_LE:
        opr = T_GE;
        break;
    }
    return( opr );
}


bool IsZero( TREEPTR tree )
{
    bool    ret;

    if( tree->op.opr == OPR_PUSHINT ) {
        uint64      val64;
        
        val64 = LongValue64( tree );
        ret = !U64Test( &val64 );
    } else {
        ret = FALSE;
    }
    return( ret );
}


TREEPTR BaseConv( TYPEPTR typ1, TREEPTR op2 )
{
    TYPEPTR         typ2;
    type_modifiers  typ1_flags, typ2_flags;

    typ2 =  op2->expr_type;
    typ1 = SkipTypeFluff( typ1 ); // skip typedefs go into enums base
    typ2 = SkipTypeFluff( typ2 );
    typ1_flags = typ1->u.p.decl_flags;
    typ2_flags = typ2->u.p.decl_flags;
    if( typ1->decl_type == TYPE_POINTER && typ2->decl_type == TYPE_POINTER ) {
        if( (typ1_flags & FLAG_FAR) && (typ2_flags & FLAG_BASED) ) {
            op2 = BasedPtrNode( typ2, op2 );
        }
    } else if( typ2->decl_type == TYPE_POINTER ) {
        // If we're converting a near pointer to some larger arithmetic type,
        // we must convert it to a long pointer first to get proper segment.
        // However, in flat model this conversion isn't done, we pretend
        // that segments don't exist.
        if( TypeSize( typ1 ) > TypeSize( typ2 ) ) {
#if _CPU == 386
            if( !(TargetSwitches & FLAT_MODEL) ) {
                op2 = CnvOp( op2, PtrNode( typ2->object, FLAG_FAR, 0 ), 1 );
            }
#else
            op2 = CnvOp( op2, PtrNode( typ2->object, FLAG_FAR, 0 ), 1 );
#endif
        }
    } else if( typ1->decl_type == TYPE_POINTER ) {
        // If we're converting an arithmetic type to a pointer, first convert
        // it to appropriately sized integer to correctly extend/truncate.
        if( TypeSize( typ1 ) != TypeSize( typ2 ) ) {
            op2 = CnvOp( op2, GetIntTypeBySize( TypeSize( typ1 ), FALSE, FALSE ), 1 );
        }
    }
    return( op2 );
}


/* Convert a scalar to boolean */
TREEPTR BoolConv( TYPEPTR typ, TREEPTR tree )
{
    if( tree->op.opr == OPR_ERROR ) {
        return( ErrorNode( tree ) );
    }
    SKIP_TYPEDEFS( typ );
    /* Non-boolean types need to be converted to _Bool; _Bool expressions
     * also need to be converted unless they're constants.
     */
    if( DataTypeOf( typ ) == TYPE_BOOL && TypeOf( tree ) != typ ) {
        tree = ExprNode( tree, OPR_QUESTION, ExprNode( IntLeaf( 1 ), OPR_COLON, IntLeaf( 0 ) ) );
        typ = GetType( TYPE_BOOL );
        tree->op.result_type = typ;
        tree->expr_type      = typ;
        FoldExprTree( tree );
    }
    return( tree );
}


/*
 * what's target compatible between default int as ret type
 * and a later declaration
 */
static bool IsInt( DATA_TYPE op )
{
    bool        ret;

    switch( op ) {
    case TYPE_BOOL:
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_LONG:
    case TYPE_LONG64:
    case TYPE_ULONG64:
        ret = TRUE;
        break;
    default:
       ret = FALSE;
    }
    return( ret );
}


TREEPTR RelOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    TYPEPTR         typ1;
    TYPEPTR         typ2;
    TYPEPTR         cmp_type;
    DATA_TYPE       op1_type, op2_type, result_type;
    TREEPTR         tree;
    cmp_result      cmp_cc;

    FoldExprTree( op1 );   // Needed for meaning less compare
    FoldExprTree( op2 );
    op1 = RValue( op1 );
    op2 = RValue( op2 );
    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    typ1 = TypeOf( op1 );
    typ2 = TypeOf( op2 );
    if( typ1->decl_type == TYPE_POINTER && typ2->decl_type == TYPE_POINTER ) {
        op2 = BaseConv( typ1, op2 );
        op1 = BaseConv( typ2, op1 );
        typ1 = TypeOf( op1 );
        typ2 = TypeOf( op2 );
    }
    op1_type = DataTypeOf( typ1 );
    op2_type = DataTypeOf( typ2 );
    cmp_type = typ1;
    result_type = op1_type;

    /* check for meaningless comparison:  04-feb-91 */
    //TODO this would be a better check maybe in foldtree
    if( !CompFlags.pre_processing ) {            /* 07-feb-89 */
        cmp_cc = CMP_VOID;
        if( op2->op.opr == OPR_PUSHINT ) {
            if( op2_type != TYPE_LONG64 && op2_type != TYPE_ULONG64 ) {
                I32ToI64( op2->op.long_value, &op2->op.long64_value );
            }
            cmp_cc = IsMeaninglessCompare( op2->op.long64_value, typ1, typ2, opr );
        } else if( op1->op.opr == OPR_PUSHINT ) {
            if( op1_type != TYPE_LONG64 && op1_type != TYPE_ULONG64 ) {
                I32ToI64( op1->op.long_value, &op1->op.long64_value );
            }
            cmp_cc = IsMeaninglessCompare( op1->op.long64_value, typ2, typ1, CommRelOp( opr ) );
        }
        if( cmp_cc != CMP_VOID ) {
            if( cmp_cc == CMP_COMPLEX ) {
                CWarn1( WARN_COMPARE_UNSIGNED_VS_ZERO, ERR_COMPARE_UNSIGNED_VS_ZERO );
            } else {
                int res = ( cmp_cc == CMP_TRUE );

                CWarn2( WARN_COMPARE_ALWAYS, ERR_COMPARE_ALWAYS, res );
            }
        }
    }
    if( op1_type == TYPE_VOID || op2_type == TYPE_VOID ) {
        ;           /* do nothing, since error has already been given */
    } else if( op1_type == TYPE_POINTER && op2_type == TYPE_POINTER ) {
         CompatiblePtrType( typ1, typ2, opr );
         if( TypeSize( typ2 ) > TypeSize( typ1 ) ) {
             /* Make sure near pointer is converted to far if necessary */
             cmp_type = typ2;
         }
    } else if( (op1_type == TYPE_POINTER && IsInt( op2_type )) ||
               (op2_type == TYPE_POINTER && IsInt( op1_type )) ) {
        /* ok to compare pointer with constant 0 */
        if( opr != T_EQ && opr != T_NE ) {
            CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        } else if( !( IsZero( op1 ) || IsZero( op2 ) ) ) {
            CWarn1( WARN_POINTER_TYPE_MISMATCH, NON_ZERO_CONST );
        }
        if( op2_type == TYPE_POINTER ) {
            cmp_type = typ2;
        }
    } else if( op1_type == TYPE_STRUCT ||
               op1_type == TYPE_UNION  ||
               op2_type == TYPE_STRUCT ||
               op2_type == TYPE_UNION ) {
            CErr1( ERR_INVALID_RELOP_FOR_STRUCT_OR_UNION );
            result_type = ERR;
    } else {
        result_type = BinExprTypeDT( op1_type, op2_type );
        if( result_type == ERR ) {                      /* 12-sep-89 */
            CErr1( ERR_TYPE_MISMATCH );
        } else {
            cmp_type = GetType( result_type );
        }
    }
    tree = ExprNode( op1, OPR_CMP, op2 );
    if( result_type == ERR ) {
        tree = ErrorNode( tree );
    } else {
        switch( opr ) {
            case T_EQ:
                tree->op.cc = CC_EQ;
                break;
            case T_NE:
                tree->op.cc = CC_NE;
                break;
            case T_LT:
                tree->op.cc = CC_LT;
                break;
            case T_LE:
                tree->op.cc = CC_LE;
                break;
            case T_GT:
                tree->op.cc = CC_GT;
                break;
            case T_GE:
                tree->op.cc = CC_GE;
                break;
            default:
                assert( 0 );
                tree->op.cc = 0;
                break;
        }
        tree->op.compare_type = cmp_type;
        tree->expr_type = GetType( TYPE_INT );
    }
    return( tree );
}


TREEPTR FlowOp( TREEPTR op1, opr_code opr, TREEPTR op2 )
{
    TREEPTR     tree;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    if( op1->op.opr == OPR_PUSHINT ) {
        if( opr == OPR_OR_OR ) {
            if( op1->op.long_value == 0 ) {
                FreeExprNode( op1 );
                return( op2 );
            }
        } else {        // OPR_AND_AND
            if( op1->op.long_value != 0 ) {
                FreeExprNode( op1 );
                return( op2 );
            }
        }
    }
    if( op2->op.opr == OPR_PUSHINT ) {
        if( opr == OPR_OR_OR ) {
            if( op2->op.long_value == 0 ) {
                FreeExprNode( op2 );
                return( op1 );
            }
        } else {        // OPR_AND_AND
            if( op2->op.long_value != 0 ) {
                FreeExprNode( op2 );
                return( op1 );
            }
        }
    }
    tree = ExprNode( op1, opr, op2 );
    tree->expr_type = GetType( TYPE_INT );
    tree->op.label_index = NextLabel();
    return( tree );
}


local TREEPTR MulByConst( TREEPTR opnd, long amount )
{
    TREEPTR     tree;

    if( opnd->op.opr == OPR_PUSHINT ) {
        opnd->op.long_value *= amount;
        return( opnd );
    }
    switch( TypeOf( opnd )->decl_type ) {
    case TYPE_LONG:
    case TYPE_ULONG:
        tree = ExprNode( opnd, OPR_MUL, LongLeaf( amount ) );
        tree->expr_type = GetType( TYPE_LONG );
        break;
    case TYPE_INT:
    case TYPE_UINT:
    default:
        tree = ExprNode( opnd, OPR_MUL, IntLeaf( amount ) );
        tree->expr_type = GetType( TYPE_INT );
    }
    tree->op.result_type = tree->expr_type;
    return( tree );
}


local TREEPTR PtrSubtract( TREEPTR result, unsigned long size, int result_type)
{
    int         shift_count;
    int         n;
    TREEPTR     tree;
    TYPEPTR     typ;

    typ = GetType( result_type );
    result->expr_type = typ;
    result->op.result_type = typ;
    for( n = 2, shift_count = 1; shift_count < 8; ++shift_count ) {
        if( n == size ) {
            tree = ExprNode( result, OPR_RSHIFT, IntLeaf( shift_count ) );
            tree->expr_type = typ;
            tree->op.result_type = typ;
            return( tree );
        }
        n *= 2;
    }
    if( result_type == INT ) {
        tree = ExprNode( result, OPR_DIV, IntLeaf( size ) );
        tree->expr_type = GetType( TYPE_INT );
    } else {
        tree = ExprNode( result, OPR_DIV, LongLeaf( size ) );
        tree->expr_type = GetType( TYPE_LONG );
    }
    tree->op.result_type = tree->expr_type;
    return( tree );
}


// Remove the OPR_CONVERT for lcast so it looks like an LVALUE
extern TREEPTR LCastAdj( TREEPTR tree )
{
    TREEPTR         opnd;
    TYPEPTR         typ;
    type_modifiers  modifiers;

    opnd = tree->right;
    tree->right = NULL;
    typ = tree->expr_type;
    opnd->expr_type = typ;
    FreeExprTree( tree );
    CWarn1( WARN_LVALUE_CAST, ERR_LVALUE_CAST );
    opnd->op.flags &= ~OPFLAG_RVALUE;
    if( opnd->op.opr == OPR_PUSHSYM ) {
        opnd->op.opr = OPR_PUSHADDR;
    } else if( opnd->op.opr == OPR_POINTS ) {
        // fix up fred's screw ball pointer op
        modifiers = FlagOps( opnd->op.flags );
        opnd->op.result_type = PtrNode( typ, modifiers, SEG_DATA );
    }
    return( opnd );
}


static void CheckAddrOfArray( TREEPTR opnd )
{
    TYPEPTR     typ;

    if( opnd->op.opr == OPR_ADDROF ) {
        typ = opnd->expr_type->object;
        if( typ != NULL ) {
            SKIP_TYPEDEFS( typ );
            if( typ->decl_type == TYPE_ARRAY ) {
                CWarn1( WARN_ADDR_OF_ARRAY, ERR_ADDR_OF_ARRAY );
            }
        }
    }
}


static TYPEPTR PtrofSym( SYM_HANDLE sym_handle, TYPEPTR typ )
{
    SYM_ENTRY   sym;

    SymGet( &sym, sym_handle );
    typ = PtrNode( typ, sym.attrib, SEG_DATA );
    return( typ );
}


static TREEPTR ArrayPlusConst( TREEPTR op1, TREEPTR op2 )
{
    TREEPTR     result;
    TYPEPTR     typ;

    CheckAddrOfArray( op1 );
    if( op2->op.opr == OPR_PUSHINT ) {
        if( op1->op.opr == OPR_PUSHADDR ) {
            typ = op1->expr_type;
            SKIP_TYPEDEFS( typ );
            if( typ->decl_type == TYPE_ARRAY ) {

                op2->op.long_value *= SizeOfArg( typ->object );
                typ = PtrofSym( op1->op.sym_handle, typ->object );
                result = ExprNode( op1, OPR_ADD, op2 );
                result->expr_type = typ;
                result->op.result_type = typ;
                return( result );
            }
        }
    }
    return( 0 );
}


static TREEPTR ArrayMinusConst( TREEPTR op1, TREEPTR op2 )
{
    TREEPTR     result;
    TYPEPTR     typ;

    CheckAddrOfArray( op1 );
    if( op2->op.opr == OPR_PUSHINT ) {
        if( op1->op.opr == OPR_PUSHADDR ) {
            typ = op1->expr_type;
            SKIP_TYPEDEFS( typ );
            if( typ->decl_type == TYPE_ARRAY ) {
                op2->op.long_value =
                        (- op2->op.long_value) * SizeOfArg( typ->object );
                typ = PtrofSym( op1->op.sym_handle, typ->object );
                result = ExprNode( op1, OPR_ADD, op2 );
                result->expr_type = typ;
                result->op.result_type = typ;
                return( result );
            }
        }
    }
    return( 0 );
}


local int LValue( TREEPTR op1 )
{
    TYPEPTR     typ;

    if( op1->op.opr == OPR_ERROR ) return( 1 );
    if( IsLValue( op1 ) ) {
        typ = TypeOf( op1 );
        if( typ->decl_type != TYPE_ARRAY ) {
            if( TypeSize( typ ) == 0 ) {
                CErr1( ERR_INCOMPLETE_EXPR_TYPE );
            }
            if( op1->op.flags & OPFLAG_LVALUE_CAST ) {
                op1->op.flags &= ~(OPFLAG_LVALUE_CAST | OPFLAG_RVALUE);
                CWarn1( WARN_LVALUE_CAST, ERR_LVALUE_CAST );
            }
            return( 1 );
        }
    }
    CErr1( ERR_MUST_BE_LVALUE );
    return( 0 );
}


TREEPTR AddOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    DATA_TYPE           op1_type, op2_type, result_type;
    TYPEPTR             op1_tp, op2_tp;
    TYPEPTR             res_type;
    unsigned long       size;
    TREEPTR             result;
    TREEPTR             tree;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    switch( opr ) {
    case T_PLUS_EQUAL:
    case T_PLUS_PLUS:
    case T_MINUS_EQUAL:
    case T_MINUS_MINUS:
        if( (op1->op.opr == OPR_CONVERT || op1->op.opr == OPR_CONVERT_PTR)
         && CompFlags.extensions_enabled ) {
            op1 = LCastAdj( op1 );
        }
        LValue( op1 );
        break;
    case T_MINUS:
        result = ArrayMinusConst( op1, op2 );
        if( result != 0 )  return( result );
        op1 = RValue( op1 );
        break;
    case T_PLUS:
        result = ArrayPlusConst( op1, op2 );    // check for array + const
        if( result != 0 )  return( result );
        result = ArrayPlusConst( op2, op1 );    // check for const + array
        if( result != 0 )  return( result );
    default:
        op1 = RValue( op1 );
    }
    op2 = RValue( op2 );
    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    op1_tp = TypeOf( op1 );
    op2_tp = TypeOf( op2 );
    op1_type = DataTypeOf( op1_tp );
    op2_type = DataTypeOf( op2_tp );
    result = 0;
    if( op1_type == TYPE_UNION || op1_type == TYPE_STRUCT
    ||  op2_type == TYPE_UNION || op2_type == TYPE_STRUCT ) {
        result_type = ERR;
    } else if( op1_type == TYPE_VOID  ||
               op2_type == TYPE_VOID  ||
               op1_type == TYPE_ARRAY ) {       /* ++array; or --array; */
        result_type = INT;
    } else {
        switch( opr ) {
        case T_PLUS:
            result_type = AddResult[ op1_type ][ op2_type ];
            break;
        case T_PLUS_PLUS:
        case T_MINUS_MINUS:
        case T_PLUS_EQUAL:
        case T_MINUS_EQUAL:
            if( op1_type == TYPE_POINTER  &&  op2_type >= TYPE_FLOAT ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            }
            result_type = op1_type;
            break;
        default:
            result_type = SubResult[ op1_type ][ op2_type ];
            if(( op1_type == PTR )&&( op2_type == PTR )) {
                /* make sure both pointers are same type */
                CompatiblePtrType( op1_tp, op2_tp, opr );
                if(( op1_tp->u.p.decl_flags & FLAG_HUGE ) ||
                   ( op2_tp->u.p.decl_flags & FLAG_HUGE ) ) {
                    result_type = LNG;
                } else if( (TargetSwitches & (BIG_DATA | CHEAP_POINTER) )
                          == BIG_DATA ) {
                    if( ((op1_tp->u.p.decl_flags & (FLAG_FAR | FLAG_NEAR)) == 0 )
                     && ((op2_tp->u.p.decl_flags & (FLAG_FAR | FLAG_NEAR)) == 0) ) {

                        result_type = LNG;
                    }
                }
            }
        }
    }
    if( result_type == ERR ) {
        CErr1( ERR_EXPR_MUST_BE_SCALAR );
        result_type = INT;
    }
    if( op1_type == PTR ) {
        res_type = TypeOf( op1 );
        size = SizeOfArg( res_type->object );
        if( size == 0 ) {
            CErr1( ERR_CANT_USE_VOID );
            size = 1;
        }
        /* subtraction of 2 pointers */
        if( (result_type == INT) || (result_type == LNG) ) {
            res_type = GetType( result_type );
        }
        if( size != 1 ) {
            /* subtraction of 2 pointers */
            if( (result_type == INT) || (result_type == LNG) ) {
                result = ExprNode( op1, TokenToOperator( opr ), op2 );
                return( PtrSubtract( result, size, result_type ) );
            } else if( (op1_tp->u.p.decl_flags & FLAG_HUGE) ||
                      ((TargetSwitches & (BIG_DATA | CHEAP_POINTER)) == BIG_DATA) ) {
                if( (op2_type != LNG) && (op2_type != ULN) ) {
                    op2 = CnvOp( op2, GetType( TYPE_LONG ), 1 );
                }
            }
            op2 = MulByConst( op2, size );
        }
    } else if( op2_type == PTR ) {      /* &&  op1_type != PTR */
        if( opr == T_PLUS_EQUAL || opr == T_MINUS_EQUAL || opr == T_MINUS ) {
            CErr2p( ERR_RIGHT_OPERAND_IS_A_POINTER, Tokens[opr] );
        }
        res_type = TypeOf( op2 );
        size = SizeOfArg( res_type->object );
        if( size == 0 ) {
            CErr1( ERR_CANT_USE_VOID );
            size = 1;
        }
        if( size != 1 ) {
            if( (op2_tp->u.p.decl_flags & FLAG_HUGE) ||
               ((TargetSwitches & (BIG_DATA | CHEAP_POINTER)) == BIG_DATA) ) {
                if( (op1_type != LNG ) && (op1_type != ULN) ) {
                    op2 = CnvOp( op2, GetType( TYPE_LONG ), 1 );
                }
            }
            op1 = MulByConst( op1, size );
        }
    } else {
        res_type = GetType( result_type );
        op1 = Convert( op1, op1_type, result_type );
        op2 = Convert( op2, op2_type, result_type );
    }
    tree = ExprNode( op1, TokenToOperator( opr ), op2 );
    tree->expr_type = res_type;
    tree->op.result_type = res_type;
    return( tree );
}


TREEPTR BinOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    DATA_TYPE      op1_type, op2_type, result_type;
    TREEPTR        tree;
    TYPEPTR        typ;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    op2 = RValue( op2 );
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    typ = TypeOf( op1 );
    op1_type = DataTypeOf( typ );
    op2_type = DataTypeOf( TypeOf( op2 ) );
    if( op1_type == TYPE_VOID  ||  op2_type == TYPE_VOID ) {
        result_type = TYPE_VOID;
    } else if( op1_type == TYPE_UNION  ||
               op1_type == TYPE_STRUCT ||
               op2_type == TYPE_UNION  ||
               op2_type == TYPE_STRUCT ) {
        result_type = ERR;
    } else {
        result_type = BinExprTypeDT( op1_type, op2_type );
        if( opr == T_PERCENT  &&  result_type >= TYPE_FLOAT ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        }
        if( result_type < INT ) result_type = INT;
    }
    if( result_type == ERR ) {
        CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
        result_type = INT;
    }
    switch( opr ) {
    case T_XOR_EQUAL:
    case T_OR_EQUAL:
        /* if op2 is a constant, check to see if constant truncated */
        if( op2->op.opr == OPR_PUSHINT ) {
            unsigned long       max_value;

            switch( TypeSize( typ ) ) {
            case 1:
                max_value = 0x000000FF;
                break;
            case 2:
                max_value = 0x0000FFFF;
                break;
            default:
                max_value = ~0ul;
                break;
            }
            if( op2->op.ulong_value > max_value ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
        }
    case T_AND_EQUAL:
    case T_RSHIFT_EQUAL:
    case T_LSHIFT_EQUAL:
    case T_PERCENT_EQUAL:
        /* check for integral operand.          16-nov-89 */
        result_type = IntResult[ op1_type ][ op2_type ];
        if( result_type == ERR ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        }
    case T_TIMES_EQUAL:
    case T_DIVIDE_EQUAL:
        result_type = op1_type;
        break;
    default:
        op1 = Convert( op1, op1_type, result_type );
        op2 = Convert( op2, op2_type, result_type );
        typ = GetType( result_type );
    }
    tree = ExprNode( op1, TokenToOperator( opr ), op2 );
    if( result_type == ERR ) {
        tree = ErrorNode( tree );
    } else {
        tree->expr_type = GetType( result_type );
        tree->op.result_type = typ;
    }
    return( tree );
}


local void SetSymAssigned( TREEPTR opnd )
{
    SYM_ENTRY   sym;

    while( opnd->op.opr == OPR_INDEX ) opnd = opnd->left;
    if( opnd->op.opr == OPR_PUSHADDR ) {
        SymGet( &sym, opnd->op.sym_handle );
        if( sym.level != 0 ) {
            if( !(sym.flags & SYM_ASSIGNED) ) {
                sym.flags |= SYM_ASSIGNED;
                SymReplace( &sym, opnd->op.sym_handle );
            }
        }
    }
}


TREEPTR InitAsgn( TYPEPTR typ, TREEPTR op2 )
{
    if( op2->op.opr == OPR_ERROR ) {
        return( op2 );
    }
    op2 = RValue( op2 );
    op2 = BoolConv( typ, op2 );
    if( !CompFlags.no_check_inits ) {   // else fuck em
        ParmAsgnCheck( typ, op2, 0, TRUE );
    }
    return( op2 );
}


TREEPTR AsgnOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    TYPEPTR         typ;
    pointer_class   op1_class;
    pointer_class   op2_class;
    int             isLValue;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    if( op1->op.flags & OPFLAG_LVALUE_CAST ) {      /* 18-aug-95 */
        if( CompFlags.extensions_enabled ) {
            op1->op.flags &= ~(OPFLAG_LVALUE_CAST | OPFLAG_RVALUE);
            if( op1->op.opr == OPR_PUSHSYM ) {
                op1->op.opr = OPR_PUSHADDR;
            }
            CWarn1( WARN_LVALUE_CAST, ERR_LVALUE_CAST );
        } else {
            CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
        }
    }
    if( (op1->op.opr == OPR_CONVERT || op1->op.opr == OPR_CONVERT_PTR)
     && CompFlags.extensions_enabled ) {
        op1 = LCastAdj( op1 );
    }
    isLValue = LValue( op1 );
    if( isLValue ) {
        op_flags volatile_flag;

        volatile_flag = op1->op.flags & OPFLAG_VOLATILE;
        if( opr != T_EQUAL      &&  opr != T_ASSIGN_LAST ) {
            if( opr == T_PLUS_EQUAL  ||  opr == T_MINUS_EQUAL ) {
                op1 = AddOp( op1, opr, op2 );
            } else {
                op1 = BinOp( op1, opr, op2 );
            }
            op1->op.flags |= volatile_flag;
            return( op1 );
        }
        SetSymAssigned( op1 );
        typ = TypeOf( op1 );
        op2 = RValue( op2 );
        ParmAsgnCheck( typ, op2, 0, TRUE );
        op2 = BaseConv( typ, op2 );
        op2 = BoolConv( typ, op2 );
        if( opr == T_ASSIGN_LAST ) opr = T_EQUAL;
        op1_class = ExprTypeClass( typ );
        op2_class = ExprTypeClass( op2->expr_type );
        if( op1_class != op2_class ) {
            if( FAR16_PTRCLASS( op1_class ) || FAR16_PTRCLASS( op2_class ) ) {  // if far16 pointer
                op2 = ExprNode( NULL, OPR_CONVERT_PTR, op2 );
                op2->op.sp.oldptr_class = op2_class;
                op2->op.sp.newptr_class = op1_class;
            } else {
                 op2 = ExprNode( NULL, OPR_CONVERT, op2 );
                 op2->op.result_type = typ;
            }
            op2->expr_type = typ;
        }
        op1 = ExprNode( op1, TokenToOperator( opr ), op2 );
        op1->op.flags |= volatile_flag;
        op1->expr_type = typ;
        op1->op.result_type = typ;
    } else {
        FreeExprTree( op2 );
    }
    return( op1 );
}


void ChkConst( TREEPTR opnd )
{
    if( opnd->op.opr != OPR_ERROR ) {
        if( opnd->op.flags & OPFLAG_CONST ) {
            CErr1( ERR_CANNOT_MODIFY_CONST );
        }
    }
}


TREEPTR IntOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    DATA_TYPE  op1_type, op2_type, result_type;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    op1 = RValue( op1 );
    op2 = RValue( op2 );
    op1_type = DataTypeOf( TypeOf( op1 ) );
    op2_type = DataTypeOf( TypeOf( op2 ) );
    if( op1_type == TYPE_VOID  ||  op2_type == TYPE_VOID ) {
        result_type = TYPE_VOID;
    } else if( op1_type == TYPE_UNION  ||  op2_type == TYPE_UNION ) {
        result_type = ERR;
    } else {
        result_type = IntResult[ op1_type ][ op2_type ];
    }
    if( result_type == ERR ) {
        CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        result_type = INT;
    }
    op1 = Convert( op1, op1_type, result_type );
    op2 = Convert( op2, op2_type, result_type );
    op1 = ExprNode( op1, TokenToOperator( opr ), op2 );
    if( result_type == ERR ) {
        op1 = ErrorNode( op1 );
    } else {
        op1->expr_type = GetType( result_type );
        op1->op.result_type = op1->expr_type;
    }
    return( op1 );
}


TREEPTR ShiftOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    DATA_TYPE   op1_type, op2_type, result_type;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    op1 = RValue( op1 );
    op2 = RValue( op2 );
    op1_type = DataTypeOf( TypeOf( op1 ) );
    op2_type = DataTypeOf( TypeOf( op2 ) );
    if( op1_type == TYPE_VOID  ||  op2_type == TYPE_VOID ) {
        result_type = TYPE_VOID;
    } else {
        if( op1_type <= TYPE_STRUCT ) {
            result_type = ShiftResult[ op1_type ];
        } else {
            result_type = ERR;
        }
        if( result_type == ERR || op2_type > UL8 ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            result_type = INT;
        }
    }
    op1 = Convert( op1, op1_type, result_type );
    op1 = ExprNode( op1, TokenToOperator( opr ), op2 );
    if( result_type == ERR ) {
        op1 = ErrorNode( op1 );
    } else {
        op1->expr_type = GetType( result_type );
        op1->op.result_type = op1->expr_type;
    }
    return( op1 );
}


int FuncPtr( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_POINTER ) return( 0 );
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_FUNCTION ) return( 0 );
    return( 1 );
}

/* Check if pointer conversion is losing high (segment) bits.
 * A special dispensation is made for null pointer conversions; if the
 * source operand is a zero integer constant, chopping off high bits
 * is not considered an error. This helps in Win16 programming where
 * NULL (which may be a 32-bit quantity) is often converted to
 * a 16-bit HANDLE type.
 */
bool IsPtrConvSafe( TREEPTR src, TYPEPTR newtyp, TYPEPTR oldtyp )
{
    bool                is_safe = TRUE;
    enum segments       new_seg;
    enum segments       old_seg;
    type_modifiers      new_flags;
    type_modifiers      old_flags;

    SKIP_TYPEDEFS( oldtyp );
    SKIP_TYPEDEFS( newtyp );
    assert( oldtyp->decl_type == TYPE_POINTER );
    assert( newtyp->decl_type == TYPE_POINTER );

    /* If new type isn't smaller than old, assume conversion is safe. */
    if( TypeSize( newtyp ) < TypeSize( oldtyp ) ) {
        /* Determine target pointer base. */
        new_flags = newtyp->u.p.decl_flags;
        if( new_flags & FLAG_BASED ) {
            switch( newtyp->u.p.based_kind ) {
            case BASED_SEGNAME:
                new_seg = newtyp->u.p.segment;
                break;
            /* NYI: This could be smarter and check other based types. */
            default:
                new_seg = SEG_UNKNOWN;
            }
        } else if( FuncPtr( newtyp ) ) {
            new_seg = SEG_CODE;
        } else {
            new_seg = SEG_DATA;
        }
        /* Determine source pointer base. */
        old_flags = oldtyp->u.p.decl_flags;
        if( old_flags & FLAG_BASED ) {
            switch( oldtyp->u.p.based_kind ) {
            case BASED_SEGNAME:
                old_seg = oldtyp->u.p.segment;
                break;
            /* NYI: This could be smarter and check other based types. */
            default:
                old_seg = SEG_UNKNOWN;
            }
        } else {
            old_seg = oldtyp->u.p.segment;
        }
        is_safe = ( old_seg == new_seg ) || IsZero( src );
    }
    return( is_safe );
}

TREEPTR CnvOp( TREEPTR opnd, TYPEPTR newtyp, int cast_op )
{
    TYPEPTR             typ;
    enum  conv_types    cnv;
    opr_code            opr;
    op_flags            flags;
    DATA_TYPE           opnd_type;

    if( opnd->op.opr == OPR_ERROR ) return( opnd );
    SKIP_TYPEDEFS( newtyp );
    opr = opnd->op.opr;
    if( newtyp->decl_type == TYPE_VOID ) {              /* 26-oct-88 */
        typ = TypeOf( opnd );
        if( typ->decl_type == TYPE_VOID ) return( opnd );
    }
    flags = OPFLAG_NONE;
    if( cast_op && CompFlags.extensions_enabled ) {
        if( IsLValue( opnd ) ) {
            flags |= OPFLAG_LVALUE_CAST;
        }
    }
    opnd_type = opnd->expr_type->decl_type;
    if( !CompFlags.pre_processing ) {
        opnd = RValue( opnd );
    }
    typ = TypeOf( opnd );
    SetDiagType2( typ, newtyp );
    if( newtyp->decl_type > TYPE_POINTER ) {
        if( newtyp->decl_type == TYPE_VOID ) {
            opnd = ExprNode( 0, OPR_CONVERT, opnd );
            opnd->expr_type = newtyp;
            opnd->op.result_type = newtyp;
            if( cast_op )  CompFlags.meaningless_stmt = 0;      /* 21-jul-89 */
        } else if( newtyp->decl_type == TYPE_ENUM ) {
            if( typ->decl_type == TYPE_POINTER ) {
                CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
            }
            newtyp = newtyp->object;            /* 02-feb-93 */
            goto convert;
        } else {
            if( cast_op ) {
                CErr1( ERR_MUST_BE_SCALAR_TYPE );
                SetDiagPop();
                return( ErrorNode( opnd ) );
            } else if( typ != newtyp ) {        /* 16-aug-91, added cond */
                CErr1( ERR_TYPE_MISMATCH );
                SetDiagPop();
                return( ErrorNode( opnd ) );
            }
        }
    } else if( typ->decl_type != TYPE_VOID ) {
convert:                                /* moved here 30-aug-89 */
        cnv = CnvTable[ DataTypeOf( typ ) ]
                      [ DataTypeOf( newtyp ) ];
        if( cnv == CER ) {
            CErr1( ERR_INVALID_CONVERSION );
            SetDiagPop();
            return( ErrorNode( opnd ) );
        } else if( cnv != NIL ) {
            if( cnv == P2P ) {
                if( ( typ->u.p.decl_flags & MASK_ALL_MEM_MODELS )
                    != ( newtyp->u.p.decl_flags & MASK_ALL_MEM_MODELS )
                    || ( opnd_type == TYPE_ARRAY ) ) {
                    if( !IsPtrConvSafe( opnd, newtyp, typ ) ) {
                        if( cast_op ) {
                            CWarn1( WARN_CAST_POINTER_TRUNCATION, ERR_CAST_POINTER_TRUNCATION );
                        } else {
                            CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
                        }
                    }
                    if( cast_op == 0 ) {
                        if( (typ->u.p.decl_flags & FLAG_BASED) &&
                            (newtyp->u.p.decl_flags & FLAG_FAR) ) {
                            opnd = BasedPtrNode( typ, opnd );
                            opnd->expr_type = newtyp;
                            opnd->op.result_type = newtyp;
                            SetDiagPop();
                            return( opnd );
                        }
                        cast_op = 1;        /* force a convert */
                    }
                } else if( FuncPtr( typ ) || FuncPtr( newtyp ) ) {
                    cast_op = 1;    /* force a convert */
                } else if( TypeSize( typ ) != TypeSize( newtyp ) ) {
                                        /* 25-apr-88*/
                    cast_op = 1;    /* force a convert */
                } else if( typ->decl_type != TYPE_POINTER ||
                        newtyp->decl_type != TYPE_POINTER ) {
                    /* 19-jan-89 */
                    cast_op = 1;    /* force a convert */
                } else if( opr == OPR_PUSHADDR &&
                           opnd->op.opr == OPR_ADDROF ) {
                    opnd->expr_type = newtyp;
                    SetDiagPop();
                    return( opnd );
                } else if( cast_op && CompFlags.extensions_enabled ) {
                    /* 15-oct-92: We know the following: */
                    /* - it is a cast operation  */
                    /* - both types are pointers */
                    /* - extensions are enabled  */
                    /* - both pointers are the same size */
                    /* - neither pointer is a function pointer */
                    /* So, if it is still an lvalue */
                    /* - then just update the type and leave it */
                    /* - as an lvalue. This will allow the */
                    /* - following statement to get through without */
                    /* - generating an error! */
                    /*              (char *)p += 2;  */
                    if( opr == OPR_PUSHADDR || IsLValue( opnd ) ) {
                        /* don't do it for based or far16. 27-oct-92*/
                        if( !Far16Pointer( opnd->op.flags ) ) {
                            opnd->expr_type = newtyp;
                            opnd->op.opr = opr;
                            opnd->op.flags |= OPFLAG_LVALUE_CAST;
                            SetDiagPop();
                            return( opnd );
                        }
                    }
                }
            }
            if( !cast_op && cnv == P2A && TypeSize( typ ) > TypeSize( newtyp ) ) {
                CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
            }
            if( cnv == P2A || cnv == A2P ) {
                if( TypeSize( typ ) != TypeSize( newtyp ) ) {
                    // Conversion to/from a pointer may need special treatment
                    opnd = BaseConv( newtyp, opnd );
                }
            }
            if( cnv == S2B ) {
                // Conversion to _Bool needs special treatment
                opnd = BoolConv( newtyp, opnd );
            }
            if( cast_op  ||  cnv != P2P ) {
/* convert: moved 30-aug-89 */
                if( IsConstLeaf( opnd ) ) {
                    CastConstNode( opnd, newtyp );
                    opnd->expr_type = newtyp;
                } else {
                    pointer_class     new_class;
                    pointer_class     old_class;

                    new_class = ExprTypeClass( newtyp );
                    old_class = ExprTypeClass( typ );
                    if( new_class != old_class &&
                    (FAR16_PTRCLASS( new_class ) || FAR16_PTRCLASS( old_class )) ) { // foreign pointers
                        opnd = ExprNode( NULL, OPR_CONVERT_PTR, opnd );
                        opnd->op.sp.oldptr_class = old_class;
                        opnd->op.sp.newptr_class = new_class;
#if _CPU == 8086
                    } else if( cnv == P2A && newtyp->type_flags & TF2_TYPE_SEGMENT ) {
                        // getting segment value of pointer
                        opnd = BasedPtrNode( typ, opnd );
                        opnd = ExprNode( NULL, OPR_CONVERT_SEG, opnd );
                        opnd->op.result_type = newtyp;
#endif
                    } else {
                        opnd = ExprNode( NULL, OPR_CONVERT, opnd );
                        opnd->op.result_type = newtyp;
                    }
                    opnd->expr_type = newtyp;
                }
            }
        } else if( IsConstLeaf( opnd ) ) {
            CastConstValue( opnd, newtyp->decl_type );
            opnd->expr_type = newtyp;
        } else if( opnd->expr_type != newtyp ) {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->expr_type = newtyp;
            opnd->op.result_type = newtyp;
        } else { // NIL convert
            opnd->op.flags |= flags;
        }
    }
    SetDiagPop();
    return( opnd );
}

/* Fix up assignments - used for passing function arguments and also
 * return values. Fixes up based pointers and other x86 weirdnesses.
 */
TREEPTR FixupAss( TREEPTR opnd, TYPEPTR newtyp )
{
//TODO check out base ptrs
    TYPEPTR             typ;
    enum conv_types     cnv;
    DATA_TYPE           decl1;
    DATA_TYPE           decl2;

    if( opnd->op.opr == OPR_ERROR ) return( opnd );
    opnd = BaseConv( newtyp, opnd );
    opnd = BoolConv( newtyp, opnd );
    newtyp = SkipTypeFluff( newtyp );
    typ = SkipTypeFluff( opnd->expr_type );
    decl1 = DataTypeOf( typ );
    decl2 = DataTypeOf( newtyp );
    if( decl1 > TYPE_POINTER || decl2 > TYPE_POINTER ) {
        return( opnd );
    }
    cnv = CnvTable[ decl1 ][ decl2 ];
    if( cnv == CER ) {
        return(  opnd  );
    } else if( cnv == P2P ) {
        pointer_class     new_class;
        pointer_class     old_class;

        new_class = ExprTypeClass( newtyp );
        old_class = ExprTypeClass( typ );
        if( new_class != old_class ) {
            opnd = ExprNode( NULL, OPR_CONVERT_PTR, opnd );
            opnd->op.sp.oldptr_class = old_class;
            opnd->op.sp.newptr_class = new_class;
        } else {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->op.result_type = newtyp;
        }
    } else {
        if( IsConstLeaf( opnd ) ) {
            CastConstValue( opnd, newtyp->decl_type );
        } else {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->op.result_type = newtyp;
        }
    }
    opnd->expr_type = newtyp;
    return( opnd );
}

TREEPTR UMinus( TREEPTR opnd )
{
//  FLOATVAL        *flt;
    DATA_TYPE       t;

    opnd = RValue( opnd );
    if( opnd->op.opr != OPR_ERROR ) {
        t = DataTypeOf( TypeOf( opnd ) );
        if( t != TYPE_VOID ) {
            if( t >= TYPE_POINTER ) {
                CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
                opnd = ErrorNode( opnd );
            } else {
                opnd = ExprNode( 0, OPR_NEG, opnd );
                opnd->expr_type = GetType( SubResult[t][t] );
                opnd->op.result_type = opnd->expr_type;
            }
        }
    }
#if 0
    switch( opnd->op.opr ) {
    case OPR_ERROR:
        break;
    case OPR_PUSHINT:
        switch( opnd->op.const_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
            opnd->op.long_value =  -(char)opnd->op.long_value;
            break;
        case TYPE_SHORT:
        case TYPE_USHORT:
            opnd->op.long_value = -(short)opnd->op.long_value;
            break;
        case TYPE_INT:
            opnd->op.long_value = -(target_int)opnd->op.long_value;
            break;
        case TYPE_UINT:
            opnd->op.long_value =
                        (target_uint)( - (target_uint)opnd->op.long_value);
            break;
        case TYPE_LONG:
        case TYPE_ULONG:
            opnd->op.long_value = - opnd->op.long_value;
            break;
        }
        break;
    case OPR_PUSHFLOAT:
        flt = opnd->op.float_value;
        if( flt->len != 0 ) {           // if still in string form
            flt->string[0] ^= '+' ^ '-';// - change '+' to '-' and vice versa
        } else {                        // else
#ifdef _LONG_DOUBLE_
            flt->ld.exponent ^= 0x8000;     // - flip binary sign bit
#else
            flt->ld.word[1] ^= 0x80000000;  // - flip sign
#endif
        }
        break;
    default:
        t = DataTypeOf( TypeOf( opnd ) );
        if( t == TYPE_VOID ) break;
        if( t >= TYPE_POINTER ) {
            CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
            opnd = ErrorNode( opnd );
        } else {
            opnd = ExprNode( 0, OPR_NEG, opnd );
            opnd->expr_type = GetType( SubResult[t][t] );
            opnd->op.result_type = opnd->expr_type;
        }
        break;
    }
#endif
    return( opnd );
}


TREEPTR UComplement( TREEPTR opnd )
{
    int         t;
    TYPEPTR     typ;

    opnd = RValue( opnd );
    if( opnd->op.opr != OPR_ERROR ) {
        typ = opnd->expr_type;
        SKIP_TYPEDEFS( typ );
        t = DataTypeOf( typ );
        if( t != TYPE_VOID ) {
            if( t >= TYPE_FLOAT ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
                opnd = ErrorNode( opnd );
            } else {
                opnd = ExprNode( 0, OPR_COM, opnd );
                opnd->expr_type = GetType( SubResult[t][t] );
                opnd->op.result_type = opnd->expr_type;
            }
        }
    }
#if 0
    switch( opnd->op.opr ) {
    case OPR_ERROR:
        break;
    case OPR_PUSHINT:
        switch( opnd->op.const_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
            opnd->op.long_value = (char)~opnd->op.long_value;
            break;
        case TYPE_SHORT:
        case TYPE_USHORT:
            opnd->op.long_value = (short)~opnd->op.long_value;
            break;
        case TYPE_INT:
            opnd->op.long_value = (target_int)~opnd->op.long_value;
            break;
        case TYPE_UINT:
            opnd->op.ulong_value = (target_uint)~opnd->op.ulong_value;
            break;
        case TYPE_LONG:
        case TYPE_ULONG:
            opnd->op.ulong_value = ~opnd->op.ulong_value;
            break;
        }
        break;
    default:
        typ = opnd->expr_type;
        SKIP_TYPEDEFS( typ );
        t = DataTypeOf( typ );
        if( t == TYPE_VOID ) break;
        if( t >= TYPE_FLOAT ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            opnd = ErrorNode( opnd );
        } else {
            opnd = ExprNode( 0, OPR_COM, opnd );
            opnd->expr_type = GetType( SubResult[t][t] );
            opnd->op.result_type = opnd->expr_type;
        }
        break;
    }
#endif
    return( opnd );
}


local TYPEPTR MergedType( TYPEPTR typ1, TYPEPTR typ2 )  /* 25-jul-90 */
{
    type_modifiers  flags, new_flags;
    TYPEPTR         typ;
/*
(type huge *) : (type *)                        -> (type huge *)
(type *) : (type huge *)                        -> (type huge *)
(type far *) : (type *)                 -> (type far *)
(type *) : (type far *)                 -> (type far *)
(type const *) : (type *)                       -> (type const *)
(type *) : (type const *)                       -> (type const *)
(type volatile *) : (type *)                    -> (type volatile *)
(type *) : (type volatile *)                    -> (type volatile *)
            etc.
*/
    typ = typ1;
    flags = typ1->u.p.decl_flags | typ2->u.p.decl_flags;
    new_flags = flags & MASK_CV_QUALIFIERS;
    if( flags & FLAG_HUGE ) {
        new_flags |= FLAG_HUGE;
    } else if( flags & FLAG_FAR ) {
        new_flags |= FLAG_FAR;
    } else if( (typ1->u.p.decl_flags & FLAG_NEAR)   /* 12-may-91 */
           &&  (typ2->u.p.decl_flags & FLAG_NEAR) ) {
        new_flags |= FLAG_NEAR;
    }
    if( typ1->u.p.decl_flags != typ2->u.p.decl_flags ) {
        typ = PtrNode( typ1->object, new_flags, typ1->u.p.segment );
    }
    return( typ );
}


TYPEPTR TernType( TREEPTR true_part, TREEPTR false_part )
/*******************************************************/
{
    TYPEPTR          typ1;
    TYPEPTR          typ2;
    DATA_TYPE        dtype1, dtype2;

    typ1 = true_part->expr_type;
    SKIP_TYPEDEFS( typ1 );
    typ2 = false_part->expr_type;
    SKIP_TYPEDEFS( typ2 );
/*
    (type1) : (type1)                           -> (type1)
    nb. structs, unions, and identical pointers are handled here
*/
    if( typ1 == typ2 ) return( typ1 );
    dtype1 = DataTypeOf( typ1 );
    dtype2 = DataTypeOf( typ2 );
    if( dtype1 == TYPE_POINTER && false_part->op.opr == OPR_PUSHINT ) {
        if( false_part->op.long_value != 0 ) {
            CWarn1( WARN_NONPORTABLE_PTR_CONV, ERR_NONPORTABLE_PTR_CONV );
        }
        return( MergedType( typ1, typ2 ) ); /* merge near/far/const etc. */
    }
    if( dtype2 == TYPE_POINTER && true_part->op.opr == OPR_PUSHINT ) {
        if( true_part->op.long_value != 0 ) {
            CWarn1( WARN_NONPORTABLE_PTR_CONV, ERR_NONPORTABLE_PTR_CONV );
        }
        return( MergedType( typ2, typ1 ) ); /* merge near/far/const etc. */
    }
/*
    (arithmetic type) : (arithmetic type)       -> (promoted arithmetic type)
*/
    if( (dtype1 <= TYPE_DOUBLE) && (dtype2 <= TYPE_DOUBLE) ) {
        return( GetType( SubResult[dtype1][dtype2] ) );
    }
    TernChk( typ1, typ2 );
    if( dtype1 == TYPE_POINTER && dtype2 == TYPE_POINTER ) {
/*
    (void *) : (anything *)                     -> (void *)
*/

        if( typ1->object->decl_type == TYPE_VOID ) {
            return( MergedType( typ1, typ2 ) );
        } else if( typ2->object->decl_type == TYPE_VOID ) {
            return( MergedType( typ2, typ1 ) );
        } else {
            return( MergedType( typ1, typ2 ) );
        }
    }
    return( typ1 );
}
