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


#include "variety.h"
#include <sys/kernel.h>
#include <i86.h>

#if defined(__386__)
#define _CONST32 const
#else
#define _CONST32
#endif

_WCRTLINK int (__send)( pid, msg1, msg2, nbytes1, nbytes2 )
pid_t                   pid;
_CONST32 void _WCI86FAR *msg1;
void _WCI86FAR          *msg2;
unsigned                nbytes1;
unsigned                nbytes2;
{
    struct _mxfer_entry xmsg1;
    struct _mxfer_entry xmsg2;

    _setmx(&xmsg1, msg1, nbytes1);
    _setmx(&xmsg2, msg2, nbytes2);

    return( __sendmx( pid, 1, 1, &xmsg1, &xmsg2 ) );
}

_WCRTLINK int (__sendfd)( fd, msg1, msg2, nbytes1, nbytes2 )
unsigned                fd;
_CONST32 void _WCI86FAR *msg1;
void _WCI86FAR          *msg2;
unsigned                nbytes1;
unsigned                nbytes2;
{
    struct _mxfer_entry xmsg1;
    struct _mxfer_entry xmsg2;

    _setmx(&xmsg1, msg1, nbytes1);
    _setmx(&xmsg2, msg2, nbytes2);

    return( __sendfdmx( fd, 1, 1, &xmsg1, &xmsg2 ) );
}

_WCRTLINK pid_t (__receive)( pid, msg, nbytes )
pid_t           pid;
void _WCI86FAR  *msg;
unsigned        nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __receivemx( pid, 1, &xmsg ) );
}

_WCRTLINK int (__reply)( pid, msg, nbytes )
pid_t                   pid;
_CONST32 void _WCI86FAR *msg;
unsigned                nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __replymx( pid, 1, &xmsg ) );
}

_WCRTLINK pid_t (__creceive)( pid, msg, nbytes )
pid_t           pid;
void _WCI86FAR  *msg;
unsigned        nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __creceivemx( pid, 1, &xmsg ) );
}

_WCRTLINK unsigned (__readmsg)( pid, offset, msg, nbytes )
pid_t           pid;
unsigned        offset;
void _WCI86FAR  *msg;
unsigned        nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __readmsgmx( pid, offset, 1, &xmsg ) );
}

_WCRTLINK unsigned (__writemsg)( pid, offset, msg, nbytes )
pid_t                   pid;
unsigned                offset;
_CONST32 void _WCI86FAR *msg;
unsigned                nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __writemsgmx( pid, offset, 1, &xmsg ) );
}
