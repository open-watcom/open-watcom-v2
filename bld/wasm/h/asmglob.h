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


#ifndef ASMGLOB_H
#include <stdio.h>
#include <watcom.h>

#define MAX_TOKEN               100     // there is no restriction for this number
#define MAX_LINE_LEN            256     // there is no restriction for this number
#define MAX_TOK_LEN             80
#define MAX_FILE_NAME           30
#define MAX_ID_LEN              247
#define MAX_MEMORY              1024
#define MAX_LINE                1024
#define MAX_LEDATA_THRESHOLD    1012
#define MAX_PUB_SIZE            100     // max # of entries in pubdef record
#define MAX_EXT_LENGTH          0x400   // max length ( in chars ) of extdef

/* max_ledata_threshold = 1024 - 6 for the header, -6 for space for fixups */

#define ERROR                   -1
#define NOT_ERROR               1
#define EMPTY                   -2
#define NOT_EMPTY               2
#define EMPTY_U_LONG            0xFFFFFFFF // U_LONG is Unsigned Long
/* these come back from the jmp() routine */
#define SCRAP_INSTRUCTION       3
#define INDIRECT_JUMP           4

#define TRUE                    1
#define FALSE                   0

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

enum naming_conventions {
    DO_NOTHING,
    ADD_USCORES,            /*  put uscores on the front of labels
                             *  & the back of procedures
                             *  this is what the compiler does with /3r
                             */
    REMOVE_USCORES          /*
                             * assume that the user manually put uscores
                             * as described above into the assembly file
                             * and take them off
                             */
};

#endif

#if ( defined(_WASM_) && !defined( ASMSYM_H ) )

#define DELIM                   " ,\t\0"
#define T_UNDEFINED             -1

enum {
    PASS_1 = 0,
    PASS_2
};

enum {
    ASM,
    ERR,
    OBJ
};
#define FILE_TYPES 3

typedef struct {
    FILE        *file[FILE_TYPES];      // ASM, ERR and OBJ
    char        *fname[FILE_TYPES];
} File_Info;    // Information about the source and object files

#define ASM_EXT "asm"
#define ERR_EXT "err"

#ifdef __QNX__
#define OBJ_EXT "o"
#else
#define OBJ_EXT "obj"
#endif

typedef char bool;

typedef struct queuenode {
    void *next;
    void *data;
} queuenode;

/* stuff used by condasm.c,
 * here since we need it so we can tell asmeval where to go if
 * we are in the middle of a false ifdef
 */

enum if_state {
    ACTIVE,                 /* current IF cond is true */
    LOOKING_FOR_TRUE_COND,  /* current IF cond is false, looking for elseif */
    DONE                    /* done TRUE section of current if, just nuke
                               everything until we see an endif */
};
extern enum if_state CurState;

enum fpe {
    DO_FP_EMULATION,
    NO_FP_EMULATION,
    NO_FP_ALLOWED
};

typedef struct global_options {
    bool        sign_value;     /* TRUE -> WORD & DWORD are only unsigned
                                   signed #'s are SWORD & SDWORD instead. */
    bool        stop_at_end;
    bool        quiet;
    bool        banner_printed;
    bool        debug_flag;
    char        naming_convention;
    enum fpe    floating_point;
    bool        output_data_in_code_records;

    /* error handling stuff */
    char        error_count;
    char        warning_count;
    char        error_limit;
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
    char *      default_name_mangler;
    bool        allow_c_octals;
} global_options;

extern global_options Options;

typedef struct global_vars{
    uint    code_seg;
    uint    data_in_code:1,     /* have we just written data to a code seg */
            dosseg:1;           /* have we seen a .dosseg directive */
    uint    sel_idx;
    uint_32 sel_start;
    uint    expand_count;
    uint    for_counter;
} global_vars;

extern global_vars Globals;

#endif

#ifndef ASMGLOB_H
#define ASMGLOB_H
#endif
