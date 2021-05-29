/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "cpopt.h"
#include "global.h"
#include "blips.h"
#include "fileopen.h"
#include "fileget.h"
#include "fileput.h"
#include "fileseek.h"
#include "fileerr.h"
#include "sdcio.h"
#include "setcc.h"


void    SDInitIO(void)
//====================
{
    InitFileIO( 0 );    // minimum buffer size
}


void    SDScratch( const char *name )
//===================================
{
    if( remove( name ) != 0 ) {
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


size_t  SDReadText( file_handle fp, char *buff, size_t len )
//==========================================================
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


void    SDWriteTextNL( file_handle fp, const char *buff, size_t len )
//===================================================================
{
    if( fp == FStdOut ) {
        CheckBlips();
    }
    FPutRecFixed( fp, buff, len );
    FPutRecFixed( fp, "\n", 1 );
}


void    SDWriteCCChar( file_handle fp, char asa, bool nolf )
//==========================================================
{
    const char  *cc;
    uint        cc_len;

    cc_len = FSetCC( asa, &cc, nolf );
    FPutRecFixed( fp, cc, cc_len );
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
