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


#include <stdio.h>
#include "bool.h"
#include "machtype.h"


#define NULLCHAR        '\0'
#define ARG_TERMINATE   '\xff'

#define GETU8(x)        (*(unsigned char *)(x))
#define GETI8(x)        (*(signed char *)(x))
#define GETWORD(x)      (GETU8((x)) + ( GETU8((x + 1)) << 8 ))

/* file handles conversion macros */
#if defined( _WIN64 )
#define FH2FP(fh)       ((FILE *)(unsigned __int64)((fh) + 1))
#define FP2FH(fp)       ((file_handle)((unsigned __int64)(fp)) - 1)
#else
#define FH2FP(fh)       ((FILE *)(unsigned long)((fh) + 1))
#define FP2FH(fp)       ((file_handle)((unsigned long)(fp)) - 1)
#endif

#define FH2SYSH(sh,fh)  (sh).u._32[0]=fh;(sh).u._32[1]=0

/* Handles */

struct machine_state;

typedef struct machine_state    machine_state;


/* Global Definitions */

typedef unsigned                error_handle;

typedef int                     file_handle;
#define NIL_HANDLE              ((file_handle)-1)

typedef unsigned_8 debug_level; enum {
    #define pick( a,b ) a,
    #include "dbglevel.h"
    #undef pick
};

typedef unsigned_8 screen_state; enum {
        DBG_SCRN_ACTIVE  = 0x01,
        DBG_SCRN_VISIBLE = 0x02,
        USR_SCRN_ACTIVE  = 0x04,
        USR_SCRN_VISIBLE = 0x08
};

#ifdef _M_I86
#define ENUMU32(x)      x ## UL
#else
#define ENUMU32(x)      x ## U
#endif

typedef enum {
    UP_NO_CHANGE            = ENUMU32( 0x00000000 ),
    UP_MEM_CHANGE           = ENUMU32( 0x00000001 ),
    UP_CSIP_CHANGE          = ENUMU32( 0x00000002 ),
    UP_BREAK_CHANGE         = ENUMU32( 0x00000004 ),
    UP_OPEN_CHANGE          = ENUMU32( 0x00000008 ),
    UP_NEW_SRC              = ENUMU32( 0x00000010 ),
    UP_REG_CHANGE           = ENUMU32( 0x00000020 ),
    UP_RADIX_CHANGE         = ENUMU32( 0x00000040 ),
    UP_CODE_ADDR_CHANGE     = ENUMU32( 0x00000080 ),
    UP_SYMBOLS_LOST         = ENUMU32( 0x00000100 ),
    UP_CODE_EXECUTED        = ENUMU32( 0x00000200 ),
    UP_THREAD_STATE         = ENUMU32( 0x00000400 ),
    UP_SYMBOLS_ADDED        = ENUMU32( 0x00000800 ),
    UP_DLG_WRITTEN          = ENUMU32( 0x00001000 ),
    UP_NEW_PROGRAM          = ENUMU32( 0x00002000 ),
    UP_CLIP_CHANGE          = ENUMU32( 0x00004000 ),
    UP_CSIP_JUMPED          = ENUMU32( 0x00008000 ),
    UP_MACRO_CHANGE         = ENUMU32( 0x00010000 ),
    UP_STACKPOS_CHANGE      = ENUMU32( 0x00020000 ),
    UP_EVENT_CHANGE         = ENUMU32( 0x00040000 ),
    UP_MAD_CHANGE           = ENUMU32( 0x00080000 ),
    UP_REG_RESIZE           = ENUMU32( 0x00100000 ),
    UP_ASM_RESIZE           = ENUMU32( 0x00200000 ),
    UP_VAR_DISPLAY          = ENUMU32( 0x00400000 ),
    UP_ALL_CHANGE           = ENUMU32( 0xFFFFFFFF ),

    UP_SYM_CHANGE           = (UP_SYMBOLS_LOST | UP_SYMBOLS_ADDED)
} update_flags;

typedef unsigned_8 task_status; enum {
        TASK_NONE,
        TASK_NOT_LOADED,
        TASK_NEW
};

typedef unsigned_8 memory_expr; enum {
        EXPR_CODE,
        EXPR_DATA,
        EXPR_GIVEN
};

typedef unsigned_8 cnvaddr_option; enum {
    CAO_NORMAL_PLUS,
    CAO_OMIT_PLUS,
    CAO_NO_PLUS
};

typedef unsigned_8 trace_cmd_type; enum {
    #define pick( a,b ) a,
    #include "_dbgtrac.h"
    #undef pick
};

typedef struct {
    unsigned char use;
    char          buff[1];
} cmd_list;


typedef struct char_ring {
    struct char_ring    *next;
    char                name[1];
} char_ring;

typedef struct {
    int         top;
    int         bot;
    int         left;
} margins;

/* buffer lengths */
#define CMD_LEN         156
#define UTIL_LEN        1024
#define TXT_LEN         1024
#define TRP_LEN         80
#define NAM_LEN         40

#define MAX_NESTED_CALL     10

#define DBG_EXTENSION       "dbg"
#define SETUP_FILENAME      "setup"
#define PROFILE_FILENAME    "dvprof"

#define DECIMAL_BYTE_LEN    3               // 256
#define DECIMAL_WORD_LEN    5               // 65535
#define DECIMAL_DWORD_LEN   10              // 4294967295

#define OVL_MAP_CURR    0
#define OVL_MAP_EXE     1

typedef enum {
    #define pick( a,b,c ) a,
    #include "_dbgcmd.h"
    #undef pick
} wd_cmd;

enum {
    ACTION_NONE,
    ACTION_EXECUTE,
    ACTION_ASSIGNMENT,
    ACTION_THREAD_CHANGE,
    ACTION_MODIFY_IP,
    ACTION_MODIFY_MEMORY,
    ACTION_MODIFY_REGISTER,
    ACTION_MODIFY_VARIABLE,
};

#define SYM_NAME_LEN(n)         ((unsigned char)(n)[0])
#define SYM_NAME_NAME(n)        ((n)+1)
#define SET_SYM_NAME_LEN(n,l)   ((n)[0]=(char)l)

#define ArraySize( x ) ( sizeof( x ) / sizeof( (x)[0] ) )

#define DIPAddrCue      Dont_call_DIPAddrCue_directly___call_DeAliasAddrCue_instead
#define DIPAddrScope    Dont_call_DIPAddrScope_directly___call_DeAliasAddrScope_instead
#define DIPAddrSym      Dont_call_DIPAddrSym_directly___call_DeAliasAddrSym_instead
#define DIPAddrMod      Dont_call_DIPAddrMod_directly___call_DeAliasAddrMod_instead
