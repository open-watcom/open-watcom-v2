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
* Description:  format data emiting routines
*
****************************************************************************/

#include "ftnstd.h"
#include "fmtdef.h"
#include "fmtdat.h"

void    FEmCode( int code ) {
//===========================

// Emit a format code.

    FmtEmStruct->FEMcode( code );
}


void    FEmChar( char PGM *ch ) {
//===============================

// Emit a character.

    FmtEmStruct->FEMchar( ch );
}


void    FEmNum( int num ) {
//=========================

// Emit a specification number.

    FmtEmStruct->FEMnum( num );
}


void    FEmByte( int num ) {
//==========================

// Emit a byte of information.

    FmtEmStruct->FEMbyte( num );
}


void    R_FError( int code ) {
//============================

// Process a format error.

    FmtEmStruct->FError( code );
}


void    R_FExtension( int code ) {
//================================

// Process a format extension.

    FmtEmStruct->FExtension( code );
}
