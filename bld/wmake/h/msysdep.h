/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Platform dependent configuration for wmake.
*
****************************************************************************/


#ifndef _MSYSDEP_H
#define _MSYSDEP_H  1

#include <stdlib.h>
#include <signal.h>
#include <time.h>

#ifdef DLLS_IMPLEMENTED
    #include "idedrv.h"
#endif

// For debug versions, always use scarce memory manager - memory
// leak checking depends on it
#ifndef NDEBUG
    #define USE_SCARCE  1
#endif

#if defined( _M_I86 )
# define USE_FAR            1       /* use far memory for some things       */
# define USE_SCARCE         1       /* use scarce memory management         */
# define FAR                __far   /* use far memory                       */
# define FILE_BUFFER_SIZE   512     /* amount to read() at a time           */
#else
# define FAR                        /* don't use far memory at all          */
# define FILE_BUFFER_SIZE   4096    /* amount to read() at a time           */
#endif

#if defined( __DOS__ )
# define PATH_SPLIT         ';'     /* path seperator                       */
# define IS_PATH_SPLIT(c)   ((c)==PATH_SPLIT)
# define SHELL_METAS        "<>|"   /* characters that force use of shell   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( _A_VOLID )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MIN_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MIN_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define DLL_CMD_ENTRY      "???"   /* entry-pt for .DLL version of command */

#elif defined( __OS2__ ) || defined( __NT__ )

# define PATH_SPLIT         ';'     /* path seperator                       */
# define IS_PATH_SPLIT(c)   ((c)==PATH_SPLIT)
# define SHELL_METAS        "<>|&()"/* characters that force use of shell   */
# define SHELL_ESC          '^'     /* character that escapes a meta char   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( _A_VOLID )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MIN_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MIN_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define DLL_CMD_ENTRY      "EXEC_CMD"   /* entry-pt for .DLL version of command */

#elif defined( __UNIX__ )

# define PATH_SPLIT         ':'     /* path seperator                       */
# define IS_PATH_SPLIT(c)   ((c)==PATH_SPLIT||(c)==';')
# define SHELL_METAS        "<>|&()"/* characters that force use of shell   */
# define SHELL_ESC          '^'     /* character that escapes a meta char   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( 0 )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MIN_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MIN_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define DLL_CMD_ENTRY      "???"   /* entry-pt for .DLL version of command */


#elif defined( __RDOS__ )

# define PATH_SPLIT         ';'     /* path seperator                       */
# define IS_PATH_SPLIT(c)   ((c)==PATH_SPLIT||(c)==';')
# define SHELL_METAS        "<>|&()"/* characters that force use of shell   */
# define SHELL_ESC          '^'     /* character that escapes a meta char   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( 0 )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MIN_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MIN_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define DLL_CMD_ENTRY      "EXEC_CMD"   /* entry-pt for .DLL version of command */

#else
# error Must define system dependent macros
#endif

#define MAX_MAC_NAME    130     /* Maximum macro name length                */
#define MAX_MAC_NEST    16      /* Maximum depth of macro nesting           */

#define MAX_DATE_QUARTER    ((time_t)1 << (sizeof(time_t) * CHAR_BIT - 2))
#define MAX_DATE_SIGNED     ((time_t)(MAX_DATE_QUARTER - 1 + MAX_DATE_QUARTER))
#define MAX_DATE_UNSIGNED   ((time_t)-1)

#define OLDEST_DATE     ((time_t)0)
#define YOUNGEST_DATE   (((time_t)-1 > 0) ? MAX_DATE_UNSIGNED : MAX_DATE_SIGNED)

#define MAKEFILE_NAME   "makefile"
#ifdef __UNIX__
#define MAKEFILE_ALT    "Makefile"
#endif
#define MAKEINIT_NAME   "makeinit"
#define MAKEFINI_NAME   "makefini"
#define TOOLSINI_NAME   "tools.ini"

#define MAX_TOK_SIZE    ((MIN_TOK_SIZE < _MAX_PATH2)?_MAX_PATH2:MIN_TOK_SIZE)
#define MAX_SUFFIX      ((MIN_SUFFIX < _MAX_EXT)?_MAX_EXT:MIN_SUFFIX)

typedef enum {
    SOA_NOT_AN_OBJ,
    SOA_BUILD_OBJ,
    SOA_UP_TO_DATE,
} autodep_ret_t;

typedef struct dll_cmd {
    struct dll_cmd  *next;
    char const      *cmd_name;
#ifdef DLLS_IMPLEMENTED
    IDEDRV          inf;
#endif
} DLL_CMD;

typedef enum {
    AUTO_DEP_NONE,
    AUTO_DEP_OMF,
    AUTO_DEP_ORL,
    AUTO_DEP_RES
} auto_dep_type;

typedef struct EnvTracker {
    struct EnvTracker   *next;
    char                *value;
    char                name[1];
} ENV_TRACKER;

extern int          SwitchChar( void );
extern int          OSCorrupted( void );
extern bool         TouchFile( const char *name );
extern bool         IdenticalAutoDepTimes( time_t, time_t, auto_dep_type );
extern void         InitHardErr( void );
extern void         OSLoadDLL( char *cmd, char *dll_name, char *ent_name );
extern DLL_CMD      *OSFindDLL( char const *cmd_name );
extern int          OSExecDLL( DLL_CMD *dll, char const *cmd_args );
extern void         CheckForBreak( void );
extern void         InitSignals( void );
extern void         DLLFini( void );
extern char         *GetEnvExt( const char *str );
extern int          SetEnvExt( ENV_TRACKER *env );
extern int          SetEnvSafe( const char *name, const char *value );
#if !defined(NDEBUG) || defined(DEVELOPMENT)
extern void         SetEnvFini( void );
#endif

#endif
