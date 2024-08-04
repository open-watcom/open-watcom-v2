/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Languages defined for syntax highlighting.
*
****************************************************************************/


#ifndef _LANG_INCLUDED
#define _LANG_INCLUDED

typedef enum {
    #define pick(enum,enumrc,name,namej,fname,desc,filter) VI_LANG_ ## enum,
    #include "langdef.h"
    #undef pick
    VI_LANG_MAX
} lang_t;

typedef struct vi_word {
    char                *str;
    unsigned            len;
} vi_word;

typedef struct hash_entry {
    struct hash_entry   *next;
    vi_word             keyword;
    bool                real;
} hash_entry;

typedef struct tag_lang_info {
    hash_entry          *keyword_table;
    int                 table_entries;
    int                 ref_count;
    char                *read_buf;
} lang_info;

bool    IsKeyword( const char *start, const char *end, bool case_ignore );
bool    IsPragma( const char *start, const char *end );
bool    IsDeclspec( const char *start, const char *end );
void    LangInit( lang_t );
void    LangFini( lang_t );
void    LangFiniAll( void );

#endif
