/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WASM main header file contains globals and limits
*
****************************************************************************/


#ifndef _ASMGLOB_H_INCLUDED
#define _ASMGLOB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "watcom.h"
#include "bool.h"
#include "asmconst.h"
#include "asmerr.h"
#include "asmins.h"
#include "asmdefs.h"


#define EXIT_ERROR              1

#define EMPTY                   -2

/* these come back from the jmp() routine as jump_flags */
#define SCRAP_INSTRUCTION       3
#define INDIRECT_JUMP           4

#define CMPLIT(s,c)     memcmp( s, c, sizeof( c ) )
#define CMPLITBEG(s,c)  memcmp( s, c, sizeof( c ) - 1 )
#define CMPLITEND(s,c)  memcmp( s - ( sizeof( c ) - 1 ), c, sizeof( c ) - 1 )

#define CPYLIT(s,c)     memcpy( s, c, sizeof( c ) )
#define CATLIT(s,c)     (char *)memcpy( s, c, sizeof( c ) - 1 ) + sizeof( c ) - 1
#define CATSTR(s,c,l)   (char *)memcpy( s, c, l ) + l

#define ReadU16(p)      GET_LE_16(*(uint_16*)(p))
#define ReadU32(p)      GET_LE_32(*(uint_32*)(p))
#define ReadS16(p)      GET_LE_16(*(int_16*)(p))
#define ReadS32(p)      GET_LE_32(*(int_32*)(p))

#define WriteU16(p,n)   (*(uint_16*)(p) = GET_LE_16((uint_16)(n)))
#define WriteU32(p,n)   (*(uint_32*)(p) = GET_LE_32((uint_32)(n)))
#define WriteS16(p,n)   (*(int_16*)(p) = GET_LE_16((int_16)(n)))
#define WriteS32(p,n)   (*(int_32*)(p) = GET_LE_32((int_32)(n)))

#define BIT_012                 0x07
#define BIT_345                 0x38
#define BIT_67                  0xC0
#define NOT_BIT_012             0xF8
#define NOT_BIT_345             0xC7
#define NOT_BIT_67              0x3F

#define BYTE_1                  1
#define BYTE_2                  2
#define BYTE_4                  4
#define BYTE_6                  6
#define BYTE_8                  8
#define BYTE_10                 10
#define BYTE_16                 16

enum fpe {
    DO_FP_EMULATION,
    NO_FP_EMULATION,
    NO_FP_ALLOWED
};

extern enum fpe floating_point;

#if defined( _STANDALONE_ )

#define DELIM                   " ,\t\0"
#define T_UNDEFINED             -1

enum {
    PASS_1 = 0,
    PASS_2
};

enum {
    ASM,
    ERR,
    OBJ,
    LST
};
#define FILE_TYPES      4

typedef struct {
    FILE        *file[FILE_TYPES];      // ASM, ERR and OBJ
    char        *fname[FILE_TYPES];
} File_Info;    // Information about the source and object files

extern File_Info        AsmFiles;   // files information

#define ASM_EXT "asm"
#define ERR_EXT "err"
#define LST_EXT "lst"

#ifdef __UNIX__
#define OBJ_EXT "o"
#else
#define OBJ_EXT "obj"
#endif

typedef enum smode {
    MODE_MASM6  = 0,
    MODE_MASM5  = 1,
    MODE_WATCOM = 2,
    MODE_TASM   = 4,
    MODE_IDEAL  = 8
} smode;

typedef struct global_options {
    bool        sign_value;     /* true -> WORD & DWORD are only unsigned
                                   signed #'s are SWORD & SDWORD instead. */
    bool        stop_at_end;
    bool        quiet;
    bool        banner_printed;
    bool        debug_info;
    bool        output_comment_data_in_code_records;

    /* error handling stuff */
    int         error_count;
    int         warning_count;
    int         error_limit;
    char        warning_level;
    bool        warning_error;

    char        *build_target;

    char        *code_class;
    char        *data_seg;
    char        *text_seg;
    char        *module_name;

  #ifdef DEBUG_OUT
    bool        int_debug;
  #endif
    char        *default_name_mangler;
    bool        allow_c_octals;
    bool        emit_dependencies;
    bool        use_stdcall_at_number;
    bool        mangle_stdcall;
    bool        write_listing;
    bool        watcom_parms_passed_by_regs;
    smode       mode_init;
    smode       mode;
    int         locals_len;
    char        locals_prefix[3];
    char        trace_stack;
    bool        optimization;
} global_options;

extern global_options Options;

typedef struct global_vars{
    uint    code_seg;
    bool    data_in_code;       /* have we just written data to a code seg */
    bool    dosseg;             /* have we seen a .dosseg directive */
    uint_16 sel_idx;
    uint_32 sel_start;
    uint    expand_count;
    uint    for_counter;
} global_vars;

extern global_vars Globals;

#endif

extern const asm_ins    ASMI86FAR *get_instruction( char *string );

#endif
