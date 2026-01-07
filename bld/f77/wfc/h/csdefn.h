/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


#include "symtypes.h"

#define CSTYPES \
CSTYP( CS_IF,            PR_IF ) \
CSTYP( CS_ELSEIF,        PR_ELSEIF ) \
CSTYP( CS_ELSE,          PR_ELSE ) \
CSTYP( CS_GUESS,         PR_GUESS ) \
CSTYP( CS_ADMIT,         PR_ADMIT ) \
CSTYP( CS_SELECT,        PR_SELECT ) \
CSTYP( CS_CASE,          PR_CASE ) \
CSTYP( CS_OTHERWISE,     PR_OTHERWISE ) \
CSTYP( CS_ATEND,         PR_ATEND ) \
CSTYP( CS_REMOTEBLOCK,   PR_REMBLK ) \
CSTYP( CS_LOOP,          PR_LOOP ) \
CSTYP( CS_WHILE,         PR_WHILE ) \
CSTYP( CS_DO,            PR_DO ) \
CSTYP( CS_COMPUTED_GOTO, PR_GOTO ) \
CSTYP( CS_DO_WHILE,      PR_DO )

typedef enum cstype {
    #define CSTYP(id,stmt) id,
    CSTYPES
    #undef CSTYP
    CS_EMPTY_LIST
} cstype;

typedef union cs_info {
    struct do_entry     *do_parms;      // iterative DO-loop
    struct case_entry   *cases;         // SELECT statement and computed GOTO
    sym_id              rb;             // REMOTE BLOCKs
    stmt_num            term_stmt_no;   // for DO WHILE
} cs_info;

typedef struct csnode {
    struct csnode       *link;
    union cs_info       cs_info;
    label_id            branch;
    label_id            bottom;
    label_id            cycle;
    block_num           block;
    cstype              typ;
    char                label[1];
} csnode;

typedef union cs_label {
    sym_id              st_label;
    label_id            g_label;
} cs_label;

typedef struct case_entry {
    struct case_entry   *link;
    union cs_label      label;
    intstar4            low;
    intstar4            high;
    sym_id              sel_expr;
    TYPE                sel_type;
    bool                multi_case;
} case_entry;

typedef struct do_entry {
    sym_id              do_parm;
    stmt_num            term_stmt_no;
    sym_id              increment;
    sym_id              iteration;
    intstar4            incr_value;
} do_entry;
