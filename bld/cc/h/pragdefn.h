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
        int             aux_info_index;         // for pre-compiled header
    };
    struct aux_entry    *next;
#if _CPU == 370
    int                 offset;
#endif
    char                name[1];
};

typedef int     aux_flags;
#define AUX_FLAG_FAR16  1

struct aux_info {
        call_class      class;
        union {
#if _MACHINE == _ALPHA || _MACHINE == _PPC
            risc_byte_seq *code;
#else
            byte_seq    *code;
#endif
            int         code_size;      // for pre-compiled header
        };
        union {
            hw_reg_set  *parms;
            int         parms_size;     // for pre-compiled header
        };
#if _CPU == 370
        linkage_regs    *linkage;
#endif
#if _MACHINE == _ALPHA
#endif
        hw_reg_set      returns;
        hw_reg_set      streturn;
        hw_reg_set      save;
        union {
            char        *objname;
            int         objname_size;   // for pre-compiled header
        };
        int             use;            // use count
        aux_flags       flags;
        int             aux_info_index;
#if _MACHINE == _ALPHA
        char           *except_rtn;
#endif
};

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

global struct aux_info          DefaultInfo;
global struct aux_info          CdeclInfo;
global struct aux_info          PascalInfo;
global struct aux_info          FortranInfo;
global struct aux_info          SyscallInfo;            /* 04-jul-91 */
global struct aux_info          OptlinkInfo;            /* 04-jul-91 */
global struct aux_info          StdcallInfo;            /* 08-oct-92 */
global struct aux_info          FastCallInfo;
#if _CPU == 386
global struct aux_info          Far16CdeclInfo;
global struct aux_info          Far16PascalInfo;
global struct aux_info          STOSBInfo;
#endif
global call_class               CallClass;

#define MAX_POSSIBLE_REG   8

#define MAXIMUM_BYTESEQ         4096
#define MAXIMUM_PARMSETS        32

#if _CPU == 386
#define DEFAULT_PCH_NAME        "wcc386.pch"
#else
#define DEFAULT_PCH_NAME        "wcc.pch"
#endif
