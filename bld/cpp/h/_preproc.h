/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "preproc.h"

#define PPBUFSIZE           8192
#define HASH_SIZE           211

#define CC_ALPHA            1
#define CC_DIGIT            2

#define IS_END_OF_MACRO(m)  ((m)->token == PPT_NULL && (m)->data[0] == 'Z')

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

typedef struct macro_token {
    struct macro_token  *next;
    ppt_token           token;
    char                data[1];
} MACRO_TOKEN;

typedef struct  file_list {
    struct file_list    *prev_file;
    const char          *prev_bufptr;
    char                *filename;
    FILE                *handle;
    unsigned            linenum;
    char                buffer[PPBUFSIZE + 2];
} FILELIST;

typedef void        pp_callback(const char *, size_t len, const char *, int);

extern  void        PP_Dependency_List(pp_callback *);
extern  int         PP_Class(char __c);
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

extern  void        PPMacroVarInit( void );
extern  void        PPMacroVarFini( void );

extern  char        PP__DATE__[14];
extern  char        PP__TIME__[11];

extern  FILELIST    *PP_File;
extern  unsigned    PPLineNumber;
extern  const char  *PPTokenPtr;
extern  const char  *PPNextTokenPtr;
extern  MACRO_TOKEN *PPTokenList;
extern  MACRO_TOKEN *PPCurToken;
extern  pp_flags    PPFlags;
extern  char        PPSavedChar;    // saved char at end of token
