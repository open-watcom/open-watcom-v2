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
 *  CMDQNX : command line parsing for the QNX load file format.
*/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "cmdline.h"
#include "msg.h"
#include "objfree.h"
#include "exeqnx.h"
#include "loadqnx.h"
#include "wlnkmsg.h"
#include "cmdqnx.h"
#include "cmdos2.h"     // for ChkBase


bool ProcQNX( void )
/*************************/
{
    if( !ProcOne( QNXFormats, SEP_NO, FALSE ) ) {
        HintFormat( MK_QNX_16 );        // set to 16-bit qnx mode
    }
    return( TRUE );
}

bool ProcQNXFlat( void )
/*****************************/
{
    return( TRUE );
}

void SetQNXFmt( void )
/***************************/
{
    Extension = E_QNX;
    FmtData.u.qnx.flags = 0;
    FmtData.u.qnx.priv_level = 0;
    FmtData.u.qnx.seg_flags = NULL;
    FmtData.u.qnx.heapsize = 4096;
    FmtData.u.qnx.gen_seg_relocs = TRUE;
    FmtData.u.qnx.gen_linear_relocs = FALSE;
    ChkBase(4*1024);
}

void FreeQNXFmt( void )
/****************************/
{
    FreeSegFlags( (seg_flags *)FmtData.u.qnx.seg_flags );
}

void CmdQNXFini( void )
/****************************/
{
    if( !FmtData.u.qnx.gen_linear_relocs && !FmtData.u.qnx.gen_seg_relocs ) {
        LinkState &= ~MAKE_RELOCS;
    }
}

static bool GetQNXSegFlags( void )
/********************************/
{
    bool            isclass;
    qnx_seg_flags * entry;

    Token.thumb = REJECT;
    isclass = ProcOne( QNXSegDesc, SEP_NO, FALSE );
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ){
        return( FALSE );
    }
    _ChkAlloc( entry, sizeof( qnx_seg_flags ) );
    entry->flags = 0;
    entry->name = tostring();
    entry->isclass = isclass;
    entry->next = FmtData.u.qnx.seg_flags;
    FmtData.u.qnx.seg_flags = entry;
    return( ProcOne( QNXSegModel, SEP_NO, FALSE ) );
}

bool ProcQNXSegment( void )
/********************************/
{
    return( ProcArgList( GetQNXSegFlags, TOK_INCLUDE_DOT ) );
}

bool ProcQNXClass( void )
/******************************/
// All processing done for this already.
{
    return( TRUE );
}

bool ProcQNXExecuteonly( void )
/************************************/
{
    FmtData.u.qnx.seg_flags->flags = QNX_EXEC_ONLY;
    return( TRUE );
}

bool ProcQNXExecuteread( void )
/************************************/
{
    FmtData.u.qnx.seg_flags->flags = QNX_EXEC_READ;
    return( TRUE );
}

bool ProcQNXReadOnly( void )
/*********************************/
{
    FmtData.u.qnx.seg_flags->flags = QNX_READ_ONLY;
    return( TRUE );
}

bool ProcQNXReadWrite( void )
/**********************************/
{
    FmtData.u.qnx.seg_flags->flags = QNX_READ_WRITE;
    return( TRUE );
}

bool ProcLongLived( void )
/********************************/
{
    FmtData.u.qnx.flags |= _TCF_LONG_LIVED;
    return( TRUE );
}

bool ProcQNXNoRelocs( void )
/*********************************/
{
    FmtData.u.qnx.gen_seg_relocs = FALSE;
    return( TRUE );
}

bool ProcLinearRelocs( void )
/*********************************/
{
    FmtData.u.qnx.gen_linear_relocs = TRUE;
    return( TRUE );
}

bool ProcQNXHeapSize( void )
/*********************************/
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals( TOK_NORMAL ) ) return( FALSE );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "heapsize" );
    } else {
        FmtData.u.qnx.heapsize = value;
    }
    return( TRUE );
}

bool ProcQNXPrivilege( void )
/**********************************/
{
    ord_state           ret;
    unsigned_16         value;

    if( !HaveEquals( TOK_NORMAL ) ) return( FALSE );
    ret = getatoi( &value );
    if( ret != ST_IS_ORDINAL || value > 3 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "privilege" );
    } else {
        FmtData.u.qnx.priv_level = value;
    }
    return( TRUE );
}
