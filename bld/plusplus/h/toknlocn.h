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


#ifndef __TOKNLOCN_H__
#define __TOKNLOCN_H__

// TOKNLOCN.H -- define a token location
//
// 92/11/17 -- J.W.Welch        -- extracted from SRCFILE

typedef struct token_locn       TOKEN_LOCN;
typedef struct sym_token_locn   SYM_TOKEN_LOCN;

struct token_locn               // TOKEN_LOCN -- location of source token
{   SRCFILE src_file;           // - source file
    LINE_NO line;               // - line number
    COLUMN_NO column;           // - column
};

struct sym_token_locn           // SYM_TOKEN_LOCN -- location of source token
{                               //   and dwarf handle (used by symbol table)
    TOKEN_LOCN  tl;             // - token location
    union {                     // - either:
        uint_32     dwh;        // - - dwarf handle for browser
        void       *cg_handle;  // - - handle during code-generation
    };
};

#define TokenLocnAssign( d, s ) \
        (d).src_file = (s).src_file; \
        (d).line = (s).line; \
        (d).column = (s).column;

#define TokenLocnClear( d ) \
        (d).src_file = NULL; \
        (d).line = 0; \
        (d).column = 0;

// PROTOTYPES

SYM_TOKEN_LOCN* SymbolLocnAlloc(// ALLOCATE A SYM_TOKEN_LOCN, IF REQ'D
    SYM_TOKEN_LOCN** owner )    // - addr[ ptr to location ]
;
void SymbolLocnDefine(          // DEFINE LOCATION SYMBOL
    TOKEN_LOCN *sym_locn,       // - symbol location (NULL for current source file)
    SYMBOL sym )                // - the symbol
;
void SymbolLocnFree(            // FREE A SYM_TOKEN_LOCN
    SYM_TOKEN_LOCN* loc )       // - the entry
;

// pre-compiled header support

SYM_TOKEN_LOCN *SymTokenLocnGetIndex( SYM_TOKEN_LOCN * );
SYM_TOKEN_LOCN *SymTokenLocnMapIndex( SYM_TOKEN_LOCN * );
#endif
