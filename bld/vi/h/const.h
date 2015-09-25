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
* Description:  Definition of editor constants.
*
****************************************************************************/


#ifndef _CONST_INCLUDED
#define _CONST_INCLUDED

#ifndef __WIN__
    #define CFG_NAME    "ed.cfg"
#else
    #define CFG_NAME    "weditor.ini"
#endif

#define VI_2F_ID        0xfc

#define SPLIT_CHAR      3

#define TOK_INVALID         (-1)

#define GET_BOOL_PREFIX(b)  ((b) ? "" : "no")

typedef enum {
    DRIVE_NONE,
    DRIVE_IS_REMOVABLE,
    DRIVE_IS_FIXED
} drive_type;

typedef enum {
    SAVEBUF_FLAG    = 0x01,
    USE_UNDO_UNDO   = 0x02
} linedel_flags;

#if defined( __UNIX__ ) || defined( __IBMC__ )
    #define _A_NORMAL       0x00    /* Normal file - read/write permitted */
    #define _A_RDONLY       0x01    /* Read-only file */
    #define _A_HIDDEN       0x02    /* Hidden file */
    #define _A_SYSTEM       0x04    /* System file */
    #define _A_VOLID        0x08    /* Volume-ID entry */
    #define _A_SUBDIR       0x10    /* Subdirectory */
    #define _A_ARCH         0x20    /* Archive file */
#endif
    
#if defined( __UNIX__ )
    #define FILE_SEP            '/'
    #define FILE_SEP_STR        "/"
    #define ALL_FILES_WILD_CARD "*"
    #define TMP_NAME_LEN        18
#else
    #define FILE_SEP            '\\'
    #define FILE_SEP_STR        "\\"
    #define ALL_FILES_WILD_CARD "*.*"
    #define TMP_NAME_LEN        14
#endif

#define TOK_MAX_LEN     30
#define NAMEWIDTH       14
#define MAX_NUM_STR     48

/*
 * extra data per line: +4 because of swap file
 * considerations: +2 for CR,LF and +2 for extra line data
 */
#define LINE_EXTRA      (2 + sizeof( linedata_t ))

#define Tab( col, ta )  ((ta == 0) ? 0 : ((((col - 1) / ta) + 1) * ta - (col - 1)))

#include "bool.h"

#define INITIAL_MATCH_COUNT     4
#define MIN_LINE_LEN            128
#define MAX_LONG                0x7fffffffL
#define MAX_REPEAT_STRING       10
#define MAX_FILES               640
#define MAX_BOOL_TOKENS         2
#define MAX_SAVEBUFS            9
#define MAX_SPECIAL_SAVEBUFS    26
#define WORK_SAVEBUF            (MAX_SAVEBUFS + MAX_SPECIAL_SAVEBUFS)
#define NO_SAVEBUF              -1
#define CLIPBOARD_SAVEBUF       -2
#define SCROLL_HLINE            10
#define SCROLL_VLINE            2
#define MAX_MARKS               26
#define MAX_SEARCH_STRINGS      9
#define MAX_SCRIPT_LENGTH       2048
#define MAX_MOUSE_SPEED         250
#define MAX_OVERRIDE_KEY_BUFF   512
#define MAX_STR                 256
#define FGREP_BUFFSIZE          32000
#define NUM_EDIT_OPTS           4
#define EXTENSION_LENGTH        5
#define CR                      0x0d
#define LF                      0x0a
#define CTRLZ                   0x1a
#define MAX_STATIC_BUFFERS      5
#define MAX_STARTUP             10
#define MAX_INPUT_LINE          512
#define DATE_LEN                24
#define MIN_STACK_K             10
#define MAX_DUPLICATE_FILES     10

#define MAX_IO_BUFFER   0x2000

#define MAPFLAG_DAMMIT          0x01
#define MAPFLAG_UNMAP           0x02
#define MAPFLAG_MESSAGE         0x04
#define MAPFLAG_BASE            0x08

/*
 * generic file types
 */
typedef enum {
    GF_BOUND,
    GF_FILE,
    GF_BUFFER
} gftype;

/*
 * data files
 */
#define ED_CFG          0
#define BIND_DAT        1
#define ERRMSG_DAT      2
#define ERROR_DAT       3
#define DATA_FILE_CNT   4

/*
 * savebuf constants
 */
#define SAVEBUF_NOP     0
#define SAVEBUF_LINE    1
#define SAVEBUF_FCBS    2

/*
 * text insertion constants
 */
typedef enum {
    INSERT_BEFORE,
    INSERT_AFTER
} insert_dir;

/*
 * status types
 */
typedef enum {
    CSTATUS_CHANGE,
    CSTATUS_DELETE,
    CSTATUS_YANK,
    CSTATUS_CASE,
    CSTATUS_INSERT,
    CSTATUS_OVERSTRIKE,
    CSTATUS_REPLACECHAR,
    CSTATUS_COMMAND,
    CSTATUS_WRITING,
    CSTATUS_READING,
    CSTATUS_HIDE,
    CSTATUS_SHIFT_RIGHT,
    CSTATUS_SHIFT_LEFT,
    CSTATUS_AUTOSAVE
} status_type;

/*
 * find types
 */
typedef enum {
    FINDFL_FORWARD   = 0x01,
    FINDFL_BACKWARDS = 0x02,
    FINDFL_NEXTLINE  = 0x04,
    FINDFL_NOERROR   = 0x08,
    FINDFL_WRAP      = 0x10,
    FINDFL_NOCHANGE  = 0x20
} find_type;

/*
 * Font types
 */
typedef enum font_type {
    FONT_COURIER = 0,
    FONT_COURIERBOLD,
    FONT_HELV,
    FONT_ARIAL,
    FONT_ARIALBOLD,
    FONT_FIXED,
    FONT_SANSSERIF,

    MAX_FONTS = 25
} font_type;

#define FONT_DEFAULT        FONT_COURIER
#define FONT_DEFAULTBOLD    FONT_COURIERBOLD

/*
 * word constants
 */
#define ON_BLANK        1
#define ON_DELIM        2
#define ON_CHAR         3
#define ON_ENDOFLINE    4
#define ON_STARTOFLINE  5

/*
 * undo constants
 */
#define START_UNDO_GROUP        1
#define UNDO_DELETE_FCBS        2
#define UNDO_INSERT_LINES       3
#define END_UNDO_GROUP          4

/*
 * directory constants
 */
#define CREATE_DIR      0x39
#define DELETE_DIR      0x3a
#define CHANGE_DIR      0x3b

/*
 * Justification contants for Status bar
 */
#define FMT_LEFT        0x01
#define FMT_RIGHT       0x02
#define FMT_CENTRE      0x03

/*
 * Event type constants for the event list
 */
typedef enum event_type {
    EVENT_OP       = 0,
    EVENT_REL_MOVE = 1,
    EVENT_ABS_MOVE = 2,
    EVENT_MISC     = 3,
    EVENT_INS      = 4
} event_type;

/*
 * Name of environment variable to set the prompt.
 */
#ifdef __UNIX__
    #define PROMPT_ENVIRONMENT_VARIABLE "PS1"
#else
    #define PROMPT_ENVIRONMENT_VARIABLE "PROMPT"
#endif

#define MAX_COLOR_REGISTERS     16

/*
 * Color type
 */
typedef enum {
    #define vi_pick(a) a,
    #include "colors.h"
    #undef vi_pick
#ifdef __WIN__
    MAX_COLORS = 64
#else
    MAX_COLORS = MAX_COLOR_REGISTERS
#endif
} vi_color;

/*
 * Event type
 */
#define VI_KEY( a )                 __VIKEY__##a
#define BITS( a, b, c, d, e, f, g ) { a, b, c, d, e, f, g }

typedef enum vi_key {
    #define vi_pick( enum, modeless, insert, command, nm_bits, bits ) enum,
    #include "events.h"
    #undef vi_pick
    MAX_EVENTS
} vi_key;

#define NO_ADD_TO_HISTORY_KEY   VI_KEY( CTRL_A )
#define VI_KEY_HANDLED          VI_KEY( NULL )
#define VI_KEY_DUMMY            MAX_EVENTS

#define C2VIKEY(x)      ((vi_key)(unsigned char)(x))

enum border_char {
    #define vi_pick( enum, UnixNG, UnixG, DosNG, DosG ) enum,
    #include "borders.h"
    #undef vi_pick
};

#endif
