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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"

#define PPBUFSIZE           8192
#define HASH_SIZE           211

#define PPTYPE_SIGNED       0
#define PPTYPE_UNSIGNED     1

#define PPFLAG_PREPROCESSING    0x0001
#define PPFLAG_EMIT_LINE        0x0002
#define PPFLAG_SKIP_COMMENT     0x0004
#define PPFLAG_KEEP_COMMENTS    0x0008
#define PPFLAG_IGNORE_INCLUDE   0x0010
#define PPFLAG_DEPENDENCIES     0x0020
#define PPFLAG_ASM_COMMENT      0x0040
#define PPFLAG_IGNORE_CWD       0x0080
#define PPFLAG_IGNORE_DEFDIRS   0x0100
#define PPFLAG_DB_KANJI         0x0200
#define PPFLAG_DB_CHINESE       0x0400
#define PPFLAG_DB_KOREAN        0x0800
#define PPFLAG_UTF8             0x1000
#define PPFLAG_DONT_READ        0x4000
#define PPFLAG_UNDEFINED_VAR    0x8000

#define CC_ALPHA            1
#define CC_DIGIT            2

#define PPINCLUDE_USR       0
#define PPINCLUDE_SYS       1
#define PPINCLUDE_SRC       2

typedef enum {
    PPT_NULL            = 0,
    PPT_SHARP_SHARP     = 1,
    PPT_LAST_TOKEN      = 2,
    PPT_EOF             = 3,
    PPT_SHARP           = '#',
    PPT_LEFT_PAREN      = '(',
    PPT_RIGHT_PAREN     = ')',
    PPT_COMMA           = ',',
    PPT_ID              = 'A',
    PPT_TEMP_ID         = 'a',
    PPT_SAVED_ID        = 'B',
    PPT_COMMENT         = 'C',
    PPT_MACRO_PARM      = 'P',
    PPT_NUMBER          = '0',
    PPT_LITERAL         = '\"',
    PPT_WHITE_SPACE     = ' ',
    PPT_OTHER           = '$',
    PPT_MAXSIZE = 0xFFFF
} ppt_token;

typedef struct macro_entry {
    struct macro_entry *next;
    char            *replacement_list;
    unsigned char   parmcount;      /* 255 - indicates special macro */
    char            name[1];
} MACRO_ENTRY;
#define PP_SPECIAL_MACRO        255

typedef struct macro_token {
    struct macro_token  *next;
    ppt_token           token;
    char                data[1];
} MACRO_TOKEN;

typedef struct  file_list {
    struct file_list *prev_file;
    char             *prev_bufptr;
    char             *filename;
    FILE             *handle;
    unsigned         linenum;
    char             buffer[PPBUFSIZE+2];
} FILELIST;

typedef struct preproc_value {
    int             type;   // PPTYPE_SIGNED or PPTYPE_UNSIGNED
    union {
        long int    ivalue;
        unsigned long uvalue;
    } val;
} PREPROC_VALUE;

typedef void        pp_callback(const char *, size_t len, const char *, int);

extern  int         PP_Init( const char *__filename, unsigned __flags, const char *__incpath);
extern  int         PP_Init2( const char *filename, unsigned flags, const char *include_path, const char *leadbytes );
extern  void        PP_Dependency_List(pp_callback *);
extern  void        PP_SetLeadBytes( const char *bytes );
extern  int         PP_Char(void);
extern  int         PP_Class(char __c);
extern  void        PP_Fini(void);
extern  void        PP_Define( const char *__p );
extern  MACRO_ENTRY *PP_AddMacro( const char *__name, size_t len );
extern  MACRO_ENTRY *PP_MacroLookup( const char *__name, size_t len );
extern  MACRO_ENTRY *PP_ScanMacroLookup( const char *__name );
extern  const char  *PP_ScanToken( const char *__p, ppt_token *__token );
extern  int         PP_ScanNextToken( ppt_token *__token );
extern  const char  *PP_SkipWhiteSpace( const char *__p, bool *__white_space );
extern  const char  *PP_ScanName( const char *__p );
extern  int         PPEvalExpr( const char *__p, const char **__endptr, PREPROC_VALUE *__val );
extern  void        PP_ConstExpr( PREPROC_VALUE * );
extern  MACRO_TOKEN *PPNextToken(void);
extern  MACRO_TOKEN *NextMToken(void);
extern  void        DeleteNestedMacro(void);
extern  void        DoMacroExpansion( MACRO_ENTRY *__me );
extern  void        PP_AddIncludePath( const char *path_list );
extern  void        PP_IncludePathInit( void );
extern  void        PP_IncludePathFini( void );
extern  int         PP_FindInclude( const char *filename, size_t len, char *fullfilename, int incl_type );

extern  void        *PP_Malloc( size_t __size );
extern  void        PP_Free( void *__ptr );
extern  void        PP_OutOfMemory(void);

extern  const char  *PP_GetEnv( const char *__name );

extern  void        PreprocVarInit( void );
extern  void        PPMacroVarInit( void );

extern  FILELIST    *PP_File;
extern  unsigned    PPLineNumber;
extern  const char  *PPTokenPtr;
extern  const char  *PPNextTokenPtr;
extern  MACRO_TOKEN *PPTokenList;
extern  MACRO_TOKEN *PPCurToken;
extern  unsigned    PPFlags;
extern  char        PPSavedChar;    // saved char at end of token
extern  char        PreProcChar;
extern  MACRO_ENTRY *PPHashTable[HASH_SIZE];
