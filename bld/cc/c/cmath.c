/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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

#define CHR     TYP_CHAR
#define UCH     TYP_UCHAR
#define SHT     TYP_SHORT
#define USH     TYP_USHORT
#define INT     TYP_INT
#define UIN     TYP_UINT
#define LNG     TYP_LONG
#define ULN     TYP_ULONG
#define LN8     TYP_LONG64
#define UL8     TYP_ULONG64
#define FLT     TYP_FLOAT
#define DBL     TYP_DOUBLE
#define LDB     TYP_LONG_DOUBLE
#define FIM     TYP_FIMAGINARY
#define DIM     TYP_DIMAGINARY
#define LIM     TYP_LDIMAGINARY
#define BOL     TYP_BOOL
#define PTR     TYP_POINTER
#define ARR     TYP_ARRAY
#define STC     TYP_STRUCT
#define FCX     TYP_FCOMPLEX
#define DCX     TYP_DCOMPLEX
#define LCX     TYP_LDCOMPLEX

#define ERR     TYP_ERROR       /* no real type behind this value */

/*
 * define macros for promoted types
 */
#if TARGET_INT == 4

    /*
     * Promoted Unsigned Short is Signed Int
     */
    #define PUS     TYP_INT
    /*
     * Promoted Unsigned Int is Unsigned Long
     */
    #define PUI     TYP_ULONG

#else /* 16-bit ints */

    /*
     * Promoted Unsigned Short is Unsigned Int
     */
    #define PUS     TYP_UINT
    /*
     * Promoted Unsigned Int is Signed Long
     */
    #define PUI     TYP_LONG

#endif

/*
 * matches enum DATA_TYPE in ctypes.h
 */
static const unsigned char  AddResult[DATA_TYPE_SIZE][DATA_TYPE_SIZE] = {
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

/*
 * matches enum DATA_TYPE in ctypes.h
 */
static const unsigned char  SubResult[DATA_TYPE_SIZE][DATA_TYPE_SIZE] = {
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

/*
 * matches enum DATA_TYPE in ctypes.h
 */
static const unsigned char  IntResult[DATA_TYPE_SIZE][DATA_TYPE_SIZE] = {
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

/*
 * matches enum DATA_TYPE in ctypes.h
 */
static char ShiftResult[DATA_TYPE_SIZE] = {
/* >>      op2 */
/* BOL */  INT,
/* CHR */  INT,
/* UCH */  INT,         /* (used to be UIN) */
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

/*
 * matches enum DATA_TYPE in ctypes.h
 */
static const unsigned char  BinResult[DATA_TYPE_SIZE][DATA_TYPE_SIZE] = {
/*  +       BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { BOL,INT,UCH,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* CHR */ { INT,CHR,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UCH */ { UCH,INT,UCH,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* SHT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* USH */ { PUS,PUS,PUS,PUS,PUS,PUS,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* INT */ { INT,INT,INT,INT,PUS,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UIN */ { UIN,UIN,UIN,UIN,UIN,UIN,UIN,PUI,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LNG */ { LNG,LNG,LNG,LNG,LNG,LNG,PUI,LNG,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* ULN */ { ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,ULN,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LN8 */ { LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,LN8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* UL8 */ { UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,UL8,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* FLT */ { FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,FLT,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* DBL */ { DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,DBL,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
/* LDB */ { LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,LDB,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR, },
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
    C2S,    /* char to short            */
    C2I,    /* char to int              */
    C2L,    /* char to long             */
    C2M,    /* char to long long        */
    C2U,    /* char to unsigned         */
    C2F,    /* char to float            */
    C2D,    /* char to double           */
    C2T,    /* char to long double      */
    S2C,    /* short to char            */
    S2I,    /* short to int             */
    S2L,    /* short to long            */
    S2M,    /* short to long long       */
    S2U,    /* short to unsigned        */
    S2F,    /* short to float           */
    S2D,    /* short to double          */
    S2T,    /* short to long double     */
    I2C,    /* int to char              */
    I2S,    /* int to short             */
    I2U,    /* int to unsigned          */
    I2L,    /* int to long              */
    I2M,    /* int to long long         */
    I2F,    /* int to float             */
    I2D,    /* int to double            */
    I2T,    /* int to long double       */
    L2C,    /* long to char             */
    L2S,    /* long to short            */
    L2I,    /* long to integer          */
    L2M,    /* long to long long        */
    L2U,    /* long to unsigned         */
    L2F,    /* long to float            */
    L2D,    /* long to double           */
    L2T,    /* long to long double      */
    M2C,    /* long long to char        */
    M2S,    /* long long to short       */
    M2I,    /* long long to integer     */
    M2L,    /* long long to long        */
    M2U,    /* long long to unsigned    */
    M2F,    /* long long to float       */
    M2D,    /* long long to double      */
    M2T,    /* long long to long double */
    U2C,    /* unsigned to char         */
    U2L,    /* unsigned to long         */
    U2M,    /* unsigned to long long    */
    U2F,    /* unsigned to float        */
    U2D,    /* unsigned to double       */
    U2T,    /* unsigned to long double  */
    F2C,    /* float to char            */
    F2S,    /* float to short           */
    F2I,    /* float to int             */
    F2L,    /* float to long            */
    F2M,    /* float to long long       */
    F2D,    /* float to double          */
    F2T,    /* float to long double     */
    D2C,    /* double to char           */
    D2S,    /* double to short          */
    D2I,    /* double to int            */
    D2L,    /* double to long           */
    D2M,    /* double to long long      */
    D2F,    /* double to float          */
    D2T,    /* double to long double    */
    T2C,    /* long double to char      */
    T2S,    /* long double to short     */
    T2I,    /* long double to int       */
    T2L,    /* long double to long      */
    T2M,    /* long double to long long */
    T2F,    /* long double to float     */
    T2D,    /* long double to double    */
    P2P,    /* pointer to pointer       */
    P2A,    /* pointer to arithmetic    */
    A2P,    /* arithmetic to pointer    */
    S2B,    /* scalar to _Bool          */
    B2S,    /* _Bool to scalar          */
    CER,    /* conversion error         */
};

/*
 * must be implemented for long double
 * now mapped to error
 */
#define C2T     CER
#define S2T     CER
#define I2T     CER
#define U2T     CER
#define L2T     CER
#define M2T     CER
#define F2T     CER
#define D2T     CER
#define T2C     CER
#define T2S     CER
#define T2I     CER
#define T2L     CER
#define T2M     CER
#define T2F     CER
#define T2D     CER

/*
 * matches enum DATA_TYPE in ctypes.h
 * When indexing, row is the source type, column is the target.
 */
static enum  conv_types const CnvTable[DATA_TYPE_SIZE][DATA_TYPE_SIZE] = {
/*          BOL,CHR,UCH,SHT,USH,INT,UIN,LNG,ULN,LN8,UL8,FLT,DBL,LDB,FIM,DIM,LIM,PTR,ARR,STC,UNI,FNC,FLD,VOD,ENM,TDF,UFD,...,PCH,WCH,FCX,DCX,LCX, */
/* BOL */ { NIL,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,B2S,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* CHR */ { S2B,NIL,C2U,C2S,C2S,C2I,C2U,C2L,C2L,C2M,C2M,C2F,C2D,C2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UCH */ { S2B,U2C,NIL,C2S,C2S,C2I,C2U,C2L,C2L,C2M,C2M,C2F,C2D,C2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* SHT */ { S2B,S2C,S2C,NIL,S2U,S2I,S2U,S2L,S2L,S2M,S2M,S2F,S2D,S2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* USH */ { S2B,S2C,S2C,S2U,NIL,S2I,S2U,S2L,S2L,S2M,S2M,S2F,S2D,S2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* INT */ { S2B,I2C,I2C,I2S,I2S,NIL,I2U,I2L,I2L,I2M,I2M,I2F,I2D,I2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UIN */ { S2B,I2C,I2C,I2S,I2S,I2U,NIL,U2L,U2L,U2M,U2M,U2F,U2D,U2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LNG */ { S2B,L2C,L2C,L2S,L2S,L2I,L2U,NIL,L2U,L2M,L2M,L2F,L2D,L2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* ULN */ { S2B,L2C,L2C,L2S,L2S,L2I,L2U,L2U,NIL,L2M,L2M,L2F,L2D,L2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LN8 */ { S2B,M2C,M2C,M2S,M2S,M2I,M2U,M2L,M2L,NIL,M2U,M2F,M2D,M2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* UL8 */ { S2B,M2C,M2C,M2S,M2S,M2I,M2U,M2L,M2L,M2U,NIL,M2F,M2D,M2T,CER,CER,CER,A2P,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* FLT */ { S2B,F2C,F2C,F2S,F2S,F2I,F2I,F2L,F2L,F2M,F2M,NIL,F2D,F2T,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* DBL */ { S2B,D2C,D2C,D2S,D2S,D2I,D2I,D2L,D2L,D2M,D2M,D2F,NIL,D2T,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
/* LDB */ { S2B,T2C,T2C,T2S,T2S,T2I,T2I,T2L,T2L,T2M,T2M,T2F,T2D,NIL,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER,CER, },
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

static opr_code Operator[] = {
    #define OPERATORS_ONLY
    #define pick(token,string,class,oper,prec) oper,
    #include "_ctokens.h"
    #undef pick
    #undef OPERATORS_ONLY
};

static opr_code TokenToOperator( TOKEN token )
{
    return( Operator[token] );
}

static DATA_TYPE BinExprTypeDT( DATA_TYPE typ1, DATA_TYPE typ2 )
{
    DATA_TYPE   data_type;

    data_type = BinResult[typ1][typ2];
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

    typ = node->u.expr_type;
    SKIP_TYPEDEFS( typ );
    return( typ );
}

DATA_TYPE DataTypeOf( TYPEPTR typ )
{
    DATA_TYPE data_type;

    data_type = typ->decl_type;
    switch( data_type ) {
    case TYP_ENUM:
        return( typ->object->decl_type );  /* true size of enum */
    case TYP_FIELD:
    case TYP_UFIELD:
        return( typ->u.f.field_type );     /* true bitfield type */
    default:
        break;
    }
    return( data_type );
}

static pointer_class PointerClass( TYPEPTR typ )
/***********************************************
 * 0 - near data
 * 1 - based data
 * 2 - far data
 * 3 - far16 data
 * 4 - huge data
 * 5 - interrupt
 * 6 - near func
 * 7 - based func
 * 8 - far func
 * 9 - far16 func
 * 10 - huge func
 * 11 - interrupt func
 * 12 - NOT
 */
{
    type_modifiers    flags;
    pointer_class     ptrclass;

    flags = typ->u.p.decl_flags;
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    ptrclass = PTRCLS_NEAR;                // assume NEAR
    if( (flags & FLAG_INTERRUPT) == FLAG_INTERRUPT ) {
        ptrclass = PTRCLS_INTERRUPT;       // can't have huge functions
    } else if( flags & FLAG_BASED ) {
        ptrclass = PTRCLS_BASED;
    } else if( flags & FLAG_FAR ) {
        ptrclass = PTRCLS_FAR;
    } else if( flags & FLAG_FAR16 ) {
        ptrclass = PTRCLS_FAR16;
    } else if( flags & FLAG_HUGE ) {
        ptrclass = PTRCLS_HUGE;
#if _INTEL_CPU
    } else {
        if( typ->decl_type == TYP_FUNCTION ) {
            if( TargetSwitches & CGSW_X86_BIG_CODE ) {
                ptrclass = PTRCLS_FAR;
            }
        } else {
            if( TargetSwitches & CGSW_X86_BIG_DATA ) {
                ptrclass = PTRCLS_FAR;
            }
        }
#endif
    }
    if( typ->decl_type == TYP_FUNCTION )
        ptrclass += PTRCLS_FUNC;
    return( ptrclass );
}

pointer_class ExprTypeClass( TYPEPTR typ )
{
    TYPEPTR     savtyp;

    savtyp = typ;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYP_POINTER ) {
        return( PointerClass( savtyp ) );
    }
    return( PTRCLS_NOT );           // indicate not a pointer type
}



#define Convert( opnd, opnd_type, result_type )     opnd

static int NumSize( DATA_TYPE op_type )
/**************************************
 * return 0 not a num, else number of bits | SIGN_BIT if signed
 */
{
    int     size;

    size = 0;
    switch( op_type ) {
    case TYP_BOOL:
        size = 1;
        break;
    case TYP_CHAR:
        size = SIGN_BIT;
        /* fall through */
    case TYP_UCHAR:
        size |= 8;
        break;
    case TYP_SHORT:
        size = SIGN_BIT;
        /* fall through */
    case TYP_USHORT:
        size |= 16;
        break;
    case TYP_LONG:
        size = SIGN_BIT;
        /* fall through */
    case TYP_ULONG:
    case TYP_POINTER:
        size |= 32;
        break;
    case TYP_LONG64:
        size = SIGN_BIT;
        /* fall through */
    case TYP_ULONG64:
        size |= 64;
        break;
    case TYP_INT:
        size = SIGN_BIT;
        /* fall through */
    case TYP_UINT:
#if TARGET_INT == 2
        size |= 16;
#else
        size |= 32;
#endif
        break;
    }
    return( size );
}

static int NumSizeType( TYPEPTR typ )
/************************************
 * return 0 not a num, else number of bits | SIGN_BIT if signed
 */
{
    int     size;

    size = NumSize( typ->decl_type );
    if( typ->decl_type == TYP_FIELD ) {
        size = SIGN_BIT | typ->u.f.field_width ;
    } else if( typ->decl_type == TYP_UFIELD ) {
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
    if( typ_op2->decl_type != TYP_LONG64 && typ_op2->decl_type != TYP_ULONG64 ) {
        if( typ_op2->decl_type == TYP_ULONG || typ_op2->decl_type == TYP_UINT ) {
            U32ToU64( val.u._32[0], &val );
        } else {
            I32ToI64( val.u._32[0], &val );
        }
    }

    rev_ret = false;
    switch( opr ) { // mapped rel ops to equivalent cases
    case T_NE:
        rev_ret = true;
        /* fall through */
    case T_EQ:
        rel = REL_EQ;
        break;
    case T_GE:
        rev_ret = true;
        /* fall through */
    case T_LT:
        rel = REL_LT;
        break;
    case T_GT:
        rev_ret = true;
        /* fall through */
    case T_LE:
        rel = REL_LE;
        break;
    default:
        assert( 0 );
        rel = REL_EQ;
        break;
    }
    isBitField = ( typ_op1->decl_type == TYP_FIELD || typ_op1->decl_type == TYP_UFIELD );
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
            /*
             * special case for unsigned <= 0
             */
            ret = CMP_COMPLEX;
        }
    }
    return( ret );
}


static int ReverseRelOp( int opr )
/*********************************
 * map opr to reversed operand equivalent
 */
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


static TREEPTR BaseConv( TYPEPTR typ1, TREEPTR op2 )
{
    TYPEPTR         typ2;
#if _INTEL_CPU
    type_modifiers  typ1_flags;
#endif
    type_modifiers  typ2_flags;

    typ2 =  op2->u.expr_type;
    /*
     * skip typedefs, go into enum base
     */
    typ1 = SkipTypeFluff( typ1 );
    /*
     * skip typedefs, go into enum base
     */
    typ2 = SkipTypeFluff( typ2 );
#if _INTEL_CPU
    typ1_flags = typ1->u.p.decl_flags;
#endif
    typ2_flags = typ2->u.p.decl_flags;
    if( typ1->decl_type == TYP_POINTER && typ2->decl_type == TYP_POINTER ) {
#if _INTEL_CPU
        if( typ2_flags & FLAG_BASED ) {
            if( (typ1_flags & MASK_ALL_MEM_MODELS) == FLAG_NONE ) {
                if( typ1->object->decl_type == TYP_FUNCTION ) {
                    if( TargetSwitches & CGSW_X86_BIG_CODE ) {
                        typ1_flags = FLAG_FAR;
                    }
                } else {
                    if( TargetSwitches & CGSW_X86_BIG_DATA ) {
                        typ1_flags = FLAG_FAR;
                    }
                }
            }
            if( (typ1_flags & FLAG_FAR) ) {
                op2 = BasedPtrNode( typ2, op2 );
            }
        }
#endif
    } else if( typ2->decl_type == TYP_POINTER ) {
        /*
         * If we're converting a based pointer to some larger arithmetic type,
         * we must convert it to a long pointer first to get proper segment.
         * However, in flat model this conversion isn't done, we pretend
         * that segments don't exist.
         */
        if( TypeSize( typ1 ) > TypeSize( typ2 ) ) {
#if _CPU == 386
            if( (TargetSwitches & CGSW_X86_FLAT_MODEL) == 0 && (typ2_flags & FLAG_BASED) ) {
#else
            if( typ2_flags & FLAG_BASED ) {
#endif
                op2 = BasedPtrNode( typ2, op2 );
//                op2 = CnvOp( op2, PtrNode( typ2->object, FLAG_FAR, SEG_NULL ), true );
            }
        }
    } else if( typ1->decl_type == TYP_POINTER ) {
        /*
         * If we're converting an arithmetic type to a pointer, first convert
         * it to appropriately sized integer to correctly extend/truncate.
         */
        if( TypeSize( typ1 ) != TypeSize( typ2 ) ) {
            op2 = CnvOp( op2, GetIntTypeBySize( TypeSize( typ1 ), false, false ), true );
        }
    }
    return( op2 );
}


static TREEPTR BoolConv( TYPEPTR typ, TREEPTR tree )
/***************************************************
 * Convert a scalar to boolean
 */
{
    if( tree->op.opr == OPR_ERROR ) {
        return( ErrorNode( tree ) );
    }
    SKIP_TYPEDEFS( typ );
    /*
     * Non-boolean types need to be converted to _Bool; _Bool expressions
     * also need to be converted unless they're constants.
     */
    if( DataTypeOf( typ ) == TYP_BOOL && TypeOf( tree ) != typ ) {
        tree = ExprNode( tree, OPR_QUESTION, ExprNode( IntLeaf( 1 ), OPR_COLON, IntLeaf( 0 ) ) );
        typ = GetType( TYP_BOOL );
        tree->op.u2.result_type = typ;
        tree->u.expr_type = typ;
        FoldExprTree( tree );
    }
    return( tree );
}


static bool IsInt( DATA_TYPE op )
/********************************
 * what's target compatible between default int as ret type
 * and a later declaration
 */
{
    bool        ret;

    switch( op ) {
    case TYP_BOOL:
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_SHORT:
    case TYP_USHORT:
    case TYP_INT:
    case TYP_LONG:
    case TYP_LONG64:
    case TYP_ULONG64:
        ret = true;
        break;
    default:
        ret = false;
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
    if( typ1->decl_type == TYP_POINTER && typ2->decl_type == TYP_POINTER ) {
        op2 = BaseConv( typ1, op2 );
        op1 = BaseConv( typ2, op1 );
        typ1 = TypeOf( op1 );
        typ2 = TypeOf( op2 );
    }
    op1_type = DataTypeOf( typ1 );
    op2_type = DataTypeOf( typ2 );
    cmp_type = typ1;
    result_type = op1_type;
    /*
     * check for meaningless comparison:
     * TODO this would be a better check maybe in foldtree
     */
    if( IS_PPCTL_NORMAL() ) {
        cmp_cc = CMP_VOID;
        if( op2->op.opr == OPR_PUSHINT ) {
            cmp_cc = IsMeaninglessCompare( op2->op.u2.long64_value, typ1, typ2, opr );
        } else if( op1->op.opr == OPR_PUSHINT ) {
            cmp_cc = IsMeaninglessCompare( op1->op.u2.long64_value, typ2, typ1, ReverseRelOp( opr ) );
        }
        if( cmp_cc != CMP_VOID ) {
            if( cmp_cc == CMP_COMPLEX ) {
                CWarn1( ERR_COMPARE_UNSIGNED_VS_ZERO );
            } else {
                int res = ( cmp_cc == CMP_TRUE );

                CWarn2( ERR_COMPARE_ALWAYS, res );
            }
        }
    }
    if( op1_type == TYP_VOID || op2_type == TYP_VOID ) {
        ;           /* do nothing, since error has already been given */
    } else if( op1_type == TYP_POINTER && op2_type == TYP_POINTER ) {
         CompatiblePtrType( typ1, typ2, opr );
         if( TypeSize( typ2 ) > TypeSize( typ1 ) ) {
             /*
              * Make sure near pointer is converted to far if necessary
              */
             cmp_type = typ2;
         }
    } else if( (op1_type == TYP_POINTER && IsInt( op2_type )) ||
               (op2_type == TYP_POINTER && IsInt( op1_type )) ) {
        /*
         * ok to compare pointer with constant 0
         */
        if( opr != T_EQ && opr != T_NE ) {
            CWarn1( ERR_POINTER_TYPE_MISMATCH );
        } else if( !CheckZero( op1 ) && !CheckZero( op2 ) ) {
            CWarn1( ERR_NON_ZERO_CONST );
        }
        if( op2_type == TYP_POINTER ) {
            cmp_type = typ2;
        }
    } else if( op1_type == TYP_STRUCT || op1_type == TYP_UNION
      || op2_type == TYP_STRUCT || op2_type == TYP_UNION ) {
        CErr1( ERR_INVALID_RELOP_FOR_STRUCT_OR_UNION );
        result_type = ERR;
    } else {
        result_type = BinExprTypeDT( op1_type, op2_type );
        if( result_type == ERR ) {
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
            tree->op.u1.cc = CC_EQ;
            break;
        case T_NE:
            tree->op.u1.cc = CC_NE;
            break;
        case T_LT:
            tree->op.u1.cc = CC_LT;
            break;
        case T_LE:
            tree->op.u1.cc = CC_LE;
            break;
        case T_GT:
            tree->op.u1.cc = CC_GT;
            break;
        case T_GE:
            tree->op.u1.cc = CC_GE;
            break;
        default:
            assert( 0 );
            tree->op.u1.cc = 0;
            break;
        }
        tree->op.u2.compare_type = cmp_type;
        tree->u.expr_type = GetType( TYP_INT );
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
            if( CheckZeroConstant( op1 ) ) {
                FreeExprNode( op1 );
                return( op2 );
            }
        } else {        // OPR_AND_AND
            if( !CheckZeroConstant( op1 ) ) {
                FreeExprNode( op1 );
                return( op2 );
            }
        }
    }
    if( op2->op.opr == OPR_PUSHINT ) {
        if( opr == OPR_OR_OR ) {
            if( CheckZeroConstant( op2 ) ) {
                FreeExprNode( op2 );
                return( op1 );
            }
        } else {        // OPR_AND_AND
            if( !CheckZeroConstant( op2 ) ) {
                FreeExprNode( op2 );
                return( op1 );
            }
        }
    }
    tree = ExprNode( op1, opr, op2 );
    tree->u.expr_type = GetType( TYP_INT );
    tree->op.u2.label_index = NextLabel();
    return( tree );
}


static TREEPTR MulByConst( TREEPTR opnd, target_ssize amount )
{
    TREEPTR     tree;

#if 0
    if( opnd->op.opr == OPR_PUSHINT ) {
        switch( opnd->op.u1.const_type ) {
        case TYP_LONG64:
        case TYP_ULONG64:
          {
            signed_64   val64;

            I32ToI64( amount, &val64 );
            U64Mul( &opnd->op.u2.long64_value, &val64, &opnd->op.u2.long64_value );
          } break;
        default:
            opnd->op.u2.long_value *= amount;
        }
        return( opnd );
    }
#else
    if( opnd->op.opr == OPR_PUSHINT ) {
        opnd->op.u2.long_value *= amount;
        return( opnd );
    }
#endif

    switch( TypeOf( opnd )->decl_type ) {
#if 0
    case TYP_LONG64:
    case TYP_ULONG64:
      {
        unsigned_64 val64;

        I32ToI64( amount, &val64 );
        tree = ExprNode( opnd, OPR_MUL, Long64Leaf( &val64 ) );
        tree->u.expr_type = GetType( TYP_LONG64 );
      } break;
#endif
    case TYP_LONG:
    case TYP_ULONG:
        tree = ExprNode( opnd, OPR_MUL, LongLeaf( amount ) );
        tree->u.expr_type = GetType( TYP_LONG );
        break;
    case TYP_INT:
    case TYP_UINT:
    default:
        tree = ExprNode( opnd, OPR_MUL, IntLeaf( amount ) );
        tree->u.expr_type = GetType( TYP_INT );
    }
    tree->op.u2.result_type = tree->u.expr_type;
    return( tree );
}


static TREEPTR PtrSubtract( TREEPTR result, target_ssize size, DATA_TYPE result_type)
{
    int             shift_count;
    target_ssize    n;
    TREEPTR         tree;
    TYPEPTR         typ;

    typ = GetType( result_type );
    result->u.expr_type = typ;
    result->op.u2.result_type = typ;
    for( n = 2, shift_count = 1; shift_count < 8; ++shift_count ) {
        if( n == size ) {
            tree = ExprNode( result, OPR_RSHIFT, IntLeaf( shift_count ) );
            tree->u.expr_type = typ;
            tree->op.u2.result_type = typ;
            return( tree );
        }
        n *= 2;
    }
    if( result_type == INT ) {
        tree = ExprNode( result, OPR_DIV, IntLeaf( size ) );
        tree->u.expr_type = GetType( TYP_INT );
    } else {
        tree = ExprNode( result, OPR_DIV, LongLeaf( size ) );
        tree->u.expr_type = GetType( TYP_LONG );
    }
    tree->op.u2.result_type = tree->u.expr_type;
    return( tree );
}


TREEPTR LCastAdj( TREEPTR tree )
/*******************************
 * Remove the OPR_CONVERT for lcast so it looks like an LVALUE
 */
{
    TREEPTR         opnd;
    TYPEPTR         typ;
    type_modifiers  modifiers;

    opnd = tree->right;
    tree->right = NULL;
    typ = tree->u.expr_type;
    opnd->u.expr_type = typ;
    FreeExprTree( tree );
    CWarn1( ERR_LVALUE_CAST );
    opnd->op.flags &= ~OPFLAG_RVALUE;
    if( opnd->op.opr == OPR_PUSHSYM ) {
        opnd->op.opr = OPR_PUSHADDR;
    } else if( opnd->op.opr == OPR_POINTS ) {
        /*
         * fix up fred's screw ball pointer op
         */
        modifiers = FlagOps( opnd->op.flags );
        opnd->op.u2.result_type = PtrNode( typ, modifiers, SEG_DATA );
    }
    return( opnd );
}


static TYPEPTR PtrofSym( SYM_HANDLE sym_handle, TYPEPTR typ )
{
    SYM_ENTRY   sym;

    SymGet( &sym, sym_handle );
    typ = PtrNode( typ, sym.mods, SEG_DATA );
    return( typ );
}


static TREEPTR ArrayPlusConst( TREEPTR op1, TREEPTR op2 )
{
    TREEPTR     result;
    TYPEPTR     typ;

    if( op2->op.opr == OPR_PUSHINT ) {
        if( op1->op.opr == OPR_PUSHADDR ) {
            typ = op1->u.expr_type;
            SKIP_TYPEDEFS( typ );
            if( typ->decl_type == TYP_ARRAY ) {

                op2->op.u2.long_value *= SizeOfArg( typ->object );
                typ = PtrofSym( op1->op.u2.sym_handle, typ->object );
                result = ExprNode( op1, OPR_ADD, op2 );
                result->u.expr_type = typ;
                result->op.u2.result_type = typ;
                return( result );
            }
        }
    }
    return( NULL );
}


static TREEPTR ArrayMinusConst( TREEPTR op1, TREEPTR op2 )
{
    TREEPTR     result;
    TYPEPTR     typ;

    if( op2->op.opr == OPR_PUSHINT ) {
        if( op1->op.opr == OPR_PUSHADDR ) {
            typ = op1->u.expr_type;
            SKIP_TYPEDEFS( typ );
            if( typ->decl_type == TYP_ARRAY ) {
                op2->op.u2.long_value =
                        (- op2->op.u2.long_value) * SizeOfArg( typ->object );
                typ = PtrofSym( op1->op.u2.sym_handle, typ->object );
                result = ExprNode( op1, OPR_ADD, op2 );
                result->u.expr_type = typ;
                result->op.u2.result_type = typ;
                return( result );
            }
        }
    }
    return( NULL );
}


static bool LValue( TREEPTR op1 )
{
    TYPEPTR     typ;

    if( op1->op.opr == OPR_ERROR )
        return( true );
    if( IsLValue( op1 ) ) {
        typ = TypeOf( op1 );
        if( typ->decl_type != TYP_ARRAY ) {
            if( TypeSize( typ ) == 0 ) {
                CErr1( ERR_INCOMPLETE_EXPR_TYPE );
            }
            if( op1->op.flags & OPFLAG_LVALUE_CAST ) {
                op1->op.flags &= ~(OPFLAG_LVALUE_CAST | OPFLAG_RVALUE);
                CWarn1( ERR_LVALUE_CAST );
            }
            return( true );
        }
    }
    CErr1( ERR_MUST_BE_LVALUE );
    return( false );
}


TREEPTR AddOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    DATA_TYPE           op1_type, op2_type, result_type;
    TYPEPTR             op1_tp, op2_tp;
    TYPEPTR             res_type;
    target_ssize        size;
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
        if( ( op1->op.opr == OPR_CONVERT || op1->op.opr == OPR_CONVERT_PTR )
          && CompFlags.extensions_enabled ) {
            op1 = LCastAdj( op1 );
        }
        LValue( op1 );
        break;
    case T_MINUS:
        result = ArrayMinusConst( op1, op2 );
        if( result != NULL )
            return( result );
        op1 = RValue( op1 );
        break;
    case T_PLUS:
        result = ArrayPlusConst( op1, op2 );    // check for array + const
        if( result != NULL )
            return( result );
        result = ArrayPlusConst( op2, op1 );    // check for const + array
        if( result != NULL ) {
            return( result );
        }
        /* fall through */
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
    result = NULL;
    if( op1_type == TYP_UNION || op1_type == TYP_STRUCT
      || op2_type == TYP_UNION || op2_type == TYP_STRUCT ) {
        result_type = ERR;
    } else if( op1_type == TYP_VOID || op2_type == TYP_VOID
      || op1_type == TYP_ARRAY ) {       /* ++array; or --array; */
        result_type = INT;
    } else {
        switch( opr ) {
        case T_PLUS:
            result_type = AddResult[op1_type][op2_type];
            break;
        case T_PLUS_PLUS:
        case T_MINUS_MINUS:
        case T_PLUS_EQUAL:
        case T_MINUS_EQUAL:
            if( op1_type == TYP_POINTER && op2_type >= TYP_FLOAT ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            }
            result_type = op1_type;
            break;
        default:
            result_type = SubResult[op1_type][op2_type];
            if(( op1_type == PTR )&&( op2_type == PTR )) {
                /*
                 * make sure both pointers are same type
                 */
                CompatiblePtrType( op1_tp, op2_tp, opr );
#if _INTEL_CPU
                if(( op1_tp->u.p.decl_flags & FLAG_HUGE ) ||
                   ( op2_tp->u.p.decl_flags & FLAG_HUGE ) ) {
                    result_type = LNG;
                } else if( IsHugeData() ) {
                    if( ((op1_tp->u.p.decl_flags & (FLAG_FAR | FLAG_NEAR)) == 0 )
                     && ((op2_tp->u.p.decl_flags & (FLAG_FAR | FLAG_NEAR)) == 0) ) {
                        result_type = LNG;
                    }
                }
#endif
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
        /*
         * subtraction of 2 pointers
         */
        if( (result_type == INT) || (result_type == LNG) ) {
            res_type = GetType( result_type );
        }
        if( size != 1 ) {
            /*
             * subtraction of 2 pointers
             */
            if( (result_type == INT) || (result_type == LNG) ) {
                result = ExprNode( op1, TokenToOperator( opr ), op2 );
                return( PtrSubtract( result, size, result_type ) );
#if _INTEL_CPU
            } else if( (op1_tp->u.p.decl_flags & FLAG_HUGE)
              || IsHugeData() ) {
                if( (op2_type != LNG) && (op2_type != ULN) ) {
                    op2 = CnvOp( op2, GetType( TYP_LONG ), true );
                }
#endif
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
#if _INTEL_CPU
            if( (op2_tp->u.p.decl_flags & FLAG_HUGE)
              || IsHugeData() ) {
                if( (op1_type != LNG ) && (op1_type != ULN) ) {
                    op2 = CnvOp( op2, GetType( TYP_LONG ), true );
                }
            }
#endif
            op1 = MulByConst( op1, size );
        }
    } else {
        res_type = GetType( result_type );
        op1 = Convert( op1, op1_type, result_type );
        op2 = Convert( op2, op2_type, result_type );
    }
    tree = ExprNode( op1, TokenToOperator( opr ), op2 );
    tree->u.expr_type = res_type;
    tree->op.u2.result_type = res_type;
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
    if( op1_type == TYP_VOID || op2_type == TYP_VOID ) {
        result_type = TYP_VOID;
    } else if( op1_type == TYP_UNION || op1_type == TYP_STRUCT
      || op2_type == TYP_UNION || op2_type == TYP_STRUCT ) {
        result_type = ERR;
    } else {
        result_type = BinExprTypeDT( op1_type, op2_type );
        if( opr == T_PERCENT && result_type >= TYP_FLOAT ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        }
        if( result_type < INT ) {
            result_type = INT;
        }
    }
    if( result_type == ERR ) {
        CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
        result_type = INT;
    }
    switch( opr ) {
    case T_XOR_EQUAL:
    case T_OR_EQUAL:
        /*
         * if op2 is a constant, check to see if constant truncated
         */
        if( CheckAssignRange( typ, op2 ) ) {
            CWarn1( ERR_CONSTANT_TOO_BIG );
        }
        /* fall through */
    case T_AND_EQUAL:
    case T_RSHIFT_EQUAL:
    case T_LSHIFT_EQUAL:
    case T_PERCENT_EQUAL:
        /*
         * check for integral operand.
         */
        result_type = IntResult[op1_type][op2_type];
        if( result_type == ERR ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        }
        /* fall through */
    case T_TIMES_EQUAL:
    case T_DIV_EQUAL:
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
        tree->u.expr_type = GetType( result_type );
        tree->op.u2.result_type = typ;
    }
    return( tree );
}


static void SetSymAssigned( TREEPTR opnd )
{
    SYM_ENTRY   sym;

    while( opnd->op.opr == OPR_DOT || opnd->op.opr == OPR_INDEX )
        opnd = opnd->left;
    if( opnd->op.opr == OPR_PUSHADDR ) {
        SymGet( &sym, opnd->op.u2.sym_handle );
        if( sym.level != 0 ) {
            if( (sym.flags & SYM_ASSIGNED) == 0 ) {
                sym.flags |= SYM_ASSIGNED;
                SymReplace( &sym, opnd->op.u2.sym_handle );
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
        CheckParmAssign( typ, op2, 0, true );
    }
    return( op2 );
}


TREEPTR AsgnOp( TREEPTR op1, TOKEN opr, TREEPTR op2 )
{
    TYPEPTR         typ;
    pointer_class   op1_ptrclass;
    pointer_class   op2_ptrclass;
    int             isLValue;

    if( op1->op.opr == OPR_ERROR ) {
        FreeExprTree( op2 );
        return( op1 );
    }
    if( op2->op.opr == OPR_ERROR ) {
        FreeExprTree( op1 );
        return( op2 );
    }
    if( op1->op.flags & OPFLAG_LVALUE_CAST ) {
        if( CompFlags.extensions_enabled ) {
            op1->op.flags &= ~(OPFLAG_LVALUE_CAST | OPFLAG_RVALUE);
            if( op1->op.opr == OPR_PUSHSYM ) {
                op1->op.opr = OPR_PUSHADDR;
            }
            CWarn1( ERR_LVALUE_CAST );
        } else {
            CErr1( ERR_CANT_TAKE_ADDR_OF_RVALUE );
        }
    }
    if( ( op1->op.opr == OPR_CONVERT || op1->op.opr == OPR_CONVERT_PTR )
      && CompFlags.extensions_enabled ) {
        op1 = LCastAdj( op1 );
    }
    isLValue = LValue( op1 );
    if( isLValue ) {
        op_flags volatile_flag;

        volatile_flag = op1->op.flags & OPFLAG_VOLATILE;
        if( opr != T_EQUAL && opr != T_ASSIGN_LAST ) {
            if( opr == T_PLUS_EQUAL || opr == T_MINUS_EQUAL ) {
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
        CheckParmAssign( typ, op2, 0, true );
        op2 = BaseConv( typ, op2 );
        op2 = BoolConv( typ, op2 );
        if( opr == T_ASSIGN_LAST )
            opr = T_EQUAL;
        op1_ptrclass = ExprTypeClass( typ );
        op2_ptrclass = ExprTypeClass( op2->u.expr_type );
        if( op1_ptrclass != op2_ptrclass ) {
            if( FAR16_PTRCLASS( op1_ptrclass ) || FAR16_PTRCLASS( op2_ptrclass ) ) {
                /*
                 * if far16 pointer
                 */
                op2 = ExprNode( NULL, OPR_CONVERT_PTR, op2 );
                op2->op.u2.sp.old_ptrclass = op2_ptrclass;
                op2->op.u2.sp.new_ptrclass = op1_ptrclass;
            } else {
                op2 = ExprNode( NULL, OPR_CONVERT, op2 );
                op2->op.u2.result_type = typ;
            }
            op2->u.expr_type = typ;
        }
        op1 = ExprNode( op1, TokenToOperator( opr ), op2 );
        op1->op.flags |= volatile_flag;
        op1->u.expr_type = typ;
        op1->op.u2.result_type = typ;
    } else {
        FreeExprTree( op2 );
    }
    return( op1 );
}


void CheckConst( TREEPTR opnd )
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
    if( op1_type == TYP_VOID || op2_type == TYP_VOID ) {
        result_type = TYP_VOID;
    } else if( op1_type == TYP_UNION || op2_type == TYP_UNION ) {
        result_type = ERR;
    } else {
        result_type = IntResult[op1_type][op2_type];
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
        op1->u.expr_type = GetType( result_type );
        op1->op.u2.result_type = op1->u.expr_type;
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
    if( op1_type == TYP_VOID || op2_type == TYP_VOID ) {
        result_type = TYP_VOID;
    } else {
        if( op1_type <= TYP_STRUCT ) {
            result_type = ShiftResult[op1_type];
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
        op1->u.expr_type = GetType( result_type );
        op1->op.u2.result_type = op1->u.expr_type;
    }
    return( op1 );
}


bool IsFuncPtr( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYP_POINTER )
        return( false );
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYP_FUNCTION )
        return( false );
    return( true );
}

bool IsPtrConvSafe( TREEPTR src, TYPEPTR newtyp, TYPEPTR oldtyp )
/****************************************************************
 * Check if pointer conversion is losing high (segment) bits.
 * A special dispensation is made for null pointer conversions; if the
 * source operand is a zero integer constant, chopping off high bits
 * is not considered an error. This helps in Win16 programming where
 * NULL (which may be a 32-bit quantity) is often converted to
 * a 16-bit HANDLE type.
 */
{
    bool                is_safe = true;
    segment_id          new_segid;
    segment_id          old_segid;
    type_modifiers      new_flags;
    type_modifiers      old_flags;

    SKIP_TYPEDEFS( oldtyp );
    SKIP_TYPEDEFS( newtyp );
    assert( oldtyp->decl_type == TYP_POINTER );
    assert( newtyp->decl_type == TYP_POINTER );
    /*
     * If new type isn't smaller than old, assume conversion is safe.
     */
    if( TypeSize( newtyp ) < TypeSize( oldtyp ) ) {
        is_safe = CheckZero( src );
        /*
         * Determine target pointer base.
         */
        new_flags = newtyp->u.p.decl_flags;
        old_flags = oldtyp->u.p.decl_flags;
        if( (new_flags & FLAG_BASED) || (old_flags & FLAG_BASED) ) {
            if( new_flags & FLAG_BASED ) {
                switch( newtyp->u.p.based_kind ) {
                case BASED_SEGNAME:
                    new_segid = newtyp->u.p.segid;
                    break;
                /*
                 * NYI: This could be smarter and check other based types.
                 */
                default:
                    new_segid = SEG_NULL;
                }
            } else if( IsFuncPtr( newtyp ) ) {
                new_segid = SEG_CODE;
            } else {
                new_segid = SEG_DATA;
            }
            /*
             * Determine source pointer base.
             */
            old_flags = oldtyp->u.p.decl_flags;
            if( old_flags & FLAG_BASED ) {
                switch( oldtyp->u.p.based_kind ) {
                case BASED_SEGNAME:
                    old_segid = oldtyp->u.p.segid;
                    break;
                /*
                 * NYI: This could be smarter and check other based types.
                 */
                default:
                    old_segid = SEG_NULL;
                }
            } else {
                old_segid = oldtyp->u.p.segid;
            }
            is_safe |= ( old_segid == new_segid );
        }
    }
    return( is_safe );
}

TREEPTR CnvOp( TREEPTR opnd, TYPEPTR newtyp, bool cast_op )
{
    TYPEPTR             typ;
    enum  conv_types    cnv;
    opr_code            opr;
    op_flags            flags;
    DATA_TYPE           opnd_type;

    if( opnd->op.opr == OPR_ERROR )
        return( opnd );
    SKIP_TYPEDEFS( newtyp );
    opr = opnd->op.opr;
    if( newtyp->decl_type == TYP_VOID ) {
        typ = TypeOf( opnd );
        if( typ->decl_type == TYP_VOID ) {
            return( opnd );
        }
    }
    flags = OPFLAG_NONE;
    if( cast_op && CompFlags.extensions_enabled ) {
        if( IsLValue( opnd ) ) {
            flags |= OPFLAG_LVALUE_CAST;
        }
    }
    opnd_type = opnd->u.expr_type->decl_type;
    if( IS_PPCTL_NORMAL() ) {
        opnd = RValue( opnd );
    }
    typ = TypeOf( opnd );
    SetDiagType2( newtyp, typ );
    if( newtyp->decl_type > TYP_POINTER ) {
        if( newtyp->decl_type == TYP_VOID ) {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->u.expr_type = newtyp;
            opnd->op.u2.result_type = newtyp;
            if( cast_op ) {
                CompFlags.meaningless_stmt = false;
            }
        } else if( newtyp->decl_type == TYP_ENUM ) {
            if( typ->decl_type == TYP_POINTER ) {
                CWarn1( ERR_POINTER_TYPE_MISMATCH );
            }
            newtyp = newtyp->object;
            goto convert;
        } else {
            if( cast_op ) {
                CErr1( ERR_MUST_BE_SCALAR_TYPE );
                SetDiagPop();
                return( ErrorNode( opnd ) );
            } else if( typ != newtyp ) {
                CErr1( ERR_TYPE_MISMATCH );
                SetDiagPop();
                return( ErrorNode( opnd ) );
            }
        }
    } else if( typ->decl_type != TYP_VOID ) {
convert:                                /* moved here */
        cnv = CnvTable[DataTypeOf( typ )]
                      [DataTypeOf( newtyp )];
        if( cnv == CER ) {
            CErr1( ERR_INVALID_CONVERSION );
            SetDiagPop();
            return( ErrorNode( opnd ) );
        } else if( cnv != NIL ) {
            if( cnv == P2P ) {
                if( (typ->u.p.decl_flags & MASK_ALL_MEM_MODELS)
                  != (newtyp->u.p.decl_flags & MASK_ALL_MEM_MODELS)
                  || ( opnd_type == TYP_ARRAY ) ) {
                    if( !IsPtrConvSafe( opnd, newtyp, typ ) ) {
                        if( cast_op ) {
                            CWarn1( ERR_CAST_POINTER_TRUNCATION );
                        } else {
                            CWarn1( ERR_POINTER_TRUNCATION );
                        }
                    }
                    if( !cast_op ) {
                        if( (typ->u.p.decl_flags & FLAG_BASED) &&
                            (newtyp->u.p.decl_flags & FLAG_FAR) ) {
                            opnd = BasedPtrNode( typ, opnd );
                            opnd->u.expr_type = newtyp;
                            opnd->op.u2.result_type = newtyp;
                            SetDiagPop();
                            return( opnd );
                        }
                        cast_op = true;        /* force a convert */
                    }
                } else if( IsFuncPtr( typ ) || IsFuncPtr( newtyp ) ) {
                    cast_op = true;    /* force a convert */
                } else if( TypeSize( typ ) != TypeSize( newtyp ) ) {
                    cast_op = true;    /* force a convert */
                } else if( typ->decl_type != TYP_POINTER || newtyp->decl_type != TYP_POINTER ) {
                    cast_op = true;    /* force a convert */
                } else if( opr == OPR_PUSHADDR && opnd->op.opr == OPR_ADDROF ) {
                    opnd->u.expr_type = newtyp;
                    SetDiagPop();
                    return( opnd );
                } else if( cast_op && CompFlags.extensions_enabled ) {
                    /*
                     * We know the following:
                     * - it is a cast operation
                     * - both types are pointers
                     * - extensions are enabled
                     * - both pointers are the same size
                     * - neither pointer is a function pointer
                     * So, if it is still an lvalue
                     * - then just update the type and leave it
                     * - as an lvalue. This will allow the
                     * - following statement to get through without
                     * - generating an error!
                     *              (char *)p += 2;
                     */
                    if( opr == OPR_PUSHADDR || IsLValue( opnd ) ) {
                        /*
                         * don't do it for based or far16.
                         */
                        if( !Far16Pointer( opnd->op.flags ) ) {
                            opnd->u.expr_type = newtyp;
                            opnd->op.opr = opr;
                            opnd->op.flags |= OPFLAG_LVALUE_CAST;
                            SetDiagPop();
                            return( opnd );
                        }
                    }
                }
            }
            if( !cast_op && cnv == P2A && TypeSize( typ ) > TypeSize( newtyp ) ) {
                CWarn1( ERR_POINTER_TRUNCATION );
            }
            if( cnv == P2A || cnv == A2P ) {
                if( TypeSize( typ ) != TypeSize( newtyp ) ) {
                    /*
                     * Conversion to/from a pointer may need special treatment
                     */
                    opnd = BaseConv( newtyp, opnd );
                }
            }
            if( cnv == S2B ) {
                /*
                 * Conversion to _Bool needs special treatment
                 */
                opnd = BoolConv( newtyp, opnd );
            }
            if( cast_op || cnv != P2P ) {
/* convert: moved */
                if( IsConstLeaf( opnd ) ) {
                    CastConstNode( opnd, newtyp );
                    opnd->u.expr_type = newtyp;
                } else {
                    pointer_class     new_ptrclass;
                    pointer_class     old_ptrclass;

                    new_ptrclass = ExprTypeClass( newtyp );
                    old_ptrclass = ExprTypeClass( typ );
                    if( new_ptrclass != old_ptrclass
                      && ( FAR16_PTRCLASS( new_ptrclass ) || FAR16_PTRCLASS( old_ptrclass ) ) ) {
                        /*
                         * foreign pointers
                         */
                        opnd = ExprNode( NULL, OPR_CONVERT_PTR, opnd );
                        opnd->op.u2.sp.old_ptrclass = old_ptrclass;
                        opnd->op.u2.sp.new_ptrclass = new_ptrclass;
#if _CPU == 8086
                    } else if( cnv == P2A && (newtyp->type_flags & TF2_TYP_SEGMENT) ) {
                        /*
                         * getting segment value of pointer
                         */
                        opnd = BasedPtrNode( typ, opnd );
                        opnd = ExprNode( NULL, OPR_CONVERT_SEG, opnd );
                        opnd->op.u2.result_type = newtyp;
#endif
                    } else {
                        opnd = ExprNode( NULL, OPR_CONVERT, opnd );
                        opnd->op.u2.result_type = newtyp;
                    }
                    opnd->u.expr_type = newtyp;
                }
            }
        } else if( IsConstLeaf( opnd ) ) {
            CastConstValue( opnd, newtyp->decl_type );
            opnd->u.expr_type = newtyp;
        } else if( opnd->u.expr_type != newtyp ) {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->u.expr_type = newtyp;
            opnd->op.u2.result_type = newtyp;
        } else { // NIL convert
            opnd->op.flags |= flags;
        }
    }
    SetDiagPop();
    return( opnd );
}

TREEPTR FixupAss( TREEPTR opnd, TYPEPTR newtyp )
/***********************************************
 * Fix up assignments - used for passing function arguments and also
 * return values. Fixes up based pointers and other x86 weirdnesses.
 *
 * TODO check out base ptrs
 */
{
    TYPEPTR             typ;
    enum conv_types     cnv;
    DATA_TYPE           decl1;
    DATA_TYPE           decl2;

    if( opnd->op.opr == OPR_ERROR )
        return( opnd );
    /*
     * skip typedefs, go into enum base
     */
    typ = SkipTypeFluff( opnd->u.expr_type );
    opnd = BaseConv( newtyp, opnd );
    opnd = BoolConv( newtyp, opnd );
    /*
     * skip typedefs, go into enum base
     */
    newtyp = SkipTypeFluff( newtyp );
    decl1 = DataTypeOf( typ );
    decl2 = DataTypeOf( newtyp );
    if( decl1 > TYP_POINTER || decl2 > TYP_POINTER ) {
        return( opnd );
    }
    cnv = CnvTable[decl1][decl2];
    if( cnv == CER ) {
        return(  opnd  );
    } else if( cnv == P2P ) {
        pointer_class     new_ptrclass;
        pointer_class     old_ptrclass;

        new_ptrclass = ExprTypeClass( newtyp );
        old_ptrclass = ExprTypeClass( typ );
        if( new_ptrclass != old_ptrclass
          && ( FAR16_PTRCLASS( new_ptrclass ) || FAR16_PTRCLASS( old_ptrclass ) ) ) {
            /*
             * if far16 pointer
             */
            opnd = ExprNode( NULL, OPR_CONVERT_PTR, opnd );
            opnd->op.u2.sp.old_ptrclass = old_ptrclass;
            opnd->op.u2.sp.new_ptrclass = new_ptrclass;
        } else {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->op.u2.result_type = newtyp;
        }
    } else {
        if( IsConstLeaf( opnd ) ) {
            CastConstValue( opnd, newtyp->decl_type );
        } else {
            opnd = ExprNode( NULL, OPR_CONVERT, opnd );
            opnd->op.u2.result_type = newtyp;
        }
    }
    opnd->u.expr_type = newtyp;
    return( opnd );
}

TREEPTR UMinus( TREEPTR opnd )
{
//    FLOATVAL        *flt;
    DATA_TYPE       t;

    opnd = RValue( opnd );
    if( opnd->op.opr != OPR_ERROR ) {
        t = DataTypeOf( TypeOf( opnd ) );
        if( t != TYP_VOID ) {
            if( t >= TYP_POINTER ) {
                CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
                opnd = ErrorNode( opnd );
            } else {
                opnd = ExprNode( NULL, OPR_NEG, opnd );
                opnd->u.expr_type = GetType( SubResult[t][t] );
                opnd->op.u2.result_type = opnd->u.expr_type;
            }
        }
    }
#if 0
    switch( opnd->op.opr ) {
    case OPR_ERROR:
        break;
    case OPR_PUSHINT:
        switch( opnd->op.const_type ) {
        case TYP_CHAR:
        case TYP_UCHAR:
            opnd->op.u2.long_value =  -(char)opnd->op.u2.long_value;
            break;
        case TYP_SHORT:
        case TYP_USHORT:
            opnd->op.u2.long_value = -(short)opnd->op.u2.long_value;
            break;
        case TYP_INT:
            opnd->op.u2.long_value = -(target_int)opnd->op.u2.long_value;
            break;
        case TYP_UINT:
            opnd->op.u2.long_value =
                        (target_uint)( - (target_uint)opnd->op.u2.long_value);
            break;
        case TYP_LONG:
        case TYP_ULONG:
            opnd->op.u2.long_value = - opnd->op.u2.long_value;
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
            flt->ld.u.word[1] ^= 0x80000000;  // - flip sign
#endif
        }
        break;
    default:
        t = DataTypeOf( TypeOf( opnd ) );
        if( t == TYP_VOID )
            break;
        if( t >= TYP_POINTER ) {
            CErr1( ERR_EXPR_MUST_BE_ARITHMETIC );
            opnd = ErrorNode( opnd );
        } else {
            opnd = ExprNode( NULL, OPR_NEG, opnd );
            opnd->u.expr_type = GetType( SubResult[t][t] );
            opnd->op.u2.result_type = opnd->u.expr_type;
        }
        break;
    }
#endif
    return( opnd );
}


TREEPTR UComplement( TREEPTR opnd )
{
    DATA_TYPE   t;
    TYPEPTR     typ;

    opnd = RValue( opnd );
    if( opnd->op.opr != OPR_ERROR ) {
        typ = opnd->u.expr_type;
        SKIP_TYPEDEFS( typ );
        t = DataTypeOf( typ );
        if( t != TYP_VOID ) {
            if( t >= TYP_FLOAT ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
                opnd = ErrorNode( opnd );
            } else {
                opnd = ExprNode( NULL, OPR_COM, opnd );
                opnd->u.expr_type = GetType( SubResult[t][t] );
                opnd->op.u2.result_type = opnd->u.expr_type;
            }
        }
    }
#if 0
    switch( opnd->op.opr ) {
    case OPR_ERROR:
        break;
    case OPR_PUSHINT:
        switch( opnd->op.const_type ) {
        case TYP_CHAR:
        case TYP_UCHAR:
            opnd->op.u2.long_value = (char)~opnd->op.u2.long_value;
            break;
        case TYP_SHORT:
        case TYP_USHORT:
            opnd->op.u2.long_value = (short)~opnd->op.u2.long_value;
            break;
        case TYP_INT:
            opnd->op.u2.long_value = (target_int)~opnd->op.u2.long_value;
            break;
        case TYP_UINT:
            opnd->op.u2.ulong_value = (target_uint)~opnd->op.u2.ulong_value;
            break;
        case TYP_LONG:
        case TYP_ULONG:
            opnd->op.u2.ulong_value = ~opnd->op.u2.ulong_value;
            break;
        }
        break;
    default:
        typ = opnd->u.expr_type;
        SKIP_TYPEDEFS( typ );
        t = DataTypeOf( typ );
        if( t == TYP_VOID )
            break;
        if( t >= TYP_FLOAT ) {
            CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            opnd = ErrorNode( opnd );
        } else {
            opnd = ExprNode( NULL, OPR_COM, opnd );
            opnd->u.expr_type = GetType( SubResult[t][t] );
            opnd->op.u2.result_type = opnd->u.expr_type;
        }
        break;
    }
#endif
    return( opnd );
}


static TYPEPTR MergedType( TYPEPTR typ1, TYPEPTR typ2 )
/******************************************************
 * (type huge *) : (type *)            -> (type huge *)
 * (type *) : (type huge *)            -> (type huge *)
 * (type far *) : (type *)             -> (type far *)
 * (type *) : (type far *)             -> (type far *)
 * (type const *) : (type *)           -> (type const *)
 * (type *) : (type const *)           -> (type const *)
 * (type volatile *) : (type *)        -> (type volatile *)
 * (type *) : (type volatile *)        -> (type volatile *)
 *          etc.
 */
{
    type_modifiers  flags, new_flags;
    TYPEPTR         typ;

    typ = typ1;
    flags = typ1->u.p.decl_flags | typ2->u.p.decl_flags;
    new_flags = flags & MASK_CV_QUALIFIERS;
    if( flags & FLAG_HUGE ) {
        new_flags |= FLAG_HUGE;
    } else if( flags & FLAG_FAR ) {
        new_flags |= FLAG_FAR;
    } else if( (typ1->u.p.decl_flags & FLAG_NEAR)
      && (typ2->u.p.decl_flags & FLAG_NEAR) ) {
        new_flags |= FLAG_NEAR;
    }
    if( typ1->u.p.decl_flags != typ2->u.p.decl_flags ) {
        typ = PtrNode( typ1->object, new_flags, typ1->u.p.segid );
    }
    return( typ );
}


TYPEPTR TernType( TREEPTR true_part, TREEPTR false_part )
/*******************************************************/
{
    TYPEPTR          typ1;
    TYPEPTR          typ2;
    DATA_TYPE        dtype1, dtype2;

    typ1 = true_part->u.expr_type;
    SKIP_TYPEDEFS( typ1 );
    typ2 = false_part->u.expr_type;
    SKIP_TYPEDEFS( typ2 );
    /*
     *  (type1) : (type1)                           -> (type1)
     *  nb. structs, unions, and identical pointers are handled here
     */
    if( typ1 == typ2 )
        return( typ1 );
    dtype1 = DataTypeOf( typ1 );
    dtype2 = DataTypeOf( typ2 );
    if( dtype1 == TYP_POINTER && false_part->op.opr == OPR_PUSHINT ) {
        if( !CheckZeroConstant( false_part ) ) {
            CWarn1( ERR_NONPORTABLE_PTR_CONV );
        }
        return( MergedType( typ1, typ2 ) ); /* merge near/far/const etc. */
    }
    if( dtype2 == TYP_POINTER && true_part->op.opr == OPR_PUSHINT ) {
        if( !CheckZeroConstant( true_part ) ) {
            CWarn1( ERR_NONPORTABLE_PTR_CONV );
        }
        return( MergedType( typ2, typ1 ) ); /* merge near/far/const etc. */
    }
    /*
     * (arithmetic type) : (arithmetic type)    -> (promoted arithmetic type)
     */
    if( (dtype1 <= TYP_LONG_DOUBLE) && (dtype2 <= TYP_LONG_DOUBLE) ) {
        return( GetType( SubResult[dtype1][dtype2] ) );
    }
    CheckTernary( typ1, typ2 );
    if( dtype1 == TYP_POINTER && dtype2 == TYP_POINTER ) {
        /*
         * (void *) : (anything *)              -> (void *)
         */
        if( typ1->object->decl_type == TYP_VOID ) {
            return( MergedType( typ1, typ2 ) );
        } else if( typ2->object->decl_type == TYP_VOID ) {
            return( MergedType( typ2, typ1 ) );
        } else {
            return( MergedType( typ1, typ2 ) );
        }
    }
    return( typ1 );
}
