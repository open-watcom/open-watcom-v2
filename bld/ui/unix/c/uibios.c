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


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#ifndef HP
    #include <curses.h>
    #include <termio.h>
#else
    #include <stdarg.h>
    #include <curses.h>
#endif
#include <term.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "uidef.h"
#include "uivirt.h"
#include "qnxuiext.h"
#include "qdebug.h"

extern PossibleDisplay DisplayList[];
char    *UITermType;    /* global so that the debugger can get at it */

bool global uiset80col()
{
    return( TRUE );
}

unsigned global uiclockdelay( unsigned milli )
{
    /* this routine converts milli-seconds into platform  */
    /* dependant units - used to set mouse & timer delays */
    return( milli );
}

char *GetTermType()
{
    if( UITermType == NULL ) {
        UITermType = getenv( "TERM" );
        if( UITermType == NULL ) {
            UITermType = "";
        }
    }
    return( UITermType );
}

int intern initbios()
{
    PossibleDisplay             *curr;
    int                         error;

    if( UIConFile == NULL ) {
        char *tty;

        tty = getenv( "TTY" );
        if( tty == NULL ) {
            tty = "/dev/tty";
        }
        UIConFile = fopen( tty, "w+" );
        if( UIConFile == NULL ) return( FALSE );
        UIConHandle = fileno( UIConFile );
        fcntl( UIConHandle, F_SETFD, 1 );
    }
    setupterm( GetTermType(), UIConHandle, &error );
    if( error != 1 ) return( FALSE );
    // Check to make sure terminal is suitable
    if( cursor_address == NULL || hard_copy ) {
        del_curterm( cur_term );
        return( FALSE );
    }

    curr = DisplayList;

    for( ;; ) {
        if( curr->check == NULL ) return( FALSE );
        if( curr->check() ) break;
        ++curr;
    }
    UIVirt = curr->virt;
    return( _uibiosinit() );
}

void intern finibios()
{
    _uibiosfini();
    del_curterm( cur_term );
}

static unsigned RefreshForbid= 0;

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


#if defined( QNX_DEBUG )
#include <stdio.h>
#include <stdarg.h>
void QNXDebugPrintf(const char *f, ...)
{
static FILE *file = 0;
        va_list vargs;
        if (!file) {
                if ((file=fopen("QNX.Debug","w")) == 0) {
                        return;
                }
        }
        va_start(vargs, f);
        vfprintf(file, f, vargs);
        putc('\n',file);
        fflush( file );
}
#endif
