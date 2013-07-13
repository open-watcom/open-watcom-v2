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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// PLUSPLUS.H -- C++ Common Definitions

//
// 91/06/04 -- J.W.Welch        -- defined
// 91/11/07 -- J.W.Welch        -- placed common definitions in WCCP.H
// 92/12/29 -- B.J. Stecher     -- QNX support

#ifdef NDEBUG
#define NAME_PTR_IS_NAME_MEMBER
#endif

#include <stdlib.h>
#include <string.h>

#include "wcpp.h"
#include "target.h"
#if defined( __WATCOMC__ ) && !defined( NDEBUG )
#include "trap.h"
#endif

#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[0] ) )
#define PAD_UNSIGNED unsigned :0;

#define OPTIMIZE_EMPTY
#define CARVEPCH

#ifndef NDEBUG
//#define OPT_BR          // OPTIMA-STYLE BROWSING
#endif

// useful for stringizing tokens
#define __str( m )      #m
#define __xstr( l )     __str( l )

#ifndef global
#define global  extern
#endif

#ifdef __UNIX__
    #define FNAMECMPSTR      strcmp      /* for case  sensitive file systems */
#else
    #define FNAMECMPSTR      stricmp     /* for case insensitive file systems */
#endif

typedef struct idname {
    struct idname       *next;
    uint_16             xhash;
    uint_16             hash;
    char                name[1];
} idname;

#ifdef NAME_PTR_IS_NAME_MEMBER
typedef char            *NAME;      // name pointer
#define NameStr(x)      (x)
#else
typedef struct idname   *NAME;      // name pointer
#define NameStr(x)      (x)->name
#endif

#ifndef NDEBUG
    #define __location " (" __FILE__ "," __xstr(__LINE__) ")"
    #define DbgVerify( cond, msg ) if( ! ( cond ) ) CFatal( msg __location );
    #define DbgDefault( msg ) default: CFatal( msg __location )
    #define DbgAssert( cond )   if( !(cond) ) CFatal( #cond __location );
    #define DbgStmt( stmt )     stmt;
    #define XTRA_RPT
    #define IfDbgToggle( n ) if( PragDbgToggle.n )

    #define DbgIsBoolean( x )   (((unsigned)(x))<=1)

    #define DbgNever()          (CFatal( "should never execute this" __location ))
    #define DbgUseless()        (CFatal( "this code isn't useless!" __location ))
#else
    #include "toggle.h"
    #define DbgVerify( cond, msg )
    #define DbgDefault( msg )
    #define DbgAssert( cond )
    #define DbgStmt( stmt )
    #define IfDbgToggle( n )

    #define DbgIsBoolean( x )   (1)

    #define DbgNever()
    #define DbgUseless()
#endif
#include "dbgzap.h"

typedef int boolean;                // - boolean: FALSE, ! FALSE
typedef unsigned MSG_NUM;           // - message number

#define BY_CPP_FRONT_END            // - for use in code-generator

#define NULL_CGLABEL    (-1)
#define NULL_CGREFNO    (0)
typedef unsigned        CGLABEL;
typedef unsigned        CGREFNO;
typedef union  cgvalue  CGVALUE;    // value in intermediate instr.
typedef unsigned        RTF;        // type for run-time function code
typedef unsigned        LINE_NO;    // a line number
typedef unsigned        COLUMN_NO;  // a column number
typedef short           fe_seg_id;  // segment ids (must be a signed type)
typedef struct _src_file *SRCFILE;  // SOURCE FILE (PERMANENT)
typedef struct symbol   *SYMBOL;    // symbol pointer
typedef struct type     *TYPE;      // type pointer

#define PCH_struct      struct      // use to indicate struct is saved in PCH

typedef union cgvalue {             // CGVALUE: one of
    target_ulong        uvalue;     // - unsigned integer
    target_long         ivalue;     // - signed integer
    void                *pvalue;    // - pointer
} CGVALUE;

#include "icodes.h"

// structure is written out to a file
#include "pushpck1.h"

// having 'value' first means it will be aligned in cases
// were it is a singleton
typedef struct cginter          CGINTER;    // intermediate text
struct cginter {                // CGINTER -- intermediate-code instruction
    CGVALUE     value;          // - value
    CGINTEROP   opcode;         // - opcode for text
};

#include "poppck.h"

/* Target System types */
enum {
    TS_OTHER,
    TS_DOS,
    TS_NT,
    TS_WINDOWS,
    TS_CHEAP_WINDOWS,
    TS_NETWARE,
    TS_OS2,
    TS_QNX,
    TS_NETWARE5,
    TS_LINUX,
    TS_MAX
};

typedef struct cfloat CPP_FLOAT;

#include "symtype.h"
#include "symbol.h"
#include "utility.h"
#include "compflag.h"
#include "errdefns.h"

#define LINE_NO_MAX  0xFFFFFFFF     // MAXIMUM LINE NO.
#define BRINF_SYMBOL ((SYMBOL)0xFFFFFFFF) // FAKE BROWSE-INFO SYMBOL

                                    // COMMON DATA
global int WngLevel;                // - warning severity level
global unsigned TargetSystem;       // - target system
global struct comp_flags CompFlags; // - compiler flags
global struct comp_info CompInfo;   // - compiler information
global void *Environment;           // - var for Suicide()
global int ErrLimit;                // - error limit

int WppCompile(                 // MAIN-LINE (DLL)
    DLL_DATA* dll_data,         // - data for DLL
    char *input,                // - input file name argv string
    char *output )              // - output file name argv string
;
