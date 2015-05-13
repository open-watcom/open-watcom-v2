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
* Description:  Platform independent __allocfp() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#if defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "liballoc.h"
#include "fileacc.h"
#include "streamio.h"
#include "errorno.h"
#include "thread.h"


#define KEEP_FLAGS (_READ | _WRITE | _DYNAMIC)

FILE *__allocfp( int handle )
{
    FILE                *end;
    FILE                *fp;
    __stream_link       *link;
    unsigned            flags;

    handle = handle;
    _AccessIOB();
    /* Try and take one off the recently closed list */
    link = _RWD_cstream;
    if( link != NULL ) {
        _RWD_cstream = link->next;
        fp = link->stream;
        flags = (fp->_flag & KEEP_FLAGS) | (_READ | _WRITE);
        goto got_one;
    }
    /* See if there is a static FILE structure available. */
    end = &_RWD_iob[_NFILES];
    for( fp = _RWD_iob; fp < end; ++fp ) {
        if( (fp->_flag & (_READ | _WRITE)) == 0 ) {
            link = lib_malloc( sizeof( __stream_link ) );
            if( link == NULL )
                goto no_mem;
            flags = _READ | _WRITE;
            goto got_one;
        }
    }
    /* Allocate a new dynamic structure */
    flags = _DYNAMIC | _READ | _WRITE;
    link = lib_malloc( sizeof( __stream_link ) + sizeof( FILE ) );
    if( link == NULL )
        goto no_mem;
    fp = (FILE *)(link + 1);
got_one:
    memset( fp, 0, sizeof( *fp ) );
    fp->_flag = flags;
    link->stream = fp;
    link->stream->_link = link;     /* point back to link structure */
    link->next = _RWD_ostream;
    _RWD_ostream = link;
    _ReleaseIOB();
    return( fp );
no_mem:
    _RWD_errno = ENOMEM;
    _ReleaseIOB();
    return( NULL );     /* no free slots */
}
