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
* Description:  compile a program
*
****************************************************************************/


#include "ftnstd.h"
#include "progsw.h"
#include "segsw.h"
#include "cpopt.h"
#include "stmtsw.h"
#include "errcod.h"
#include "global.h"
#include "ferror.h"
#include "frl.h"
#include "comio.h"
#include "inout.h"
#include "arglist.h"
#include "compprog.h"
#include "compstmt.h"
#include "cpsubpgm.h"
#include "csutls.h"
#include "ifused.h"
#include "rstdump.h"
#include "stmtno.h"
#include "stresolv.h"
#include "symtab.h"
#include "cptraps.h"
#include "gsubprog.h"
#include "tdinit.h"
#include "rstmgr.h"

extern  void    BIInit( void );
extern  void    BIEnd( void );
extern  void    BIStartSubProg( void );
extern  void    BIResolveUndefTypes( void );
extern  void    BIEndSubProg( void );
extern  bool    TBreak( void );


static bool CompSProg( void )
{
    bool        fini_subprog;

    InitSubProg();
    fini_subprog = FALSE;
    for(;;) {
        for(;;) {
            if( CurrFile->flags & INC_PENDING ) {
                CurrFile->flags &= ~INC_PENDING;
                ProcInclude();
                ComRead();
            } else if( CurrFile->flags & CONC_PENDING ) {
                if( ( ProgSw & PS_DONT_GENERATE ) &&
                    ( ( Options & OPT_SYNTAX ) == 0 ) &&
                    ( ( ProgSw & PS_ERROR ) == 0 ) &&
                    ( CurrFile->link == NULL ) ) break;
                Conclude();
                if( CurrFile == NULL ) break;
                ComRead();
            } else {
                break;
            }
        }
        Options = NewOptions;
        if( ProgSw & ( PS_SOURCE_EOF | PS_END_OF_SUBPROG ) ) break;
        CompStatement();
        if( ProgSw & ( PS_SOURCE_EOF | PS_END_OF_SUBPROG ) ) {
            // consider:        call sam
            //                  end
            //          c$include sam
            // Before we open 'sam', we must finish off the subprogram
            // so we get the correct file name in the traceback.
            FiniSubProg();
            fini_subprog = TRUE;
        }
        if( TBreak() ) return( TRUE );
    }
    if( !fini_subprog ) {
        FiniSubProg();
    }
    return( FALSE );
}


static bool CompFile( void )
{
    bool        tbreak;

    tbreak = FALSE;
    for(;;) {
        if( ProgSw & PS_SOURCE_EOF ) break;
        tbreak = CompSProg();
        if( tbreak ) break;
    }
    return( tbreak );
}


static void InitProgram( void )
{
    ExtnSw = 0;
    OpenSymTab();
    // VAX/VMS version of FTrapInit() goes back 2 stack frames to set
    // exception bits for the whole compile.
    // If the sequence of calls "CompProg()/InitProgram()/FTrapInit()"
    // changes, FTrapInit() will have to be modified.
    FTrapInit();
    ISNNumber = 0;
    ITHead = NULL;
    InitCSList();
    IFInit();
    TDProgInit();
    BIInit(); // Initialize Browser Info Generator
}


static void FiniProgram( void )
{
    FTrapFini();
    Options = NewOptions;
    TDProgFini();
    CSPurge();
    BIEnd(); // Close down the Browse generator and create the .MBR file
}



void CompProg( void )
{
    bool        tbreak;

    InitProgram();
    tbreak = CompFile();
    if( tbreak ) {
        Error( KO_INTERRUPT );
    }
    FiniProgram();
}


void InitSubProg( void )
{
    ProgSw &= ~( PS_END_OF_SUBPROG | PS_IN_SUBPROGRAM | PS_BLOCK_DATA );
    FrlInit( &ITPool );
    SgmtSw       = 0;
    ArgList      = NULL;
    Entries      = NULL;
    EquivSets    = NULL;
    SubProgId    = NULL;
    STInit();         // <-------+
    TDSubInit();      // Must come before InitStNumbers().
    InitStNumbers();
    ClearRem();
    BIStartSubProg();
}

void FiniSubProg( void )
{
    FrlFini( &ITPool );
    CheckCSList( CS_EMPTY_LIST ); // all control structures should be finished
    if( !Remember.endstmt ) {
        Error( EN_NO_END );
    }
    if( ( SgmtSw & SG_SYMTAB_RESOLVED ) == 0 ) {
        STResolve();
        SgmtSw |= SG_SYMTAB_RESOLVED;
    }
    if(( ProgSw & PS_BLOCK_DATA ) != 0 ) {
        GEndBlockData();
    } else if(( ProgSw & PS_IN_SUBPROGRAM ) != 0 ) {
        Epilogue();
    } else if( ( ProgSw & PS_ERROR ) == 0 ) {
        GReturn();
    }
    DumpEntries();
    BIResolveUndefTypes();
    STDump();
    TDSubFini();
    BIEndSubProg();
    EnPurge(); // Must come after TDSubFini() for optimizing compiler
    SubProgId = NULL;
}
