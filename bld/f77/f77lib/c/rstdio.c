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
* Description:  initialize standard i/o
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "units.h"
#include "rundat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
BartoszP
These declarations conflicts with all other modules
file_handle is effectively void *
extern  file_handle     FStdIn;
extern  file_handle     FStdOut;
*/


static ftnfile *_GetFtnFile( int unit, int mode, void *fp, char *fname ) {
//=================================================================

    ftnfile     *fcb;

    fcb = RChkAlloc( sizeof( ftnfile ) );
    if( fcb == NULL ) return( NULL );
    memset( fcb, 0, sizeof( ftnfile ) );
    fcb->link = Files;
    Files = fcb;
    IOCB->fileinfo = fcb;
    fcb->flags |= FTN_FSEXIST;
    fcb->flags |= FTN_EXIST;
    fcb->unitid = unit;
    fcb->action = mode;
    fcb->formatted = FORMATTED_IO;
    fcb->accmode = ACCM_SEQUENTIAL;
    fcb->blanks = BLANK_NULL;
    fcb->recnum = 1;
    fcb->fileptr = fp;
    if( (unit == STANDARD_OUTPUT) && __DevicesCC() ) {
        fcb->cctrl = CC_YES;
    }
    fcb->filename = RChkAlloc( strlen( fname ) + sizeof( char ) );
    if( fcb->filename == NULL ) return( fcb );
    strcpy( fcb->filename, fname );
    GetSysIOInfo( fcb );
    fcb->buffer = RChkAlloc( fcb->bufflen + sizeof( char ) );
    return( fcb );
}


static ftnfile *_SetStd( int unit, int mode, char *term_name, file_handle fp ) {
//=======================================================================

    return( _GetFtnFile( unit, mode, fp, term_name ) );
}


ftnfile *_InitStandardInput( void ) {
//=============================

    return( _SetStd( STANDARD_INPUT, ACTION_READ, SDTermIn, FStdIn ) );
}


ftnfile *_InitStandardOutput( void ) {
//==============================

    return( _SetStd( STANDARD_OUTPUT, ACTION_WRITE, SDTermOut, FStdOut ) );
}
