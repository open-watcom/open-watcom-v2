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


/*
   WLINK  : mainline for WATCOM linker

*/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "command.h"
#include "fileio.h"
#include "objpass2.h"
#include "cmdline.h"
#include "loadfile.h"
#include "objfree.h"
#include "mapio.h"
#include "objcalc.h"
#include "procfile.h"
#include "spillio.h"
#include "virtmem.h"
#include "loados2.h"
#include "loadpe.h"
#include "loadqnx.h"
#include "loadnov.h"
#include "loadelf.h"
#include "symtrace.h"
#include "objnode.h"
#include "objio.h"
#include "distrib.h"
#include "objorl.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "toc.h"

extern void     ResetAddr( void );
extern void     ResetMsg( void );
extern void     ResetSym( void );
extern void     ResetDBI( void );
extern void     ResetMapIO( void );
extern void     ResetCmdAll( void );
extern void     ResetOvlSupp( void );
extern void     ResetComdef( void );
extern void     ResetDistrib( void );
extern void     ResetLoadNov( void );
extern void     ResetLoadPE( void );
extern void     ResetMisc( void );
extern void     ResetObj2Supp( void );
extern void     ResetObjIO( void );
extern void     ResetObjOMF( void );
extern void     ResetObjPass1( void );
extern void     ResetDistrib( void );
extern void     ResetObjStrip( void );
extern void     ResetOMFReloc( void );
extern void     ResetReloc( void );
extern void     ResetSymTrace( void );
extern void     ResetLoadFile( void );
extern void     ResetToc( void );

static  void    LnkInit();

extern int              __nheapblk;
extern commandflag      CmdFlags;

#if _LINKER == _WATFOR77

extern void InitLinker( void )
/****************************/
{
    InitSubSystems();
}

extern void FiniLinker( void )
/****************************/
{
    FiniSubSystems();
}

extern int RunLinker( char *cmd )
/*******************************/
{
    LinkMainLine( cmd );
    return( (LinkState & LINK_ERROR) ? 1 : 0 );
}

#elif _LINKER == _WLINK                 // it's the standalone linker

extern int main( int argc, char ** argv )
/***************************************/
{
    argc = argc;        /* to avoid a warning */
    argv = argv;
    InitSubSystems();
    LinkMainLine( NULL );
    FiniSubSystems();
    return (LinkState & LINK_ERROR) ? 1 : 0;
}

#endif

#ifdef _INT_DEBUG
extern char *   _edata;
extern char *   _end;
#endif

static char *   ArgSave;

static void LinkMeBaby( void )
/****************************/
{
    ResetSubSystems();
    DoLink( ArgSave );
}

extern void LinkMainLine( char *cmds )
/************************************/
{
    for(;;) {
        ArgSave = cmds;         // bogus way to pass args to spawn
        Spawn( &LinkMeBaby );
        CleanSubSystems();
        cmds = GetNextLink();
        if( cmds == NULL ) break;
    }
#if _LINKER == _DLLHOST
    _heapshrink();
#endif
}

extern void InitSubSystems( void )
/********************************/
{
#ifdef _INT_DEBUG
    memset( _edata, 0xA5, _end - _edata );      // don't rely on BSS == 0
#endif
    LnkMemInit();
    LnkFilesInit();
    InitMsg();
    InitNodes();
    InitTokBuff();
    InitSpillFile();
    InitSym();
    InitObjORL();
    InitCmdFile();
}

extern void ResetSubSystems( void )
/*********************************/
{
    ResetPermData();
    ResetMsg();
    VirtMemInit();
    ResetMisc();
    ResetSym();
    Root = NewSection();
    ResetDBI();
    ResetMapIO();
    ResetCmdAll();
    ResetOvlSupp();
    ResetComdef();
    ResetDistrib();
    ResetLoadNov();
    ResetLoadPE();
    ResetObj2Supp();
    ResetObjIO();
    ResetObjOMF();
    ResetObjPass1();
    ResetDistrib();
    ResetObjStrip();
    ResetOMFReloc();
    ResetReloc();
    ResetSymTrace();
    ResetLoadFile();
    ResetAddr();
    ResetToc();
}

extern void CleanSubSystems( void )
/*********************************/
{
    if( MapFile != NIL_HANDLE ) {
        QClose( MapFile, MapFName );
        MapFile = NIL_HANDLE;
    }
    FreeOutFiles();
    _LnkFree( MapFName );
    BurnSystemList();
    FreeList( LibPath );
    CloseSpillFile();
    CleanTraces();
    FreePaths();
    FreeUndefs();
    CleanLoadFile();
    CleanLinkStruct();
    FreeFormatStuff();
    FreeObjInfo();
    FreeVirtMem();
    CleanToc();
    CleanSym();
    CleanPermData();
}

extern void FiniSubSystems( void )
/********************************/
{
    FiniLinkStruct();
    FiniMsg();
    FiniSym();
    LnkMemFini();
}

extern void DoLink( char * cmdline )
/**********************************/
// cmdline is only used when we are running under watfor.
{
#ifndef __OSI__
    signal( SIGINT, &TrapBreak ); /* so we can clean up */
#endif
    StartTime();
    DoCmdFile( cmdline );
    CheckErr();
    MapInit();
    SetupFakeModule();
    ProcObjFiles(); /* ObjPass1 */
    CheckErr();
    DoDefaultSystem();
    if( LinkState & LIBRARIES_ADDED ) {
        FindLibPaths();
        LinkState |= SEARCHING_LIBRARIES;
        ResolveUndefined();
        LinkState &= ~SEARCHING_LIBRARIES;
        LinkState |= GENERATE_LIB_LIST;
    }
    DecideFormat();
    SetFormat();
    ConvertLazyRefs();
    SetSegments();
    CheckErr();
    DefBSSSyms();
    LinkFakeModule();
    PreAddrCalcFormatSpec();
    ReportUndefined();
    CheckClassOrder();
    CalcSegSizes();
    SetStkSize();
    AutoGroup();
    CalcAddresses();
    GetBSSSize();
    GetStkAddr();
    GetStartAddr();
    PostAddrCalcFormatSpec();
    CheckErr();
    InitLoadFile();
    ObjPass2();
    CheckErr();
    FiniLoadFile();
    WritePermData();
    BuildImpLib();
    EndTime();
#ifndef __OSI__
    signal( SIGINT, SIG_IGN ); /* we're going to clean up anyway */
#endif
}

static void PreAddrCalcFormatSpec( void )
/***************************************/
// format specific routines which need to be called before address calculation
{
#ifdef _OS2
    if( FmtData.type & MK_PE ) {
        ChkPEData();
    } else if( FmtData.type & MK_OS2 ) {
        if( IS_PPC_OS2) {
            // Development temporarly on hold:
            // ChkOS2ElfData();
        } else {
            ChkOS2Data();
        }
    }
#endif
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        FindExportedSyms();
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_FLAT && LinkState & HAVE_16BIT_CODE
                                    && !(CmdFlags & CF_HAVE_REALBREAK)) {
        LnkMsg( WRN+MSG_NO_REALBREAK_WITH_16BIT, NULL );
    }
#endif
}

static void PostAddrCalcFormatSpec( void )
/****************************************/
// format specific routines which need to be called after address calculation
{
#ifdef _OS2
    if( FmtData.type & MK_PE ) {
        AllocPETransferTable();
    } else if( FmtData.type & MK_ELF ) {
        ChkElfData();
    } else if( FmtData.type & MK_OS2 ) {
        if( IS_PPC_OS2) {
            // Development temporarly on hold:
            //PrepareOS2Elf();
        } else {
            ChkOS2Exports();
        }
    }
#endif
#ifdef _QNXLOAD
     else if( FmtData.type & MK_QNX ) {
        SetQNXSegFlags();
    }
#endif
}

static void ResetMisc( void )
/***************************/
/* Linker support initialization. */
{
    LinkFlags = REDEFS_OK;
    LinkState = MAKE_RELOCS;
    AbsGroups = NULL;
    DataGroup = NULL;
    IDataGroup = NULL;
    MapFile = NIL_HANDLE;
    MapFName = NULL;
    OutFiles = NULL;
    ObjLibFiles = NULL;
    LibModules = NULL;
    Groups = NULL;
    CurrLoc.seg = UNDEFINED;
    CurrLoc.off = 0;
    OvlClasses = NULL;
    OvlVectors = NULL;
    VecNum = 0;
    OvlNum = 0;
    OvlFName = NULL;
    CurrMod = NULL;
    StackSize = 0x200;
}

static void DoDefaultSystem( void )
/*********************************/
/* first hint about format being 32-bit vs. 16-bit (might distinguish between
 * os/2 v1 & os/2 v2), and if that doesn't decide it, haul in the default
 * system block */
{
    if( !(LinkState & FMT_DECIDED) ) {
        if( LinkState & FMT_SEEN_32_BIT ) {
            HintFormat( MK_386 );
        } else {
            HintFormat( MK_286 | MK_QNX );
        }
        if( !(LinkState & FMT_DECIDED) ) {
            if( LinkState & FMT_SPECIFIED ) {
                LnkMsg( FTL+MSG_AMBIG_FORMAT, NULL );
            }
            if( LinkState & FMT_SEEN_32_BIT ) {
                ExecSystem( "386" );
            } else {
                ExecSystem( "286" ); /* no 386 obj's after this */
            }
        }
    }
}

static void FindLibPaths( void )
/******************************/
{
    AddFmtLibPaths();
    if( LinkState & FMT_SEEN_32_BIT ) {
        AddEnvPaths( "LIB386" );
    } else {
        AddEnvPaths( "LIB286" );
        /*
            If we haven't seen a 386 object file by this time, we're
            not going to.
        */
        HintFormat( MK_286 );
    }
    AddEnvPaths( "LIB" );
}
