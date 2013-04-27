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
* Description:  Platform dependent configuration for wmake.
*
****************************************************************************/


#ifndef _MSYSDEP_H
#define _MSYSDEP_H  1

#include <stdlib.h>
#include <signal.h>
#include "wio.h"

#ifdef DLLS_IMPLEMENTED
    #include "idedrv.h"
    #include <malloc.h>
#endif

// For debug versions, always use scarce memory manager - memory
// leak checking depends on it
#ifndef NDEBUG
    #define USE_SCARCE  1
#endif

#if defined( __DOS__ )
# define PATH_SPLIT         ';'     /* path seperator                       */
# define PATH_SPLIT_S       ";"     /* path seperator in string form        */
# define SHELL_METAS        "<>|"   /* characters that force use of shell   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( _A_VOLID )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MAX_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MAX_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
#if defined( _M_I86 )
# define USE_FAR            1       /* use far memory for some things       */
# define USE_SCARCE         1       /* use scarce memory management         */
# define FAR                _far
# define FILE_BUFFER_SIZE   512     /* amount to read() at a time           */
#else
# define FAR
# define FILE_BUFFER_SIZE   4096    /* amount to read() at a time           */
#endif
# define DLL_CMD_ENTRY      "???"   /* entry-pt for .DLL version of command */

#elif defined( __OS2__ ) || defined( __NT__ )

# define PATH_SPLIT         ';'     /* path seperator                       */
# define PATH_SPLIT_S       ";"     /* path seperator in string form        */
# define SHELL_METAS        "<>|&()"/* characters that force use of shell   */
# define SHELL_ESC          '^'     /* character that escapes a meta char   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( _A_VOLID )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MAX_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MAX_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define FILE_BUFFER_SIZE   4096    /* amount to read() at a time           */
# define FAR                        /* don't use far memory at all          */
# define DLL_CMD_ENTRY      "EXEC_CMD"   /* entry-pt for .DLL version of command */

#elif defined( __UNIX__ )

# define PATH_SPLIT         ':'     /* path seperator                       */
# define PATH_SPLIT_S       ":"     /* path seperator in string form        */
# define SHELL_METAS        "<>|&()"/* characters that force use of shell   */
# define SHELL_ESC          '^'     /* character that escapes a meta char   */
# define WILD_METAS         "*?"    /* wild card characters opendir supports*/
                                    /* dir entries to ignore (direct.h)     */
# define IGNORE_MASK        ( 0 )
# define EXIT_OK            0       /* normal termination of program        */
# define EXIT_WARN          1       /* return from aborted -q (Query) make  */
# define EXIT_ERROR         2       /* return after errors in parsing       */
# define EXIT_FATAL         4       /* return after fatal error             */
# define MAX_SUFFIX         16      /* must fit dotname, or largest .ext.ext*/
# define MAX_TOK_SIZE       130     /* Maximum token size                   */
# define LINE_BUFF          80      /* length of one-line user input buffer */
# define FILE_BUFFER_SIZE   4096    /* amount to read() at a time           */
# define FAR                        /* don't use far memory at all          */
# define DLL_CMD_ENTRY      "???"   /* entry-pt for .DLL version of command */

#else
# error Must define system dependent macros
#endif

#define MAX_MAC_NAME    130     /* Maximum macro name length                */
#define MAX_MAC_NEST    16      /* Maximum depth of macro nesting           */

enum {
    OLDEST_DATE   = 0L,                                         // oldest
    YOUNGEST_DATE = (((time_t)-1) > 0) ? ULONG_MAX : LONG_MAX   // youngest
};

#define MAKEFILE_NAME   "makefile"
#ifdef __UNIX__
#define MAKEFILE_ALT    "Makefile"
#endif
#define MAKEINIT_NAME   "makeinit"
#define MAKEFINI_NAME   "makefini"
#define TOOLSINI_NAME   "tools.ini"

/*
 * Sanity checks
 */
#if MAX_TOK_SIZE < _MAX_PATH
#undef MAX_TOK_SIZE
#define MAX_TOK_SIZE    _MAX_PATH
#endif
#if MAX_TOK_SIZE < _MAX_PATH2
#undef MAX_TOK_SIZE
#define MAX_TOK_SIZE    _MAX_PATH2
#endif
#if MAX_SUFFIX < _MAX_EXT
#undef MAX_SUFFIX
#define MAX_SUFFIX      _MAX_EXT
#endif

typedef enum {
    SOA_NOT_AN_OBJ,
    SOA_BUILD_OBJ,
    SOA_UP_TO_DATE,
} autodep_ret_t;

typedef struct dll_cmd DLL_CMD;
struct dll_cmd {
    DLL_CMD     *next;
    char const  *cmd_name;
#ifdef DLLS_IMPLEMENTED
    IDEDRV      inf;
#endif
};

extern int              SwitchChar( void );
extern int              OSCorrupted( void );
extern RET_T            TouchFile( const char *name );
extern BOOLEAN          IdenticalAutoDepTimes( time_t, time_t );
extern void             InitHardErr( void );
extern void             OSLoadDLL( char *cmd, char *dll_name, char *ent_name );
extern DLL_CMD          *OSFindDLL( char const *cmd_name );
extern int              OSExecDLL( DLL_CMD *dll, char const *cmd_args );
extern void             CheckForBreak( void );
extern void             InitSignals( void );
extern void             DLLFini( void );

#endif
