/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  BIOS emulation routines for UNIX platforms.
*
****************************************************************************/


#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "wterm.h"
#include "uidef.h"
#include "uivirts.h"
#include "uiextrn.h"
#include "ctkeyb.h"
#include "qdebug.h"


extern PossibleDisplay  DisplayList[];

static char     *UITermType = NULL; /* global so that the debugger can get at it */

bool UIAPI uiset80col( void )
{
    return( true );
}

char *GetTermType( void )
{
    const char  *p;
    size_t      len;

    if( UITermType == NULL ) {
        p = getenv( "TERM" );
        if( p == NULL ) {
            p = "";
        }
        len = strlen( p ) + 1;
        UITermType = uimalloc( len );
        memcpy( UITermType, p, len );
    }
    return( UITermType );
}

void SetTermType( const char *new_term )
{
    size_t      len;

    if( UITermType != NULL ) {
        uifree( UITermType );
    }
    if( new_term == NULL ) {
        new_term = "";
    }
    len = strlen( new_term ) + 1;
    UITermType = uimalloc( len );
    memcpy( UITermType, new_term, len );
}

bool intern initbios( void )
{
    PossibleDisplay     *curr;

    if( UIConFile == NULL ) {
        const char  *tty;

        tty = getenv( "TTY" );
        if( tty == NULL ) {
            tty = "/dev/tty";
        }
        UIConFile = fopen( tty, "w+" );
        if( UIConFile == NULL )
            return( false );
        UIConHandle = fileno( UIConFile );
        fcntl( UIConHandle, F_SETFD, 1 );
    }
    setupterm( GetTermType(), UIConHandle, NULL );

    /* will report an error message and exit if any
       problem with a terminfo */

    // Check to make sure terminal is suitable
    if( cursor_address == NULL || hard_copy ) {
        del_curterm( cur_term );
        return( false );
    }

    for( curr = DisplayList; curr->check != NULL; curr++ ) {
        if( curr->check() ) {
            UIVirt = curr->virt;
            return( _uibiosinit() );
        }
    }
    return( false );
}

void intern finibios( void )
{
    _uibiosfini();
    del_curterm( cur_term );
    if( UITermType != NULL ) {
        uifree( UITermType );
        UITermType = NULL;
    }
}

static unsigned RefreshForbid = 0;

void forbid_refresh( void )
{
    RefreshForbid++;
}

void permit_refresh( void )
{
    if( RefreshForbid ) {
        RefreshForbid--;
    }
    if( !RefreshForbid ) {
        _ui_refresh( false );
    }
}

void intern physupdate( SAREA *area )
{
    _physupdate( area );
    if( !RefreshForbid ) {
        _ui_refresh( false );
    }
}

#if defined( UI_DEBUG )

#include <stdio.h>
#include <stdarg.h>

void UIDebugPrintf( const char *f, ... )
{
    static FILE     *file = NULL;
    va_list         vargs;

    if( file == NULL ) {
        if( (file = fopen( "UI.Debug", "w" )) == NULL ) {
            return;
        }
    }
    va_start( vargs, f );
    vfprintf( file, f, vargs );
    putc( '\n', file );
    fflush( file );
}
#endif
