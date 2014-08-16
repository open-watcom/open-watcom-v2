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


#include "wio.h"
#include "global.h"
#include "yydriver.h"
#include "errors.h"
#include "depend.h"
#include "rcldstr.h"
#include "preproc.h"
#include "dbtable.h"
#include "tmpctl.h"
#include "loadstr.h"
#include "rcspawn.h"
#include "semantic.h"
#include "rcrtns.h"
#include "wresset2.h"
#include "rclayer0.h"
#include "clibext.h"
#include "rccore.h"

extern HANDLE_INFO  Instance;

WResSetRtns(RcOpen,RcClose,RcRead,RcWrite,RcSeek,RcTell,RcMemMalloc,RcMemFree);

void InitGlobs( void )
/********************/
{
    memset( &CmdLineParms, 0, sizeof( struct RCParams ) );
    memset( &CurrResFile, 0, sizeof( RcResFileID ) );
    memset( &Pass2Info, 0, sizeof( RcPass2Info ) );
    ErrorHasOccured = false;
    memset( CharSetLen, 0, sizeof( CharSetLen ) );
    memset( &Instance, 0, sizeof( HANDLE_INFO ) );
    TmpCtlInitStatics();
    Layer0InitStatics();
    SemanticInitStatics();
    SemanticInitStaticsWIN();
    SemanticInitStaticsOS2();
    ErrorInitStatics();
    SharedIOInitStatics();
    ScanInitStaticsWIN();
    ScanInitStaticsOS2();
    AutoDepInitStatics();
    DbtableInitStatics();
    LoadstrInitStatics();
    WriteInitStatics();
    PreprocVarInit();
    PPMacroVarInit();
    PP_IncludePathInit();
    ParseInitStaticsWIN();
    ParseInitStaticsOS2();
}

void FiniGlobs( void )
/********************/
{
    FiniTable();
    ScanParamShutdown();
}

static bool CreatePreprocFile( void )
{
    WResFileID  handle;
    bool        error;
    int         ch;
    char        ch1;

    error = false;
    handle = RcOpen( CmdLineParms.OutResFileName, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, PMODE_RW );
    if( handle == NIL_HANDLE ) {
        RcError( ERR_CANT_OPEN_FILE, CmdLineParms.OutResFileName, strerror( errno ) );
        error = true;
    } else {
        ch = RcIoGetChar();
        while( ch != RC_EOF ) {
            ch1 = (char)ch;
            if( RcWrite( handle, &ch1, 1 ) != 1 ) {
                RcError( ERR_WRITTING_FILE, CmdLineParms.OutResFileName, strerror( errno ) );
                error = true;
            }
            ch = RcIoGetChar();
        }
    }
    if( handle != NIL_HANDLE )
        RcClose( handle );
    return( error );
}

static bool Pass1( void )
/***********************/
{
    bool    noerror;

    noerror = RcPass1IoInit();
    if( noerror ) {
        if( !CmdLineParms.PreprocessOnly ) {
            if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
                ParseInitOS2();
                ParseOS2();
            } else {
                ParseInitWIN();
                ParseWIN();
            }
            WriteDependencyRes();
        } else {
            CreatePreprocFile();
        }
        PP_Fini();
        RcPass1IoShutdown();
        noerror = !ErrorHasOccured;
    }
    return( noerror );
}

/* Please note that this function is vital to the resource editors. Thusly
 * any changes made to Pass2 should cause the notification of the
 * resource editor dude.
 */
static bool Pass2( void )
/**********************/
{
    bool     noerror;

    noerror = RcPass2IoInit();
    if( noerror ) {
        switch( Pass2Info.OldFile.Type ) {
        case EXE_TYPE_NE_WIN:
            noerror = MergeResExeWINNE();
            break;
        case EXE_TYPE_NE_OS2:
            noerror = MergeResExeOS2NE();
            break;
        case EXE_TYPE_PE:
            noerror = MergeResExePE();
            break;
        case EXE_TYPE_LX:
            noerror = MergeResExeLX();
            break;
        default: //EXE_TYPE_UNKNOWN
            RcError( ERR_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
            noerror = false;
            break;
        }
        RcPass2IoShutdown( noerror );
    }
    return( noerror );
}

void RCmain( void )
/*****************/
{
    bool    noerror = true;

#if defined( __WATCOMC__ )
#if ( !defined( BOOTSTRAP ) || !defined( __LINUX__ ) )   // temporary fix for bug in OW 1.9 CRTL 
#if !defined( __OSI__ ) /* _grow_handles doesn't work yet */
    _grow_handles( 100 );
#endif
#endif
#endif
    if( !CmdLineParms.Pass2Only ) {
        noerror = Pass1();
    }
    if( noerror && !CmdLineParms.Pass1Only && !CmdLineParms.PreprocessOnly ) {
        noerror = Pass2();
    }
    if( !noerror ) {
        RCSuicide( 1 );
    }
}
