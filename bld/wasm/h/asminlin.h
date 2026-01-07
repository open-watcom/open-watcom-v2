/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Symbols interface between inline assembler
*                  and C/C++/F77 compilers
*
****************************************************************************/


#ifndef INLINE_H
#define INLINE_H

typedef enum sym_state {
    SYM_UNDEFINED,
    SYM_INTERNAL,
    SYM_EXTERNAL,
    SYM_STACK,
#if defined( _STANDALONE_ )
    SYM_SEG,                // segment
    SYM_GRP,                // group
    SYM_PROC,               // procedure
    SYM_MACRO,              // macro
    SYM_CONST,              // constant - created with EQU, =, or /D on the cmdline
    SYM_LIB,                // included library
    SYM_EXT,                // extern def.
    SYM_CLASS_LNAME,        // lname entry for segment class ... not in symbol table
    SYM_STRUCT_FIELD,       // field defined in some structure
    SYM_STRUCT              // structure
#endif
} sym_state;

/*
 * This macro definition is used also for CvtTable in asmsym.c
 */
#define ASM_TYPES \
    ASM_TYPE( SYM_EMPTY,   MT_EMPTY ) /* no type */ \
    ASM_TYPE( SYM_INT1,    MT_BYTE  ) /* a 8-bit variable */ \
    ASM_TYPE( SYM_INT2,    MT_WORD  ) /* a 16-bit variable */ \
    ASM_TYPE( SYM_INT4,    MT_DWORD ) /* a 32-bit variable */ \
    ASM_TYPE( SYM_INT6,    MT_FWORD ) /* a 32-bit far pointer */ \
    ASM_TYPE( SYM_INT8,    MT_QWORD ) /* a 64-bit variable */ \
    ASM_TYPE( SYM_FLOAT4,  MT_DWORD ) /* a 32-bit floating point variable */ \
    ASM_TYPE( SYM_FLOAT8,  MT_QWORD ) /* a 64-bit floating point variable */ \
    ASM_TYPE( SYM_FLOAT10, MT_TBYTE ) /* a 80-bit floating point variable */ \
    ASM_TYPE( SYM_FLOAT16, MT_OWORD ) /* a 128-bit floating point variable */ \
    ASM_TYPE( SYM_NEAR2,   MT_NEAR  ) /* a 16-bit near routine */ \
    ASM_TYPE( SYM_NEAR4,   MT_NEAR  ) /* a 32-bit near routine */ \
    ASM_TYPE( SYM_FAR2,    MT_FAR   ) /* a 16-bit far routine */ \
    ASM_TYPE( SYM_FAR4,    MT_FAR   ) /* a 32-bit far routine */

typedef enum sym_type {
    #define ASM_TYPE(c,a)   c,
    ASM_TYPES
    #undef ASM_TYPE
} sym_type;

typedef enum fixup_types {
    FIX_SEG,
    FIX_LOBYTE,
    FIX_RELOFF8,
    FIX_RELOFF16,
    FIX_RELOFF32,
    FIX_OFF16,
    FIX_OFF32,
    FIX_PTR16,
    FIX_PTR32,
    FIX_FPPATCH
} fixup_types;

typedef enum fixup_options {
    OPTJ_NONE,
    OPTJ_EXPLICIT,          /* forward reference explicit J... SHORT */
    OPTJ_EXTEND,            /* forward reference JXX (SHORT 8086), can be extend by JMP NEAR */
    OPTJ_JXX,               /* forward reference JXX (SHORT/NEAR 386) */
    OPTJ_CALL               /* forward reference CALL (NEAR or converted FAR to NEAR) */
} fixup_options;

typedef enum prefix_reg {
    PREFIX_EMPTY    = 0,
    PREFIX_ES       = 0x26,
    PREFIX_CS       = 0x2E,
    PREFIX_SS       = 0x36,
    PREFIX_DS       = 0x3E,
    PREFIX_FS       = 0x64,
    PREFIX_GS       = 0x65
} prefix_reg;

#if defined( _STANDALONE_ )
/*
 * forward declaration
 */
typedef struct dir_node *dir_node_handle;
#endif

typedef struct asm_sym  *asm_sym_handle;

typedef struct asmfixup {
    struct asmfixup     *next;
#if defined( _STANDALONE_ )
    unsigned long       _offset;
  #define u_offset      _offset
#else
    struct {
        unsigned        _offset;
        unsigned char   _fppatch;
    } u;
  #define u_offset      u._offset
  #define u_fppatch     u._fppatch
#endif
    unsigned            fixup_loc;
    fixup_types         fixup_type;
    fixup_options       fixup_option;
    bool                external;
//    unsigned            line;
#if defined( _STANDALONE_ )
    struct asmfixup     *next_loc;
    asm_sym_handle      frame;          // frame of the fixup
    dir_node_handle     fixup_seg;     // segment of the fixup location
    asm_sym_handle      sym;
#else
    char                *name;
#endif
} asmfixup;

#if defined( _STANDALONE_ )
#define AsmCodeAddress  ( GetCurrAddr() )

typedef struct asmfixup_list {
    asmfixup            *head;
    asmfixup            *tail;
} asmfixup_list;

extern asmfixup_list    FixupList;
#else
extern uint_32          AsmCodeAddress;
extern uint_32          AsmCodeLimit;
extern asmfixup         *FixupHead;
#endif

extern unsigned char    *AsmCodeBuffer;    // code buffer for generated bytes

#if defined( _STANDALONE_ )
extern void             AsmLine( const char * );
#else
extern void             AsmLine( const char *, bool );
extern void             AsmEnvInit( int, int, int, bool );
extern void             AsmInit( void );
extern void             AsmFini( void );
extern void             AsmFiniRelocs( void );

extern void             *AsmQuerySymbol( const char *name );

/*
 * The following function is supplied by the user of the mini-assembler.
 * It returns either:
 *      SYM_UNDEFINED   - the name is not in the user's symbol table
 *      SYM_EXTERNAL    - the name is a static symbol in the user's symbol table
 *      SYM_STACK       - the symbol is an auto symbol in the user's symbol table
 */
extern sym_state        AsmQueryState( void *handle );

/*
 * The following function is supplied by the user of the mini-assembler.
 * It returns the type of the symbol via one of the 'enum sym_type'
 * constants.
 */
extern sym_type         AsmQueryType( void *handle );

#endif

extern void             AsmSymInit( void );
extern void             AsmSymFini( void );
extern void             AsmError( unsigned msgnum );
extern void             AsmWarning( unsigned msgnum );

#endif
