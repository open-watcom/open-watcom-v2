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
* Description:  Common stuff for wcl and owcc
*
****************************************************************************/


/* Several char foo[MAX_CMD] arrays are defined. Overflow goes undetected */
#if defined(__OS2__) || defined(__NT__) || defined(__UNIX__)
#define MAX_CMD 10240
#else
#define MAX_CMD 500
#endif

#if defined(__UNIX__)
#define fname_cmp   strcmp
#else
#define fname_cmp   stricmp
#endif

#ifdef __UNIX__
#define PATH_SEPS_STR   SYS_DIR_SEP_STR
#else
#define PATH_SEPS_STR   SYS_DIR_SEP_STR "/"
#endif

#ifdef __UNIX__
#define OBJ_EXT             ".o"
#define OBJ_EXT_SECONDARY   ".obj"
#define EXE_EXT             ""
#else
#define OBJ_EXT             ".obj"
#define OBJ_EXT_SECONDARY   ".o"
#define EXE_EXT             ".exe"
#endif
#define LIB_EXT             ".lib"
#define LIB_EXT_SECONDARY   ".a"

#define ASM_EXT             ".asm"
#define ASMS_EXT            ".s"

#define LNK_EXT             ".lnk"

#define IS_OBJ(x)           (HasFileExtension( x, OBJ_EXT ) || HasFileExtension( x, OBJ_EXT_SECONDARY ))

typedef enum tool_type {
    TYPE_LINK,
    TYPE_PACK,
    TYPE_DIS,
    TYPE_ASM,
    TYPE_C,
    TYPE_CPP,
    TYPE_FORT,
    TYPE_MAX
} tool_type;

#define TYPE_ALLARCH_COUNT  TYPE_ASM

typedef struct etool {
    char *name;
    char *exename;
    char *path;
} etool;

enum {
    #define E(msg)  msg
    #define pick(code,msg)  code
    #include "wclmsg.h"
    #undef pick
    #undef E
};

typedef enum DBG_OPT {
    DBG_NONE,
    DBG_LINES,
    DBG_ALL,
    DBG_FMT_DWARF,
    DBG_FMT_WATCOM,
    DBG_FMT_CODEVIEW,
} DBG_OPT;

typedef struct  list {
    char        *item;
    struct list *next;
} list;

typedef struct  flags {
    unsigned math_8087        : 1;  /* 0 ==> no 8087, otherwise /7 option */
    unsigned map_wanted       : 1;  /* -fm option specified               */
    unsigned link_ignorecase  : 1;  /* link nocaseexact option            */
    unsigned tiny_model       : 1;  /* tiny memory model                  */
    unsigned be_quiet         : 1;  /* -zq option to be quiet             */
    unsigned no_link          : 1;  /* -c compile only, no link step      */
    unsigned do_link          : 1;  /* flag for link if no .obj in Cmd    */
    unsigned do_disas         : 1;  /* flag to call wdis                  */
    unsigned do_cvpack        : 1;  /* flag for link do codeview cvpack   */
    unsigned link_for_dos     : 1;  /* -lr produce DOS executable         */
    unsigned link_for_os2     : 1;  /* -lp produce OS/2 executable        */
    unsigned windows          : 1;  /* -zw specified for Windows          */
    unsigned link_for_sys     : 1;  /* -l<system> option given            */
    unsigned force_c          : 1;  /* -cc option                         */
    unsigned force_c_plus     : 1;  /* -cc++ option                       */
    unsigned strip_all        : 1;  /* -s option for owcc                 */
    unsigned want_errfile     : 1;  /* -fr option in owcc                 */
    unsigned keep_exename     : 1;  /* verbatim -o name from owcc         */
} flags;

extern char     *Exe_Name;          /* name of executable                 */
extern char     *Map_Name;          /* name of map file                   */
extern char     *Obj_Name;          /* object file name pattern           */
extern list     *Libs_List;         /* list of libraires from Cmd         */
extern list     *Obj_List;          /* linked list of object filenames    */
extern list     *Directive_List;    /* linked list of wlink directives    */
extern const char *WclMsgs[];

extern flags    Flags;

extern char     *StackSize;         /* size of stack                      */
extern DBG_OPT  DebugFlag;          /* debug info wanted                  */
extern DBG_OPT  DebugFormat;        /* debug info format                  */

extern void     PrintMsg( const char *fmt, ... );
extern void     FindPath( const char *name, char *buf );
extern void     BuildLinkFile( FILE *fp );
extern void     AddNameObj( const char * );
extern void     ListAppend( list **, list * );
extern void     ListFree( list * );
extern void     Fputnl( const char *, FILE * );
extern void     MemInit( void );
extern void     MemFini( void );
extern void     *MemAlloc( size_t );
extern char     *MemStrDup( const char * );
extern char     *MemStrLenDup( const char *str, size_t len );
extern void     *MemReAlloc( void *, size_t );
extern void     MemFree( void * );
extern char     *MakePath( const char * );
extern char     *GetName( const char *, char * );
extern char     *DoQuoted( char *buffer, const char *name, char quote );
extern void     BuildSystemLink( FILE *fp );
extern void     AddDirective( const char *directive );
extern void     AddDirectivePath( const char *directive, const char *path );
extern char     *RemoveExt( char * );
extern int      HasFileExtension( const char *p, const char *ext );
extern void     MakeName( char *name, const char *ext );
