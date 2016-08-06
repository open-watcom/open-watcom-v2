/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Structures and functions to manipulate labels.
*
****************************************************************************/


#include "targsys.h"
#include "optopts.h"
#include "offset.h"
#include "rtclass.h"
#ifndef NDEBUG
#include "useinfo.h"
#endif


#define RTIDX2SYM(i)    ((cg_sym_handle)(pointer_int)((i) + 1))
#define SYM2RTIDX(s)    ((rt_class)(pointer_int)(s) - 1)

#define SYM2IMPHDL(s)   ((import_handle)(pointer_int)(s))
#define IMPHDL2SYM(i)   ((cg_sym_handle)(pointer_int)(i))

typedef enum {
    CODELABEL           = 0x0001,
    KEEPLABEL           = 0x0002,
    DYINGLABEL          = 0x0004,
    REACHED             = 0x0008,
    SHORTREACH          = 0x0010,
    CONDEMNED           = 0x0020,
    RUNTIME             = 0x0040,
    REDIRECTION         = 0x0080,
    UNIQUE              = 0x0100,
    COMMON_LBL          = 0x0200,
    UNREACHABLE         = 0x0400,
    OWL_OWNED           = 0x0800,
    WEIRD_PPC_ALIAS     = 0x1000,
    HAS_PPC_ALIAS       = 0x2000,
    PROCEDURE           = 0x4000,
} status_bits;

typedef struct label_def {
    label_handle            link;
    pointer                 patch;
    cg_sym_handle           sym;
    offset                  address;
    status_bits             status;
} label_def;

typedef struct code_lbl {
#ifndef NDEBUG
    use_info                useinfo;
#endif
    struct label_def        lbl;
    struct code_lbl         *alias;
    struct ins_entry        *ins;
    struct ins_entry        *refs;
#if  OPTIONS & SHORT_JUMPS
    struct code_lbl         *redirect;
#endif
#if _TARGET & _TARG_RISC
    struct code_lbl         *ppc_alt_name;
    void                    *owl_symbol;
#endif
} code_lbl;

#define _SetStatus( var, stat ) var->lbl.status |= (stat)
#define _ClrStatus( var, stat ) var->lbl.status &= ~(stat)
#define _TstStatus( var, stat ) ((var->lbl.status & (stat)) != 0)

#ifndef PRODUCTION
#define _ValidLbl( lbl ) ValidLbl( lbl )
#else
#define _ValidLbl( lbl )
#endif

extern bool             ValidLbl( label_handle lbl );

extern label_handle     AskForNewLabel( void );
extern label_handle     AskForRTLabel( rt_class rtindex );
extern label_handle     AskForLabel( cg_sym_handle sym );
extern bool             AskIfReachedLabel( label_handle lbl );
extern bool             AskIfRTLabel( label_handle lbl );
extern bool             AskIfUniqueLabel( label_handle lbl );
extern bool             AskIfCommonLabel( label_handle lbl );
extern offset           AskAddress( label_handle lbl );
extern pointer          AskLblPatch( label_handle lbl );
extern cg_sym_handle    AskForLblSym( label_handle lbl );

extern void             AddLblDef( ins_entry *instr );
extern void             DelLblDef( ins_entry *instr );
extern void             AddLblRef( ins_entry *instr );
extern void             DelLblRef( ins_entry *instr );
extern void             ChgLblRef( ins_entry *instr, label_handle new );
extern bool             UniqueLabel( label_handle lbl );
extern ins_entry        *AliasLabels( ins_entry *oldlbl, ins_entry *newlbl );
extern void             TryScrapLabel( label_handle old );
extern void             ScrapCodeLabel( label_handle lbl );
