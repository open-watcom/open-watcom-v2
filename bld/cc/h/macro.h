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
* Description:  Preprocessor related data types and constants.
*
****************************************************************************/



enum    special_macros {
    MACRO_DATE,
    MACRO_FILE,
    MACRO_LINE,
    MACRO_STDC,
    MACRO_STDC_HOSTED,
    MACRO_STDC_LIB_EXT1,
    MACRO_STDC_VERSION,
    MACRO_TIME,
    MACRO_FUNC,
};

struct  macro_parm {
    struct macro_parm   *next_macro_parm;
    char                *parm;
};

/* Actual macro definition is at (char *)mentry + mentry->macro_defn */
typedef enum macro_flags {
    MFLAG_NONE,
    MFLAG_DEFINED_BEFORE_FIRST_INCLUDE  =   0x01,
    MFLAG_CAN_BE_REDEFINED              =   0x02,
    MFLAG_USER_DEFINED                  =   0x04,
    MFLAG_REFERENCED                    =   0x08,
    MFLAG_VAR_ARGS                      =   0x10,   // macro has varargs.
} macro_flags;

typedef struct  macro_entry {
    union {
        struct  macro_entry     *next_macro;
        int                     macro_index;    /* for pre-compiled header */
    };
    unsigned    macro_defn;     /* offset to defn, 0 ==>special macro name*/
    unsigned    macro_len;      /* length of macro definition */
    unsigned    parm_count;     /* special macro indicator if defn == 0 */
    macro_flags macro_flags;    /* flags */
    source_loc  src_loc;        /* where macro defined, for diagnostic */
    char        macro_name[1];  /* name,parms, and macro definition */
} MEDEFN, *MEPTR;

typedef struct  macro_stack {
    struct  macro_stack *stacked_macro;
    MEPTR   prev_macro;
    char    **macro_parms;  /* pointer to array of actual parms */
    char    *prev_ptr;
    int     macro_class;    /* T_MACRO or T_MACRO_PARM */
    char    macro_definition[1]; /* copy of current macro definition */
} MSTACK, *MSTACKPTR;
