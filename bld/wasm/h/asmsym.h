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


#ifndef ASMSYM_H
#include "watcom.h"

enum fixup_types {
        FIX_SEG,
        FIX_RELOFF8,
        FIX_RELOFF16,
        FIX_RELOFF32,
        FIX_OFF16,
        FIX_OFF32,
        FIX_PTR16,
        FIX_PTR32
};

enum sym_state {
    SYM_UNDEFINED,
    SYM_INTERNAL,
    SYM_EXTERNAL,
    SYM_STACK,

#ifdef _WASM_
    SYM_SEG,    // segment
    SYM_GRP,    // group
    SYM_PROC,   // procedure
    SYM_MACRO,  // macro
    SYM_CONST,  // constant - created with EQU, =, or /D on the cmdline
    SYM_LIB,    // included library
    SYM_EXT,    // extern def.
    SYM_LNAME,  // lname entry
    SYM_CLASS_LNAME,    // lname entry for segment class ... not in symbol table
    SYM_STRUCT_FIELD,   // field defined in some structure
    SYM_STRUCT          // structure
#endif

};

enum sym_type {
        SYM_INT1,               /* a byte variable */
        SYM_INT2,               /* a word variable */
        SYM_INT4,               /* a dword variable */
        SYM_INT6,               /* a 32-bit far pointer */
        SYM_FLOAT4,             /* a 4 byte floating point variable */
        SYM_FLOAT8,             /* an 8 byte floating point variable */
        SYM_FLOAT10,            /* a 10 byte floating point variable */
        SYM_NEAR2,              /* a 16-bit near routine */
        SYM_NEAR4,              /* a 32-bit near routine */
        SYM_FAR2,               /* a 16-bit far routine */
        SYM_FAR4                /* a 32-bit far routine */
};

struct asmfixup {
        struct asmfixup         *next;
        char                    *name;
        unsigned long           offset;
        unsigned                fix_loc;
        enum fixup_types        fix_type;
        char                    external;

#ifdef _WASM_
        int_8                   frame;          // frame of the fixup
        uint_16                 frame_datum;    // frame_datum of the fixup
        struct dir_node         *def_seg;       // segment fixup is in
#endif

};

typedef struct asm_sym {
        struct asm_sym  *next;
        char            *name;

#ifdef _WASM_
        uint            grpidx;
        uint            segidx;
        uint_32         offset;
        uint_8          public;
        uint_8          first_size;   /* size of 1st initializer in bytes */
        uint_8          first_length; /* size of 1st initializer--elts. dup'd */
        uint_8          total_size;   /* total number of bytes (sizeof) */
        uint_8          total_length; /* total number of elements (lengthof) */
        char            *(*mangler)( struct asm_sym *sym, char *buffer );
#else
        long            addr;
#endif
        int             mem_type;
        enum sym_state  state;
        struct asmfixup *fixup;
} asm_sym;
extern struct asm_sym *AsmLookup( char *name );
extern struct asm_sym *AsmAdd( struct asm_sym *symbol );
extern void AsmSymFini(void);
extern struct asm_sym **AsmFind( char *name );
extern struct asm_sym *AsmGetSymbol( char *name );
#ifdef _WASM_
    extern void AsmTakeOut( char *name );
    extern int AsmChangeName( char *old, char *new );
#endif

/*
   The following function is supplied by the user of the mini-assembler.
   It returns either:
        SYM_UNDEFINED   - the name is not in the user's symbol table
        SYM_EXTERNAL    - the name is a static symbol in the user's
                                symbol table
        SYM_STACK       - the symbol is an auto symbol in the user's
                                symbol table
*/
extern enum sym_state   AsmQueryExternal( char *name );

/*
   The following function is supplied by the user of the mini-assembler.
   It returns the type of the symbol via one of the 'enum sym_type'
   constants.
*/
extern enum sym_type    AsmQueryType( char *name );

extern struct asmfixup  *FixupHead;

#define ASMSYM_H
#endif
