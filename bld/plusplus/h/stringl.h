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


#ifndef _STRINGL_H
#define _STRINGL_H

typedef struct string_literal_t STRING_LITERAL;
typedef STRING_LITERAL *STRING_CONSTANT;

typedef enum string_literal_flags { // string literal flags
    STRLIT_NONE    = 0,
    STRLIT_CONCAT  = 0x01,  // - is result of concatenation
    STRLIT_MLINE   = 0x02,  // - concat parts were on different lines
    STRLIT_WIDE    = 0x04,  // - this is L"string"
} string_literal_flags;

struct string_literal_t {
    STRING_CONSTANT         next;       // - next entry
    back_handle             cg_handle;  // - handle during code generation
    target_size_t           alloc_len;  // - allocated length in bytes
    target_size_t           len;        // - current used length in bytes including null terminate character
    fe_seg_id               segid;      // - segment containing string bytes
    string_literal_flags    flags;      // - attributes
    char                    string[1];  // - data
};

extern void             StringTrash( STRING_CONSTANT );
extern STRING_CONSTANT  StringCreate( const char *, size_t, bool );
extern STRING_CONSTANT  StringConcat( STRING_CONSTANT, STRING_CONSTANT );
extern void             StringConcatDifferentLines( STRING_CONSTANT );
extern bool             StringSame( STRING_CONSTANT, STRING_CONSTANT );
extern target_size_t    StringLength( STRING_CONSTANT );
extern target_size_t    StringAWStrLen( STRING_CONSTANT );
extern target_size_t    StringAlign( STRING_CONSTANT );
extern target_size_t    StringCharSize( STRING_CONSTANT );

extern void StringWalk(         // WALK DEFINED STRING LITERALS
    void (*walker)              // - walking routine
        ( STRING_CONSTANT ) )   // - - current string
;

extern STRING_CONSTANT  StringGetIndex( STRING_CONSTANT );
extern STRING_CONSTANT  StringMapIndex( STRING_CONSTANT );

#endif
