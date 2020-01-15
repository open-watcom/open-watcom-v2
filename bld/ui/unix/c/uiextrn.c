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


#include <sys/types.h>
#include "uidef.h"
#include "uiintern.h"
#include "uiextrn.h"
#include "uivirts.h"


int     UIConHandle = -1;
#ifndef __QNX__
FILE    *UIConFile = NULL;

void    TermRefresh( SAREA *area )
{
    _physupdate( area );
    if( area == NULL ) {
        UserForcedTermRefresh = true;
    }
}

bool    TermKeyboardHit( void )
{
    CATTR           cattr;
    CURSOR_TYPE     ctype;
    CURSORORD       crow;
    CURSORORD       ccol;

    _uigetcursor( &crow, &ccol, &ctype, &cattr );
    _uisetcursor( crow, ccol, C_NORMAL, cattr );
    _ui_refresh( 0 );
    return( _uiwaitkeyb( 0, 0 ) != 0 );
}

void TermGetCursor( CURSORORD *crow, CURSORORD *ccol )
{
    CATTR           cattr;
    CURSOR_TYPE     ctype;

    _uigetcursor( crow, ccol, &ctype, &cattr );
}

void TermSetCursor( CURSORORD crow, CURSORORD ccol )
{
    CATTR           cattr;
    CURSOR_TYPE     ctype;
    CURSORORD       old_crow;
    CURSORORD       old_ccol;

    _uigetcursor( &old_crow, &old_ccol, &ctype, &cattr );
    _uisetcursor( crow, ccol, ctype, cattr );
}

#endif
