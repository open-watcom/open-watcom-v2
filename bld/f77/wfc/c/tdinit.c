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


//
// TDINIT    : target dependent initialization routines
//

#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "fcodes.h"
#include "progsw.h"
#include "stmtsw.h"
#include "cpopt.h"
#include "cgflags.h"
#include "emitobj.h"

extern  void            FreeNameList(sym_id);
extern  void            FreeLocalLists(void);
extern  void            FiniFormatList(void);
extern  void            InitFormatList(void);
extern  bool            TypeCmplx(TYPE);
extern  label_id        NextLabel(void);
extern  void            CGPurge(void);
extern  void            FiniAuxInfo(void);
extern  void            CGGenSub(void);
extern  void            DefaultLibInfo(void);
extern  void            SubAuxInit(void);
extern  void            SubAuxFini(void);

extern  cgflags_t       CGFlags;


void            TDProgInit( void ) {
//============================

// Initialize for compilation of file.

    CodeSize = 0;
    if( ProgSw & PS_DONT_GENERATE ) {
        NumSubProgs = 0;
    }
}


void            TDProgFini( void ) {
//============================

// Finish off compilation of file.

    if( ProgSw & PS_DONT_GENERATE ) {
        // at the end of pass 1
        if( ProgSw & PS_PROGRAM_DONE ) {
            CGFlags = CG_HAS_PROGRAM;
        }
        DefaultLibInfo();
    }
    if( ( Options & OPT_SYNTAX ) || ( ( ProgSw & PS_DONT_GENERATE ) == 0 ) ) {
        FreeNameList( GList );
        GList = NULL;
    }
}


void            TDSubInit( void ) {
//===========================

// Initialize compilation of a subprogram.

    InitObj();
    LabelIdx = 1;       // NULL label has value 0
    WildLabel = NULL;
    CommonEntry = NULL;
    ReturnValue = NULL;
    EPValue = NULL;
    EpilogLabel = 0;
    InitFormatList();
    SubAuxInit();
    if( ProgSw & PS_DONT_GENERATE ) {
        ++NumSubProgs; // count # of subprograms on first pass
    } else {
        CGFlags &= ~CG_MEM_LOW_ISSUED;
    }
}


void            TDSubFini( void ) {
//===========================

// Finish off compilation of a subprogram.

    SubAuxFini();
    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) {
        EmitOp( FC_END_OF_SEQUENCE );
        CGGenSub();
    }
    FiniObj();
    FiniFormatList();
    FreeLocalLists();
}


void            TDStmtInit( void ) {
//============================

// Target dependent statement initialization.

}


void            TDStmtFini( void ) {
//============================

// Target dependent statement finalization.

    if( StmtProc == PR_ASNMNT ) {
        if( TypeCmplx( ResultType ) ) {
            EmitOp( FC_CMPLX_EXPR_DONE );
        } else {
            EmitOp( FC_EXPR_DONE );
        }
    }
    if( StmtSw & SS_SF_REFERENCED ) {
        EmitOp( FC_SF_REFERENCED );
    }
    EmitOp( FC_STMT_DONE );
}


void            TDPurge( void ) {
//=========================

// Free all allocated structures.

    FiniFormatList();
    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) {
        CGPurge();
    }
    FiniObj();
    FiniAuxInfo();
}
