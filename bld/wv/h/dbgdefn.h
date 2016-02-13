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


#include "machtype.h"
#include "bool.h"

/* Functions declared as OVL_EXTERN are used only within the module
 * they are declared in however pointers to them are used.  In order
 * for the overlay manager to be able to keep track of these pointers
 * the functions must be extern when an overlayed debugger is made
 */

#if defined(_OVERLAYED_)
#define OVL_EXTERN
#else
#define OVL_EXTERN      static
#endif

#define NULLCHAR        '\0'
#define ARG_TERMINATE   '\xff'

#define GETU8(x)        (*(unsigned char *)(x))
#define GETI8(x)        (*(signed char *)(x))
#define GETWORD(x)      (GETU8((x)) + ( GETU8((x + 1)) << 8 ))

/* Handles */

struct machine_state;

typedef struct machine_state    machine_state;


/* Global Definitions */

typedef unsigned                error_handle;

typedef int                     file_handle;

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

#define UP_NO_CHANGE            0x00000000UL
#define UP_MEM_CHANGE           0x00000001UL
#define UP_CSIP_CHANGE          0x00000002UL
#define UP_BREAK_CHANGE         0x00000004UL
#define UP_OPEN_CHANGE          0x00000008UL
#define UP_NEW_SRC              0x00000010UL
#define UP_REG_CHANGE           0x00000020UL
#define UP_RADIX_CHANGE         0x00000040UL
#define UP_CODE_ADDR_CHANGE     0x00000080UL
#define UP_SYMBOLS_LOST         0x00000100UL
#define UP_CODE_EXECUTED        0x00000200UL
#define UP_THREAD_STATE         0x00000400UL
#define UP_SYMBOLS_ADDED        0x00000800UL
#define UP_DLG_WRITTEN          0x00001000UL
#define UP_NEW_PROGRAM          0x00002000UL
#define UP_CLIP_CHANGE          0x00004000UL
#define UP_CSIP_JUMPED          0x00008000UL
#define UP_MACRO_CHANGE         0x00010000UL
#define UP_STACKPOS_CHANGE      0x00020000UL
#define UP_EVENT_CHANGE         0x00040000UL
#define UP_MAD_CHANGE           0x00080000UL
#define UP_REG_RESIZE           0x00100000UL
#define UP_ASM_RESIZE           0x00200000UL
#define UP_VAR_DISPLAY          0x00400000UL
#define UP_ALL_CHANGE           0xFFFFFFFFUL

#define UP_SYM_CHANGE           (UP_SYMBOLS_LOST+UP_SYMBOLS_ADDED)

typedef unsigned long update_list;

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

#undef ArraySize
#define ArraySize( x ) ( sizeof( x ) / sizeof( (x)[0] ) )

#define AddrCue Dont_call_AddrCue_directly___call_DeAliasAddrCue_instead
#define AddrScope Dont_call_AddrScope_directly___call_DeAliasAddrScope_instead
#define AddrSym Dont_call_AddrSym_directly___call_DeAliasAddrSym_instead
#define AddrMod Dont_call_AddrMod_directly___call_DeAliasAddrMod_instead
