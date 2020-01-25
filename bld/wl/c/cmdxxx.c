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
* Description:  Implentation of functions which process multiple targets
*
****************************************************************************/


#include "linkstd.h"
#include "cmdutils.h"
#include "cmdall.h"
#include "cmdline.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "cmd16m.h"
#include "exe16m.h"
#include "load16m.h"
#include "cmdos2.h"
#include "cmdelf.h"
#include "cmdnov.h"
#include "cmdqnx.h"
#include "cmdxxx.h"


#if defined( _OS2 ) || defined( _NOVELL ) || defined( _ELF )
bool ProcImport( void )
/*********************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2Import() );
    }
#endif
#ifdef _ELF
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFImport() );
    }
#endif
#ifdef _NOVELL
    if( HintFormat( MK_NOVELL ) ) {
        return( ProcNovImport() );
    }
#endif
    return( false );
}

bool ProcExport( void )
/*********************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE | MK_WIN_VXD ) ) {
        return( ProcOS2Export() );
    }
#endif
#ifdef _ELF
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFExport() );
    }
#endif
#ifdef _NOVELL
    if( HintFormat( MK_NOVELL ) ) {
        return( ProcNovExport() );
    }
#endif
    return( false );
}
#endif

#if defined( _DOS16M ) || defined( _QNX ) || defined( _OS2 ) || defined( _ELF )
bool ProcNoRelocs( void )
/******************************/
{
#ifdef _QNX
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXNoRelocs() );
    }
#endif
#ifdef _OS2
    if( HintFormat( MK_PE ) ) {
        return( ProcPENoRelocs() );
    }
#endif
#ifdef _DOS16M
    if( HintFormat( MK_DOS16M ) ) {
        return( Proc16MNoRelocs() );
    }
#endif
#ifdef _ELF
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFNoRelocs() );
    }
#endif
    return( true );
}
#endif

#if defined( _OS2 ) || defined( _QNX )
bool ProcSegment( void )
/**********************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE | MK_WIN_VXD ) ) {
        return( ProcOS2Segment() );
    }
#endif
#ifdef _QNX
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXSegment() );
    }
#endif
    return( true );
}
#endif

#if defined( _OS2 ) || defined( _ELF )
bool ProcAlignment( void )
/************************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2_16BIT | MK_OS2_LX | MK_PE ) ) {
        return( ProcOS2Alignment() );
    }
#endif
#ifdef _ELF
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFAlignment() );
    }
#endif
    return( true );
}
#endif

#if defined( _OS2 ) || defined( _QNX )
bool ProcHeapSize( void )
/******************************/
{
#if defined( _QNX ) && defined( __QNX__ )
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2HeapSize() );
    }
#endif
#if defined( _QNX ) && !defined( __QNX__ )
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
    return( true );
}
#endif

#if defined( _PHARLAP ) || defined( _QNX ) || defined( _OS2 ) || defined( _ELF ) || defined( _RAW )
bool ProcOffset( void )
/*********************/
{
    if( !GetLong( &FmtData.base ) ) {
        return( false );
    }
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_LAP ) {
        ChkBase( _4KB );
        return( true );
    }
#endif
#ifdef _QNX
    if( FmtData.type & MK_QNX_FLAT ) {
        ChkBase( _4KB );
        return( true );
    }
#endif
#ifdef _RAW
    if( FmtData.type & MK_RAW ) {
        ChkBase( _4KB );
        return( true );
    }
#endif
//#ifdef _OS2
//    if( FmtData.type & (MK_OS2 | MK_PE) ) {
//        ChkBase( _64KB );
//        return( true );
//    }
//#endif
//#ifdef _ELF
//    if( FmtData.type & MK_ELF ) {
//        ChkBase( _4KB );
//        return( true );
//    }
//#endif
    ChkBase( _64KB );
    return( true );
}
#endif

#if defined( _ELF ) || defined( _NOVELL )
bool ProcModule( void )
/*********************/
{
#ifdef _ELF
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFModule() );
    }
#endif
#ifdef _NOVELL
    if( HintFormat( MK_NOVELL ) ) {
        return( ProcNovModule() );
    }
#endif
    return( false );
}
#endif
