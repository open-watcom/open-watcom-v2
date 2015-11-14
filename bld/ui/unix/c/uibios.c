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
* Description:  BIOS emulation routines for UNIX platforms.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <curses.h>
#include <term.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "uidef.h"
#include "uivirt.h"
#include "unxuiext.h"
#include "ctkeyb.h"
#include "qdebug.h"

extern PossibleDisplay  DisplayList[];

static const char       *UITermType = NULL; /* global so that the debugger can get at it */

bool UIAPI uiset80col( void )
{
    return( TRUE );
}

unsigned UIAPI uiclockdelay( unsigned milli )
{
    /* this routine converts milli-seconds into platform  */
    /* dependant units - used to set mouse & timer delays */
    return( milli );
}

const char *GetTermType( void )
{
    if( UITermType == NULL ) {
        UITermType = getenv( "TERM" );
        if( UITermType == NULL ) {
            UITermType = "";
        }
    }
    return( UITermType );
}

bool intern initbios( void )
{
    PossibleDisplay             *curr;

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

    {
        const char  *p1;
        char        *p2;

        p1 = GetTermType();
        p2 = malloc( strlen( p1 ) + 1 );
        strcpy( p2, p1 );
        setupterm( p2, UIConHandle, NULL );
        free( p2 );
    }
    /* will report an error message and exit if any
       problem with a terminfo */

    // Check to make sure terminal is suitable
    if( cursor_address == NULL || hard_copy ) {
        del_curterm( cur_term );
        return( false );
    }

    curr = DisplayList;

    for( ;; ) {
        if( curr->check == NULL )
            return( false );
        if( curr->check() )
            break;
        ++curr;
    }
    UIVirt = curr->virt;
    return( _uibiosinit() );
}

void intern finibios( void )
{
    _uibiosfini();
    del_curterm( cur_term );
}

static unsigned RefreshForbid = 0;

void forbid_refresh( void )
{
    RefreshForbid++;
}

void permit_refresh( void )
{
    if( RefreshForbid ){
        RefreshForbid--;
    }
    if( !RefreshForbid ){
        _ui_refresh(0);
    }
}

void intern physupdate( SAREA *area )
{
    _physupdate(area);
    if( !RefreshForbid ){
        _ui_refresh(0);
    }
}

#if defined( UI_DEBUG )
#include <stdio.h>
#include <stdarg.h>
void UIDebugPrintf( const char *f, ... )
{
    static FILE     *file = 0;
    va_list         vargs;

    if (!file) {
        if( (file = fopen( "UI.Debug", "w" )) == 0 ) {
            return;
        }
    }
    va_start( vargs, f );
    vfprintf( file, f, vargs );
    putc( '\n', file );
    fflush( file );
}
#endif
