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
* Description:  Assmebler symbols internal structures and definitions.
*
****************************************************************************/


#ifndef _ASMSYM_H_
#define _ASMSYM_H_

#include "asminlin.h"

typedef enum {
        MT_EMPTY,
        MT_BYTE,
        MT_WORD,
        MT_DWORD,
        MT_QWORD,
        MT_FWORD,
        MT_TBYTE,
        MT_OWORD,
        MT_SHORT,
        MT_NEAR,
        MT_FAR,
        MT_PTR,
#if defined( _STANDALONE_ )
        MT_SBYTE,
        MT_SWORD,
        MT_SDWORD,
        MT_STRUCT,
        MT_PROC,
        MT_ABS,
#endif
        MT_ERROR
} memtype;

#if defined( _STANDALONE_ )
typedef enum {
    LANG_NONE     = 0,
    LANG_C        = 1,
    LANG_SYSCALL  = 2,
    LANG_STDCALL  = 3,
    LANG_PASCAL   = 4,
    LANG_FORTRAN  = 5,
    LANG_BASIC    = 6,
    LANG_WATCOM_C = 7
} lang_type;
#endif

typedef struct asm_sym {
        struct asm_sym  *next;
        char            *name;

#if defined( _STANDALONE_ )
        struct asm_sym  *segment;
        struct asm_sym  *structure;     /* structure type name */
        uint_32         offset;
        uint_32         first_size;     /* size of 1st initializer in bytes */
        uint_32         first_length;   /* size of 1st initializer--elts. dup'd */
        uint_32         total_size;     /* total number of bytes (sizeof) */
        uint_32         total_length;   /* total number of elements (lengthof) */
        uint_32         count;
        char            *(*mangler)( struct asm_sym *sym, char *buffer );
        bool            public;
        bool            referenced;
        lang_type       langtype;
#else
        long            addr;
#endif
        memtype         mem_type;
        enum sym_state  state;
        struct asmfixup *fixup;
} asm_sym;

extern  struct asm_sym  *AsmLookup( const char *name );
extern  struct asm_sym  *AsmGetSymbol( const char *name );
extern  void            FreeASym( struct asm_sym *sym );

#if defined( _STANDALONE_ )

extern  void            AsmTakeOut( const char *name );
extern  bool            AsmChangeName( const char *old, const char *new );
extern  void            WriteListing( void );

extern  struct asm_sym  *AllocDSym( const char *, bool );

#define IS_SYM_COUNTER( x ) ( ( x[0] == '$' ) && ( x[1] == 0 ) )

#endif

#endif
