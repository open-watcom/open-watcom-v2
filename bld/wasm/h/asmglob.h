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

#ifdef _M_I86
#define ASMFAR __far
#else
#define ASMFAR
#endif

#define RC_ERROR                true
#define RC_OK                   false

#define ERROR                   -1
#define EMPTY                   -2

/* these come back from the jmp() routine as jump_flags */
#define SCRAP_INSTRUCTION       3
#define INDIRECT_JUMP           4

#define MAX_TOKEN               100     // there is no restriction for this number
#define MAX_LINE_LEN            512     // there is no restriction for this number
#define MAX_TOK_LEN             256
#define MAX_FILE_NAME           30
#define MAX_ID_LEN              247
#define MAX_MEMORY              1024
#define MAX_LINE                1024
#define MAX_LEDATA_THRESHOLD    1012
#define MAX_PUB_SIZE            100     // max # of entries in pubdef record
#define MAX_EXT_LENGTH          0x400   // max length ( in chars ) of extdef

/* max_ledata_threshold = 1024 - 6 for the header, -6 for space for fixups */

#define CMPLIT(s,c)     memcmp( s, c, sizeof( c ) )
#define CMPLITBEG(s,c)  memcmp( s, c, sizeof( c ) - 1 )
#define CMPLITEND(s,c)  memcmp( s - ( sizeof( c ) - 1 ), c, sizeof( c ) - 1 )

#define CPYLIT(s,c)     memcpy( s, c, sizeof( c ) )
#define CATLIT(s,c)     (char *)memcpy( s, c, sizeof( c ) - 1 ) + sizeof( c ) - 1
#define CATSTR(s,c,l)   (char *)memcpy( s, c, l ) + l

#include "asmerr.h"
#include "asmins.h"
#include "asmdefs.h"

#define NULLC                   '\0'
#define NULLS                   "\0"

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
    bool        sign_value;     /* TRUE -> WORD & DWORD are only unsigned
                                   signed #'s are SWORD & SDWORD instead. */
    bool        stop_at_end;
    bool        quiet;
    bool        banner_printed;
    bool        debug_flag;
    bool        output_comment_data_in_code_records;

    /* error handling stuff */
    int         error_count;
    int         warning_count;
    int         error_limit;
    char        warning_level;
    char        warning_error;

    char        *build_target;

    char        *code_class;
    char        *data_seg;
    char        *text_seg;
    char        *module_name;

  #ifdef DEBUG_OUT
    char        debug;
  #endif
    char        *default_name_mangler;
    bool        allow_c_octals;
    bool        emit_dependencies;
    bool        use_stdcall_at_number;
    bool        mangle_stdcall;
    bool        write_listing;
    bool        watcom_parms_passed_by_regs;
    smode       mode;
    int         locals_len;
    char        locals_prefix[3];
    char        trace_stack;
} global_options;

extern global_options Options;

typedef struct global_vars{
    uint    code_seg;
    bool    data_in_code:1;     /* have we just written data to a code seg */
    bool    dosseg:1;           /* have we seen a .dosseg directive */
    uint_16 sel_idx;
    uint_32 sel_start;
    uint    expand_count;
    uint    for_counter;
} global_vars;

extern global_vars Globals;

#endif

#endif
