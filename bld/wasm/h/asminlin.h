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
* Description:  Symbols interface between inline assembler
*                  and C/C++/F77 compilers
*
****************************************************************************/


#ifndef INLINE_H
#define INLINE_H

enum sym_state {
        SYM_UNDEFINED,
        SYM_INTERNAL,
        SYM_EXTERNAL,
        SYM_STACK,

#if defined( _STANDALONE_ )
        SYM_SEG,            // segment
        SYM_GRP,            // group
        SYM_PROC,           // procedure
        SYM_MACRO,          // macro
        SYM_CONST,          // constant - created with EQU, =, or /D on the cmdline
        SYM_LIB,            // included library
        SYM_EXT,            // extern def.
        SYM_CLASS_LNAME,    // lname entry for segment class ... not in symbol table
        SYM_STRUCT_FIELD,   // field defined in some structure
        SYM_STRUCT          // structure
#endif

};

/* It must correspond with CvtTable in asmsym.c */
enum sym_type {
        SYM_INT1,               /* a byte variable */
        SYM_INT2,               /* a word variable */
        SYM_INT4,               /* a dword variable */
        SYM_INT6,               /* a 32-bit far pointer */
        SYM_INT8,               /* a qword variable */
        SYM_FLOAT4,             /* a 4 byte floating point variable */
        SYM_FLOAT8,             /* an 8 byte floating point variable */
        SYM_FLOAT10,            /* a 10 byte floating point variable */
        SYM_NEAR2,              /* a 16-bit near routine */
        SYM_NEAR4,              /* a 32-bit near routine */
        SYM_FAR2,               /* a 16-bit far routine */
        SYM_FAR4                /* a 32-bit far routine */
};

enum fixup_types {
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
};

enum fixup_options {
        OPTJ_NONE,
        OPTJ_EXPLICIT,           /* forward reference explicit J... SHORT */
        OPTJ_EXTEND,             /* forward reference JXX (SHORT 8086), can be extend by JMP NEAR */
        OPTJ_JXX,                /* forward reference JXX (SHORT/NEAR 386) */
        OPTJ_CALL                /* forward reference CALL (NEAR or converted FAR to NEAR) */
};

enum prefix_reg {
#if defined( _STANDALONE_ )
        PREFIX_EMPTY = EMPTY,
#endif
        PREFIX_ES = 0x26,
        PREFIX_CS = 0x2E,
        PREFIX_SS = 0x36,
        PREFIX_DS = 0x3E,
        PREFIX_FS = 0x64,
        PREFIX_GS = 0x65
};

#if !defined( _STANDALONE_ )

extern void             *AsmQuerySymbol( const char *name );

/*
   The following function is supplied by the user of the mini-assembler.
   It returns either:
        SYM_UNDEFINED   - the name is not in the user's symbol table
        SYM_EXTERNAL    - the name is a static symbol in the user's
                                symbol table
        SYM_STACK       - the symbol is an auto symbol in the user's
                                symbol table
*/
extern enum sym_state   AsmQueryState( void *handle );

/*
   The following function is supplied by the user of the mini-assembler.
   It returns the type of the symbol via one of the 'enum sym_type'
   constants.
*/
extern enum sym_type    AsmQueryType( void *handle );

#endif

struct asmfixup {
        struct asmfixup         *next;
#if defined( _STANDALONE_ )
        unsigned long           _offset;
  #define u_offset              _offset
#else
        struct {
            unsigned            _offset;
            unsigned char       _fppatch;
        } u;
  #define u_offset              u._offset
  #define u_fppatch             u._fppatch
#endif
        unsigned                fixup_loc;
        enum fixup_types        fixup_type;
        enum fixup_options      fixup_option;
        char                    external;
//        unsigned                line;

#if defined( _STANDALONE_ )
        struct asmfixup         *next_loc;
        struct asm_sym          *frame;         // frame of the fixup
        struct dir_node         *fixup_seg;     // segment of the fixup location
        struct asm_sym          *sym;
#else
        char                    *name;
#endif

};

#if defined( _STANDALONE_ )
#define AsmCodeAddress          ( GetCurrAddr() )
#else
extern uint_32                  AsmCodeAddress;
#endif

extern struct asmfixup          *FixupHead;
extern unsigned char            *AsmCodeBuffer;    // code buffer for generated bytes

#if defined( _STANDALONE_ )
extern void                     AsmLine( char * );
extern void                     AsmSymInit( void );
#else
extern void                     AsmLine( char *, bool );
extern void                     AsmInit( int, int, int, bool );
extern void                     AsmSaveCPUInfo( void );
extern void                     AsmRestoreCPUInfo( void );
#endif
extern void                     AsmSymFini( void );

#endif
