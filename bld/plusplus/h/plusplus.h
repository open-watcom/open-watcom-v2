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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "bool.h"
#include "wcpp.h"
#include "target.h"
#if defined( __WATCOMC__ ) && !defined( NDEBUG )
#include "wtrap.h"
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
typedef const char          *NAME;      // name pointer
#define NameStr(x)          (x)
#else
typedef const struct idname *NAME;      // name pointer
#define NameStr(x)          (x)->name
#endif

#define VAL64(x)            (x.u._64[0])

#define FatalMsgExit(msg)   CFatal( msg )

#ifndef NDEBUG
    #define __location " (" __FILE__ "," __xstr(__LINE__) ")"
    #define DbgVerify( cond, msg ) if( !( cond ) ) CFatal( msg __location )
    #define DbgDefault( msg )   default: CFatal( msg __location )
    #define DbgAssert( cond )   if( !(cond) ) CFatal( #cond __location )
    #define DbgStmt( stmt )     stmt
    #define XTRA_RPT
    #define IfDbgToggle( n )    if( PragDbgToggle.n )

    #define DbgNever()          (CFatal( "should never execute this" __location ))
    #define DbgUseless()        (CFatal( "this code isn't useless!" __location ))
#else
    #include "toggle.h"
    #define DbgVerify( cond, msg )
    #define DbgDefault( msg )
    #define DbgAssert( cond )
    #define DbgStmt( stmt )
    #define IfDbgToggle( n )

    #define DbgNever()
    #define DbgUseless()
#endif
#include "dbgzap.h"

typedef unsigned MSG_NUM;           // - message number

#define BY_CPP_FRONT_END            // - for use in code-generator

#define NULL_CGLABEL    (-1)
#define NULL_CGREFNO    (0)
typedef unsigned        CGLABEL;
typedef unsigned        CGREFNO;
typedef unsigned        LINE_NO;    // a line number
typedef unsigned        COLUMN_NO;  // a column number
typedef struct symbol   *SYMBOL;    // symbol pointer
typedef struct type     *TYPE;      // type pointer

#include "rtfuncod.h"               // type for run-time function code

#include "cg.h"
#define fe_seg_id       segment_id  // segment ids (use cg prototype, must be a signed type)

#define PCH_struct      struct      // use to indicate struct is saved in PCH

typedef union cgvalue {             // CGVALUE: one of
    target_ulong        uvalue;     // - unsigned integer
    target_long         ivalue;     // - signed integer
    void                *pvalue;    // - pointer
} CGVALUE;

#include "icodes.h"

// having 'value' first means it will be aligned in cases
// were it is a singleton
typedef struct cginter {        // CGINTER -- intermediate-code instruction
    CGVALUE     value;          // - value
    CGINTEROP   opcode;         // - opcode for text
} CGINTER;

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

typedef struct {
    void                *curr_offset;
    unsigned long       undef_count;
} MACRO_STATE;

typedef struct open_file OPEN_FILE;

struct open_file {                      // ACTIVE FILE (BEING READ)
    LINE_NO             line;           // - current line
    COLUMN_NO           column;         // - current column
    int                 currc;          // - current character
    unsigned char       *nextc;         // - addr[ next character ]
    unsigned char       *lastc;         // - addr[ last character ]
    unsigned char       *buff;          // - buffer
    FILE                *fp;            // - file control block
    unsigned long       pos;            // - file offset to seek when reopened
};

typedef struct _src_file *SRCFILE;

struct _src_file {                      // SOURCE FILE (PERMANENT)
    struct _src_file    *sister;        // - ring of files for #line directives
    struct _src_file    *parent;        // - NULL or including source file
    struct _src_file    *unique;        // - next in unique list
    struct _src_file    *pch_child;     // - #include to create pchdr for (NULL otherwise)
    LINE_NO             parent_locn;    // - line no. for inclusion
    unsigned            index;          // - index of this source file
    OPEN_FILE           *active;        // - information for open file
    char                *name;          // - file name
    char                *full_name;     // - absolute pathname for file
    char                *ifndef_name;   // - name used when #ifndef
    unsigned            ifndef_len;     // - length of name used when #ifndef
    unsigned            guard_state;    // - guard state
    MACRO_STATE         macro_state;    // - state of macro table when opened
    time_t              time_stamp;     // - time stamp for file
                                        // - SRCFILE attributes
    unsigned            lib_inc  : 1;   // -- library include: #include <file>
    unsigned            primary  : 1;   // -- primary source file
    unsigned            alias    : 1;   // -- alias'ed source file
    unsigned            cmdline  : 1;   // -- command-line file
    unsigned            cmdlneol : 1;   // -- EOL for command-line file
    unsigned            cmdlneof : 1;   // -- EOF for command-line file
    unsigned            uncached : 1;   // -- have to re-open file on read
    unsigned            free : 1;       // -- free SRCFILE
    unsigned            pch_create : 1; // -- create pchdr when child closes
    unsigned            pch_kludge : 1; // -- EOF needs 3 ';''s to align parser
    unsigned            assume_file : 1;// -- handle represents a file not a device
    unsigned            found_eof : 1;  // -- next read will return 0
    unsigned            read_only : 1;  // -- read-only header file
    unsigned            once_only : 1;  // -- read once header file
    unsigned            ignore_swend:1; // -- ignore cmdline switch end chars
};


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
