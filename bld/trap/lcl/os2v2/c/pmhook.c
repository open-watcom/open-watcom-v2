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
* Description:  Message queue hook for PM app debugging.
*
****************************************************************************/


#define INCL_BASE
#define INCL_WIN
#define INCL_GPI
#define INCL_WINHOOKS
#include <os2.h>
#include "pmhook.h"

typedef struct {
    HMQ         hmq;
    HWND        hwnd;
} hmq_redirect;

#define NUM_REDIRS      100

static hmq_redirect *FindHmqRedirect( HMQ hmq )
{
    int                 i;
    static hmq_redirect redir[NUM_REDIRS];

    for( i = 0; i < NUM_REDIRS; ++i ) {
        if( redir[i].hmq == hmq )
            return( &redir[i] );
    }
    return( NULL );
}

static hmq_redirect *NewHmqRedirect( HMQ hmq )
{
    hmq_redirect        *redir;

    redir = FindHmqRedirect( NULLHANDLE );
    if( redir == NULL )
        return( NULL );
    redir->hmq = hmq;
    return( redir );
}

static void FreeHmqRedirect( hmq_redirect *redir )
{
    redir->hmq = NULLHANDLE;
}

VOID EXPENTRY SendMsgHookProc( HAB hab, PSMHSTRUCT smh, BOOL it )
{
    hmq_redirect *redir;
    HMQ         hmq;

    it=it; hab=hab;
    hmq = (HMQ)WinQueryWindowULong( smh->hwnd, QWL_HMQ );
    if( hmq == NULLHANDLE )
        return;
    redir = FindHmqRedirect( hmq );
    if( redir == NULL )
        return;
    smh->hwnd = redir->hwnd;
}

VOID EXPENTRY SetHmqDebugee( HMQ hmq, HWND hwnd )
{
    hmq_redirect *redir;

    redir = FindHmqRedirect( hmq );
    if( redir == NULL ) {
        redir = NewHmqRedirect( hmq );
        if( redir == NULL )
            return;
    }
    redir->hwnd = hwnd;
    if( hwnd == NULLHANDLE ) {
        FreeHmqRedirect( redir );
    }
}
