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

struct aux_entry {
    union {
        struct aux_info *info;
        unsigned        aux_info_index;         // for pre-compiled header
    };
    struct aux_entry    *next;
#if _CPU == 370
    int                 offset;
#endif
    char                name[1];
};

typedef int     aux_flags;
#define AUX_FLAG_FAR16  1

#include "cmemmgr.h"
#include "strsave.h"
#include "callinfo.h"

struct  inline_funcs {
        char       *name;       /* func name */
        byte_seq   *code;       /* sequence of bytes */
        hw_reg_set *parms;      /* parameter information */
        hw_reg_set returns;     /* function return information */
        hw_reg_set save;        /* registers not modified */
};

struct toggle {
        char *name;
        int flag;
    };

global struct aux_entry         *AuxList;
global struct aux_info          *CurrAlias;
global struct aux_entry         *CurrEntry;
global struct aux_info          *CurrInfo;
global struct aux_info          *DftCallConv;

#if _CPU == 386
global struct aux_info          STOSBInfo;
#endif
global call_class               CallClass;

#define MAX_POSSIBLE_REG        8

#define MAXIMUM_BYTESEQ         4096
#define MAXIMUM_PARMSETS        32

#if _CPU == 386
#define DEFAULT_PCH_NAME        "wcc386.pch"
#else
#define DEFAULT_PCH_NAME        "wcc.pch"
#endif
