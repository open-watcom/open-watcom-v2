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
* Description:  RDOS select() implementation.
*
****************************************************************************/


#include "variety.h"
#include <sys/time.h>
#include "rdos.h"

_WCRTLINK int select( int __width, fd_set * __readfds, fd_set * __writefds, fd_set * __exceptfds, struct timeval * __timeout )
{
    int i;
    int Count = 0;
    int waithandle = 0;
    int timeout = 0;

    if( __timeout ) {
        timeout = __timeout->tv_usec  / 1000;
        timeout += __timeout->tv_sec * 1000; 
    }

    if( __readfds || __writefds || __exceptfds ) {
        waithandle = RdosCreateWait();
    }

    if( __readfds )
        for( i = 0; i < __width; i++ )
            if( FD_ISSET( i, __readfds ) )
                RdosAddWaitForHandleRead( waithandle, i, (void *)0xFFFFFFFF );

    if( __writefds )
        for( i = 0; i < __width; i++ )
            if( FD_ISSET( i, __writefds ) )
                RdosAddWaitForHandleWrite( waithandle, i, (void *)0xFFFFFFFF );

    if( __exceptfds )
        for( i = 0; i < __width; i++ )
            if( FD_ISSET( i, __exceptfds ) )
                RdosAddWaitForHandleException( waithandle, i, (void *)0xFFFFFFFF );

    if( waithandle ) {
        if( timeout )
            RdosWaitTimeout( waithandle, timeout );
        else
            RdosWaitForever( waithandle );
    } else {
        if( timeout == 0 )
            timeout = 0x7FFFFFFF;
        RdosWaitMilli( timeout );
    }

    if( __readfds ) {
        for( i = 0; i < __width; i++ ) {
            if( FD_ISSET( i, __readfds ) ) {
                if( RdosGetHandleReadBufferCount( i ) )
                    Count++;
                else
                    FD_CLR( i, __readfds );
            }
        }
    }

    if( __writefds ) {
        for( i = 0; i < __width; i++ ) {
            if( FD_ISSET( i, __writefds ) ) {
                if( RdosGetHandleWriteBufferSpace( i ) )
                    Count++;
                else
                    FD_CLR( i, __writefds );
            }
        }
    }

    if( __exceptfds ) {
        for( i = 0; i < __width; i++ ) {
            if( FD_ISSET( i, __exceptfds ) ) {
                if( RdosHasHandleException( i ) )
                    Count++;
                else
                    FD_CLR( i, __exceptfds );
            }
        }
    }

    if( waithandle )
        RdosCloseWait( waithandle );

    return( Count );
}
