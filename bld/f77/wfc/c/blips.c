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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
// BLIPS        : "blip" processing
//

#include "ftnstd.h"
#include "cgflags.h"
#include "fcgmain.h"
#include "blips.h"


void    SendBlip( void )
//======================
// Send a blip to console.
{
    char        ch;

    ch = '.';
    fwrite( &ch, 1, 1, stdout );
    fflush( stdout );
    CGFlags |= CG_BLIPS;
}


void    CheckBlips( void )
//========================
// If "blips" have been displayed, display a CR.
{
    char        ch;

    if( CGFlags & CG_BLIPS ) {
        ch = '\r';
        fwrite( &ch, 1, 1, stdout );
        fflush( stdout );
        CGFlags &= ~CG_BLIPS;
    }
}


void    SendStd( const char *msg )
//================================
// Send message to standard output.
{
    fwrite( msg, 1, strlen( msg ), stdout );
    fflush( stdout );
}
