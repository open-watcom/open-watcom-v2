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


#define BY_C_FRONT_END
#include "cgaux.h"
#include "cmemmgr.h"
#include "strsave.h"
#include "callinfo.h"

#define IS_KEYWORD(t)           (t >= FIRST_KEYWORD && t <= LAST_KEYWORD)
#define IS_ID_OR_KEYWORD(t)     (t == T_ID || t >= FIRST_KEYWORD && t <= LAST_KEYWORD)

typedef struct aux_entry {
    struct aux_entry    *next;
    aux_info            *info;         // also used by pre-compiled header
#if _CPU == 370
    int                 offset;
#endif
    char                name[1];
} aux_entry;

typedef struct inline_funcs {
    char       *name;       /* func name */
    byte_seq   *code;       /* sequence of bytes */
    hw_reg_set *parms;      /* parameter information */
    hw_reg_set returns;     /* function return information */
    hw_reg_set save;        /* registers not modified */
} inline_funcs;

typedef struct alt_inline_funcs {
    byte_seq        *byteseq;
    inline_funcs    alt_ifunc;
} alt_inline_funcs;

typedef struct toggle {
    char *name;
    int flag;
} toggle;

global aux_entry        *AuxList;
global aux_info         *CurrAlias;
global aux_entry        *CurrEntry;
global aux_info         *CurrInfo;
global aux_info         *DftCallConv;

#if _CPU == 386
global aux_info         STOSBInfo;
#endif

#define MAX_POSSIBLE_REG        8

#define MAXIMUM_BYTESEQ         4096
#define MAXIMUM_PARMSETS        32

#if _CPU == 386
#define DEFAULT_PCH_NAME        "wcc386.pch"
#else
#define DEFAULT_PCH_NAME        "wcc.pch"
#endif
