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
* Description:  Mainline for run-time system
*
****************************************************************************/


#include "ftnstd.h"
#include "frtdata.h"
#include "fthread.h"
#include "xfflags.h"
#include "fio.h"
#include "rundat.h"
#include "rmemmgr.h"
#include "thread.h"
#include "runmain.h"
#include "posopen.h"
#include "rtspawn.h"
#include "rtsysutl.h"
#include "rtutls.h"


static  bool    __IsUnit6CC = false;
static  bool    __AllowCommaSeparator = false;


void    __InitUnit6CC( void ) {
//=======================

    __IsUnit6CC = true;
}

bool    __DevicesCC( void ) {
//=====================

    return( __IsUnit6CC );
}

void    __InitAllowCommaSeparator( void ) {
//===================================

    __AllowCommaSeparator = true;
}

bool    __AllowCommaSep( void ) {
//=========================

    return( __AllowCommaSeparator );
}

static int _InitIO( void ) {
//==========================

    InitStd();
    Files = NULL;
    FmtBuff = NULL;
    IOCB = NULL;
    FmtBuff = RChkAlloc( SCAN_STORAGE_SIZE );
    if( FmtBuff == NULL ) return( -1 );
    IOCB = RChkAlloc( sizeof( iocb ) );
    if( IOCB == NULL ) return( -1 );
    IOCB->flags = 0;
    IOCB->set_flags = 0;
    return 0;
}

static void DoCloseFile( void ) {
//=============================

    CloseFile( Files );
}

static void CloseAllFiles( void ) {
//=================================

    while( Files != NULL ) {
        // We must spawn CloseFile() since an error can occur (i.e. a disk full)
        // on the close and if the error does occur, RTErr() will be called and
        // he will suicide to where, your guess is as good as mine!
        RTSpawn( &DoCloseFile );
        if( Files->status == STATUS_SCRATCH ) {
            Scrtched( Files );
        }
        DiscoFile( Files );
    }
}

static void _FiniEx( void ) {
//===========================

    CloseAllFiles();
    if( FmtBuff != NULL ) {
        RMemFree( FmtBuff );
    }
    if( IOCB != NULL ) {
        RMemFree( IOCB );
    }
}

bool    RunEntry( void ) {
//==================

    IOTypeRtn = &IOType;
    RTSpawn( (void(*)( void ))_InitIO );
    return( (_RWD_XcptFlags & XF_FATAL_ERROR) == 0 );
}

void    RunExit( void ) {
//=================

    _FiniEx();
}
