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
// CLE       : compile, link, and execute a FORTRAN program
//

#include "ftnstd.h"
#include "global.h"
#include "progsw.h"
#include "cpopt.h"
#include "comio.h"
#include "inout.h"
#include "cle.h"
#include "utility.h"

#include <time.h>

extern  void            DoCompile( void );
extern  void            StatProg( void );
extern  void            VSTInit( void );
extern  void            STPurge( void );
extern  void            CSPurge( void );
extern  void            EnPurge( void );
extern  void            EqPurge( void );
extern  void            TDPurge( void );
extern  void            OpenSymTab( void );
extern  void            InitMacros( void );
extern  void            FiniMacros( void );
extern  void            FiniMacroProcessor( void );
extern  void            InitAuxInfo( void );
extern  void            FiniAuxInfo( void );
extern  void            InitGlobalSegs( void );
extern  void            FreeGlobalSegs( void );
extern  void            SDRewind(file_handle);
extern  void            TOutBanner( void );
extern  void            PrtOptions(void);

unsigned_32     CompTime;

static  void StartCompile( void )
{
    OpenLst();
    if( !(Options & OPT_QUIET) && !(Options & OPT_TYPE) ) {
        TOutBanner();
    }
    PrtBanner();
    PrtOptions();
}

static void Compile( void )
{
    InitGlobalSegs();
    ProgSw |= PS_DONT_GENERATE;
    InitAuxInfo();      // must be done before ComRead()
    InvokeCompile();
    if( ( ( Options & OPT_SYNTAX ) == 0 ) && // syntax check only
        ( ( CurrFile != NULL ) ) &&          // not an "null" file
        ( ( ProgSw & PS_ERROR ) == 0 ) ) {   // no error during first pass
        CurrFile->flags &= ~CONC_PENDING;
        CurrFile->rec = 0;
        SrcRecNum = 0;
        ProgSw = 0;
        SDRewind( CurrFile->fileptr );
        InvokeCompile();
    }
    FiniAuxInfo();
    FreeGlobalSegs();
}

void InvokeCompile( void )
{
    InitMacros();
    ComRead(); // pre-read must occur here in case of null program
    if( ProgSw & PS_SOURCE_EOF ) {
        Conclude();
    } else {
        DoCompile();
    }
    FiniMacros();
}


static void FiniCompile( void )
{
    SetLst( TRUE ); // listing file on for statistics
    LFEndSrc();
    CloseErr();
}


static void Conclusion( void )
{
    StatProg();
    if( ProgSw & PS_ERROR ) {
        PurgeAll();
    }
    CloseLst();
}

void PurgeAll( void )
{
    STPurge();
    ITPurge();
    CSPurge();
    IOPurge();
    EnPurge();
    EqPurge();
    TDPurge();
    FiniMacroProcessor();
}

static  void    InitPurge( void )
{
// Initialize variables for purge routines in case the purge routines
// get called and the corresponding variables don't get initialized.

    // Initialize for STPurge();
    OpenSymTab();
    VSTInit();
    // Initialize for ITPurge();
    ITHead = NULL;
    ITPool = NULL;
    // Initialize for CSPurge();
    CSHead = NULL;
    // Initialize for IOPurge();
    CurrFile = NULL;
    // Initialize for EnPurge();
    Entries = NULL;
    // Initialize for EqPurge();
    EquivSets = NULL;
}

void CLE( void )
{
    time_t      start;

    InitPurge();
    OpenSrc();
    if( CurrFile != NULL ) {
        StartCompile();
        start = time( NULL );
        Compile();
        CompTime = difftime( time( NULL ), start );
        FiniCompile();
        Conclusion();
    } else {
        // Consider: wfc /who what
        CloseErr();
    }
}
