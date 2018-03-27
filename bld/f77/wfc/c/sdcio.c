/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
#include "omodes.h"
#include "cpopt.h"
#include "global.h"
#include "posio.h"
#include "blips.h"
#include "posopen.h"
#include "posget.h"
#include "posput.h"
#include "posseek.h"
#include "posrew.h"
#include "poserr.h"
#include "sdcio.h"
#include "posdel.h"
#include "posdat.h"

#include <string.h>


static  f_attrs         Modes[] = { RDONLY,
                                    WRONLY,
                                    APPEND,
                                    RDWR };

static  f_attrs         CurrAttrs = { REC_TEXT };


void    SDInitIO(void) {
//==================

    InitStd();
    SetIOBufferSize( 0 ); // minimum buffer size
}


void    SDInitAttr(void) {
//====================

    CurrAttrs = REC_TEXT;
}


void    SDSetAttr( f_attrs attr ) {
//===================================

    CurrAttrs = attr;
}


void    SDScratch( const char *name )
//===================================
{
    Scratchf( name );
}


file_handle SDOpen( const char *name, int mode )
//==============================================
{
    return( Openf( name, Modes[ mode ] | CurrAttrs ) );
}


void    SDClose( file_handle fp )
//===============================
{
    Closef( fp );
}


size_t    SDRead( file_handle fp, void *buff, size_t len )
//========================================================
{
    return( FGetRec( fp, buff, len ) );
}


void    SDWrite( file_handle fp, const void *buff, size_t len )
//=============================================================
{
    if( fp == FStdOut ) {
        CheckBlips();
    }
    FPutRec( fp, buff, len );
}


void    SDSeek( file_handle fp, unsigned_32 rec_num, size_t rec_size )
//===================================================================
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


bool    SDError( file_handle fp, char *buff )
//===========================================
{
    int         err;

    err = Errorf( fp );
    if( err != IO_OK ) {
        strcpy( buff, ErrorMsg( fp ) );
    }
    return( err != IO_OK );
}
