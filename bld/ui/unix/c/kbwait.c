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
* Description:  wait for a keyboard or mouse event
*
****************************************************************************/


#include "ctkeyb.h"
extern int UIMouseHandle;

int kb_wait( int secs, int usecs )
/********************************/
{
    int                 numfds;   // Number of file descriptors
    fd_set              readfds;
    struct timeval      timeout;
    int                 res;

    timeout.tv_sec      = secs;
    timeout.tv_usec     = usecs;
    FD_ZERO( &readfds );
    FD_SET( UIConHandle, &readfds );

    numfds = UIConHandle;
    if( UIMouseHandle != -1 ) {
        if( UIMouseHandle > numfds ) numfds = UIMouseHandle;
        FD_SET( UIMouseHandle, &readfds );
    }
    numfds++;
    res = select( numfds, &readfds, NULL, NULL, &timeout );
    if( res <= 0 ) return res;
    if( FD_ISSET( UIConHandle, &readfds ) ) return 1;
    /* must be a mouse event */
    return 2;
}

