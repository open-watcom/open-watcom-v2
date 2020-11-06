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
* Description:  System dependent compiler I/O routines.
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#include "wio.h"
#include "cpopt.h"
#include "global.h"
#include "blips.h"
#include "posopen.h"
#include "posget.h"
#include "posput.h"
#include "posseek.h"
#include "fileerr.h"
#include "sdcio.h"
#include "setcc.h"


void    SDInitIO(void)
//====================
{
    InitStd();
    SetIOBufferSize( 0 ); // minimum buffer size
}


void    SDScratch( const char *name )
//===================================
{
    if( unlink( name ) != 0 ) {
        FSetSysErr( NULL );
    }
}


file_handle SDOpen( const char *name, const char *mode )
//======================================================
{
    return( Openf( name, mode, REC_FIXED | SEEK ) );
}


file_handle SDOpenText( const char *name, const char *mode )
//==========================================================
{
    return( Openf( name, mode, REC_TEXT ) );
}


void    SDClose( file_handle fp )
//===============================
{
    Closef( fp );
}


size_t  SDRead( file_handle fp, void *buff, size_t len )
//======================================================
{
    return( FGetRecFixed( fp, buff, len ) );
}


size_t  SDReadText( file_handle fp, char *buff, size_t len )
//============================================================
{
    return( FGetRecText( fp, buff, len ) );
}


void    SDWrite( file_handle fp, const void *buff, size_t len )
//=============================================================
{
    if( fp == FStdOut ) {
        CheckBlips();
    }
    FPutRecFixed( fp, buff, len );
}


void    SDWriteText( file_handle fp, const char *buff, size_t len, bool nolf )
//============================================================================
{
    if( fp == FStdOut ) {
        CheckBlips();
    }
    FPutRecText( fp, buff, len, nolf );
}


void    SDWriteCCChar( file_handle fp, char asa, bool nolf )
//========================================================
{
    const char  *cc;
    uint        cc_len;

    cc_len = FSetCC( asa, &cc, nolf );
    FPutRecFixed( fp, cc, cc_len );
}


void    SDSeekRec( file_handle fp, unsigned_32 rec_num, size_t rec_size )
//=======================================================================
{
    FSeekRec( fp, rec_num, rec_size );
}


void    SDRewind( file_handle fp )
//================================
{
    FRewind( fp );
}


bool    SDEof( file_handle fp )
//=============================
{
    return( EOFile( fp ) );
}


bool    SDError( file_handle fp, char *buff, size_t max_len )
//===========================================================
{
    if( !IOOk( fp ) ) {
        if( buff != NULL && max_len > 0 )
            ErrorMsg( fp, buff, max_len );
        return( true );
    }
    return( false );
}
