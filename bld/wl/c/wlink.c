/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Mainline for Open Watcom linker.
*
****************************************************************************/


#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#if defined( __WATCOMC__ )
    #include <malloc.h>
#endif
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
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
#include "load16m.h"
#include "loados2.h"
#include "loadpe.h"
#include "loadqnx.h"
#include "loadnov.h"
#include "loadelf.h"
#include "loadzdos.h"
#include "loadrdv.h"
#include "loadphar.h"
#include "symtrace.h"
#include "objnode.h"
#include "objio.h"
#include "objorl.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "toc.h"
#include "dbgall.h"
#include "objpass1.h"
#include "obj2supp.h"
#include "cmdall.h"
#include "reloc.h"
#include "salloc.h"
#include "objstrip.h"
#include "symtab.h"
#include "omfreloc.h"
#include "overlays.h"
#include "wcomdef.h"
#include "objomf.h"
#include "wlink.h"
#include "ideentry.h"


#define DEF_STACK_SIZE  _4KB

#if defined( _INT_DEBUG ) && defined( __WATCOMC__ )
/*
 *  Following symbols are specific for Open Watcom Linker generated executables
 *  and appropriate code can be used only if this program is linked by OW Linker
 */
extern char     *_edata;
extern char     *_end;
#endif

static const char   *ArgSave;

// Not sure what this is for - doesn't seem to be referenced
//extern int              __nheapblk;

static void PreAddrCalcFormatSpec( void )
/***************************************/
// format specific routines which need to be called before address calculation
{
#ifdef _OS2
    if( FmtData.type & (MK_PE | MK_WIN_VXD | MK_OS2) ) {
  #if 0
        if( (FmtData.type & MK_OS2) && (LinkState & LS_HAVE_PPC_CODE) ) {
            // Development temporarly on hold:
            // ChkOS2ElfData();
        } else {
            SetOS2SegFlags();
        }
  #else
        SetOS2SegFlags();
  #endif
    }
    if( FmtData.type & MK_PE ) {
        ChkPEData();
    }
#endif
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        FindExportedSyms();
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_LAP ) {
        CheckPharLapData();
    }
#endif
}

static void PostAddrCalcFormatSpec( void )
/****************************************/
// format specific routines which need to be called after address calculation
{
#ifdef _OS2
    if( FmtData.type & (MK_PE | MK_WIN_VXD | MK_OS2) ) {
  #if 0
        if( (FmtData.type & MK_OS2) && (LinkState & LS_HAVE_PPC_CODE) ) {
            // Development temporarly on hold:
            //PrepareOS2Elf();
        } else {
            SetOS2GroupFlags();
            ChkOS2Exports();
        }
  #else
        SetOS2GroupFlags();
        ChkOS2Exports();
  #endif
    }
    if( FmtData.type & MK_PE ) {
        AllocPETransferTable();
    }
#endif
#ifdef _ELF
    if( FmtData.type & MK_ELF ) {
        ChkElfData();
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        SetQNXSegFlags();
        SetQNXGroupFlags();
    }
#endif
}

static void ResetMisc( void )
/***************************/
/* Linker support initialization. */
{
    LinkFlags = LF_REDEFS_OK | LF_CASE_FLAG | LF_FAR_CALLS_FLAG;
    LinkState = LS_MAKE_RELOCS;
    AbsGroups = NULL;
    DataGroup = NULL;
    IDataGroup = NULL;
    MapFile = NIL_FHANDLE;
    MapFName = NULL;
    OutFiles = NULL;
    ObjLibFiles = NULL;
    LibModules = NULL;
    Groups = NULL;
    SET_ADDR_UNDEFINED( CurrLoc );
    CurrMod = NULL;
    StackSize = DEF_STACK_SIZE;
    // set case sensitivity for symbols
    ResetSym();
    SetSymCase();
}

static void DoDefaultSystem( void )
/*********************************/
/* first hint about format being 32-bit vs. 16-bit (might distinguish between
 * os/2 v1 & os/2 v2), and if that doesn't decide it, haul in the default
 * system block */
{
    if( (LinkState & LS_FMT_DECIDED) == 0 ) {
        if( LinkState & LS_FMT_SEEN_64BIT ) {
            HintFormat( MK_64BIT );
        } else if( LinkState & LS_FMT_SEEN_32BIT ) {
            HintFormat( MK_32BIT );
        } else {
            HintFormat( MK_16BIT | MK_QNX );
        }
        if( (LinkState & LS_FMT_DECIDED) == 0 ) {
            if( LinkState & LS_FMT_SPECIFIED ) {
                LnkMsg( FTL+MSG_AMBIG_FORMAT, NULL );
            }
            if( LinkState & LS_FMT_SEEN_64BIT ) {
                ExecSystem( "64bit" );
            } else if( LinkState & LS_FMT_SEEN_32BIT ) {
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
    if( LinkState & LS_FMT_SEEN_64BIT ) {
        AddLibPathsToEnd( GetEnvString( "LIBX64" ) );
    } else if( LinkState & LS_FMT_SEEN_32BIT ) {
        AddLibPathsToEnd( GetEnvString( "LIB386" ) );
    } else {
        AddLibPathsToEnd( GetEnvString( "LIB286" ) );
        /*
            If we haven't seen a 386 object file by this time, we're
            not going to.
        */
        HintFormat( MK_16BIT );
    }
    AddLibPathsToEndList( LibPath );
}

static void ResetSubSystems( void )
/*********************************/
{
    ResetPermData();
    ResetMsg();
    VirtMemInit();
    ResetMisc();
    ResetDBI();
    ResetMapIO();
    ResetCmdAll();
#ifdef _EXE
    ResetOverlaySupp();
#endif
    ResetComdef();
#ifdef _NOVELL
    ResetLoadNov();
#endif
#ifdef _OS2
    ResetLoadPE();
#endif
    ResetObj2Supp();
    ResetObjIO();
    ResetObjOMF();
    ResetObjPass1();
    ResetObjStrip();
    ResetOMFReloc();
    ResetReloc();
    ResetSymTrace();
    ResetLoadFile();
    ResetAddr();
    ResetToc();
    Root = NewSection();
}

void InitSubSystems( void )
/********************************/
{
#if defined( _INT_DEBUG ) && defined( __WATCOMC__ )
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

static void CleanSubSystems( void )
/*********************************/
{
    if( MapFile != NIL_FHANDLE ) {
        QClose( MapFile, MapFName );
        MapFile = NIL_FHANDLE;
    }
    FreeOutFiles();
    _LnkFree( MapFName );
    BurnSystemList();
    FreeList( UsrLibPath );
    CloseSpillFile();
    CleanTraces();
    FreePaths();
    FreeUndefs();
    FreeLocalImports();
    CleanLoadFile();
    CleanLinkStruct();
    FreeFormatStuff();
    FreeObjInfo();
    FreeVirtMem();
    CleanToc();
    CleanSym();
    CleanPermData();
}

void FiniSubSystems( void )
/********************************/
{
    FiniLinkStruct();
    FiniMsg();
    FiniSym();
    LnkMemFini();
}

static void SetSegments( void )
/*****************************/
// now that we know where everything is, do all the processing that has been
// postponed until now.
{
#ifdef _DOS16M
    if( FmtData.type & MK_DOS16M ) {
        MakeDos16PM();
    }
#endif
#ifdef _EXE
    OvlSetSegments();
#endif
}

static void set_signal( void )
{
    signal( SIGINT, &TrapBreak );   /* so we can clean up */
}

static void ignore_signal( void )
{
    signal( SIGINT, SIG_IGN );    /* we're going to clean up anyway */
}

static void DoLink( const char *cmdline )
/***************************************/
// cmdline is only used when we are running under watfor.
{
    set_signal();

    StartTime();
    InitEnvVars();
    DoCmdFile( cmdline );
    CheckErr();
    MapInit();
    SetupFakeModule();
    ProcObjFiles(); /* ObjPass1 */
    CheckErr();
    DoDefaultSystem();
    if( LinkState & LS_LIBRARIES_ADDED ) {
        FindLibPaths();
        LinkState |= LS_SEARCHING_LIBRARIES;
        ResolveUndefined();
        LinkState &= ~LS_SEARCHING_LIBRARIES;
        LinkState |= LS_GENERATE_LIB_LIST;
    }
    ProcLocalImports();
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
#ifdef _RDOS
    if( FmtData.type & MK_RDOS ) {
        GetRdosSegs();
    }
#endif
    CheckErr();
    InitLoadFile();
    ObjPass2();
    FiniMap();
    CheckErr();
    FiniLoadFile();
    WritePermData();
    BuildImpLib();
    FiniEnvVars();
    EndTime();

    ignore_signal();
}

static void LinkMeBaby( void )
/****************************/
{
    ResetSubSystems();
    DoLink( ArgSave );
}

void LinkMainLine( const char *cmds )
/***********************************/
{
    for( ;; ) {
        ArgSave = cmds;         // bogus way to pass args to spawn
        Spawn( &LinkMeBaby );
        CleanSubSystems();
        cmds = GetNextLink();
        if( cmds == NULL ) {
            break;
        }
    }
#if defined( __WATCOMC__ )
    _heapshrink();
#endif
}
