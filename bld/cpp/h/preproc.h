/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "bool.h"


#define PPENTRY

#define PPSPEC_NONE     0
#define PPSPEC_AS       1
#define PPSPEC_C        1
#define PPSPEC_RC       1
#define PPSPEC_SSL      0

typedef enum {
    PPINCLUDE_USR,
    PPINCLUDE_SYS,
    PPINCLUDE_SRC,
} incl_type;

typedef enum {
    PPFLAG_NONE                 = 0,
    PPFLAG_PREPROCESSING        = 0x0001,
    PPFLAG_EMIT_LINE            = 0x0002,
    PPFLAG_SKIP_COMMENT         = 0x0004,
    PPFLAG_KEEP_COMMENTS        = 0x0008,
    PPFLAG_DEPENDENCIES         = 0x0010,
    PPFLAG_ASM_COMMENT          = 0x0020,
    PPFLAG_IGNORE_CWD           = 0x0040,
    PPFLAG_IGNORE_DEFDIRS       = 0x0080,
    PPFLAG_DONT_READ            = 0x0100,
    PPFLAG_UNDEFINED_VAR        = 0x0200,
    PPFLAG_TRUNCATE_FILE_NAME   = 0x0400
} pp_flags;

#define PP_SPECIAL_MACRO    255

typedef struct macro_entry {
    struct macro_entry  *next;
    char                *replacement_list;
    unsigned char       parmcount;      /* PP_SPECIAL_MACRO - indicates special macro */
    char                name[1];
} MACRO_ENTRY;

#define PPTYPE_SIGNED       0
#define PPTYPE_UNSIGNED     1
#define PPTYPE_MACRO        1

typedef struct preproc_value {
    int                 type;   // PPTYPE_SIGNED or PPTYPE_UNSIGNED
    union {
        long            ivalue;
        unsigned long   uvalue;
    } val;
} PREPROC_VALUE;

typedef void        (* pp_walk_func)( const MACRO_ENTRY *me, const PREPROC_VALUE *val, void *cookie );
typedef const char  *(* pp_parent_func)( void **cookie );

extern void         PPENTRY PP_Init( char c, unsigned char spec_macros );
extern int          PPENTRY PP_Fini( void );
extern int          PPENTRY PP_FileInit( const char *filename, pp_flags ppflags );
extern void         PPENTRY PP_FileFini( void );
extern void         PPENTRY PP_IncludePathInit( incl_type incltype );
extern void         PPENTRY PP_IncludePathFini( incl_type incltype );
extern void         PPENTRY PP_IncludePathAdd( incl_type incltype, const char *path_list );
extern int          PPENTRY PP_IncludePathFind( const char *filename, size_t len, char *fullfilename, incl_type incltype, pp_parent_func fn );
extern int          PPENTRY PP_Char( void );
extern void         PPENTRY PP_Define( const char *ptr );
extern void         PPENTRY PP_Define_1( const char *ptr );
extern void         PPENTRY PP_MacrosWalk( pp_walk_func fn, void *cookie );
extern void         PPENTRY PP_MacrosInit( void );
extern void         PPENTRY PP_MacrosFini( void );

// Application defined functions

extern int          PPENTRY PP_MBCharLen( const char *p );
extern const char   * PPENTRY PP_GetEnv( const char *__name );
extern void         PPENTRY PP_OutOfMemory( void );

extern void         * PPENTRY PP_Malloc( size_t __size );
extern void         PPENTRY PP_Free( void *__ptr );
