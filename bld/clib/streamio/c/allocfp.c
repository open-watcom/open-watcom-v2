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
#if defined(__PENPOINT__)
#include <clsmgr.h>
#include <ostypes.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "liballoc.h"
#include "fileacc.h"
#include "rtdata.h"
#include "seterrno.h"

#ifdef __PENPOINT__
#define KEEP_FLAGS (_READ|_WRITE|_DYNAMIC|_SHRMEM)
#else
#define KEEP_FLAGS (_READ|_WRITE|_DYNAMIC)
#endif

FILE *__allocfp( int handle )
{
    FILE *              end;
    FILE *              fp;
    __stream_link *     link;
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
#if defined(__PENPOINT__)
    end = &_RWD_iob[NUM_STD_STREAMS];
#else
    end = &_RWD_iob[_NFILES];
#endif
    for( fp = _RWD_iob; fp < end; ++fp ) {
        if( (fp->_flag & (_READ | _WRITE)) == 0 ) {
            link = lib_malloc( sizeof( __stream_link ) );
            if( link == NULL ) goto no_mem;
            flags = _READ | _WRITE;
            goto got_one;
        }
    }
    /* Allocate a new dynamic structure */
    flags = _DYNAMIC | _READ | _WRITE;
#if defined(__PENPOINT__)
    if( handle > NUM_STD_STREAMS &&
        stsOK == ObjectCall(msgCan, (OBJECT)handle, (P_ARGS)objCapCall)) {
        flags |= _SHRMEM;
        link = _smalloc( sizeof( __stream_link ) + sizeof( FILE ) );
    } else {
        link = lib_malloc( sizeof( __stream_link ) + sizeof( FILE ) );
    }
#else
    link = lib_malloc( sizeof( __stream_link ) + sizeof( FILE ) );
#endif
    if( link == NULL ) goto no_mem;
    fp = (FILE *)(link + 1);
got_one:
    memset( fp, 0, sizeof( *fp ) );
    fp->_flag = flags;
    link->stream = fp;
    #ifndef __NETWARE__
        link->stream->_link = link;     /* point back to link structure */
    #endif
    link->next = _RWD_ostream;
    _RWD_ostream = link;
    _ReleaseIOB();
    return( fp );
no_mem:
    __set_errno( ENOMEM );
    _ReleaseIOB();
    return( NULL );     /* no free slots */
}

/*
    NOTE: This routine does not actually free the link/FILE structures.
    That is because code assumes that it can fclose the file and then
    freopen is a short time later. The __purgefp routine can be called
    to actually release the storage.
*/

void __freefp( FILE * fp )
{
    __stream_link **    owner;
    __stream_link *     link;

    _AccessIOB();
    owner = &_RWD_ostream;
    for( ;; ) {
        link = *owner;
        if( link == NULL ) return;
        if( link->stream == fp ) break;
        owner = &link->next;
    }
    fp->_flag |= _READ | _WRITE;
    (*owner) = link->next;
    link->next = _RWD_cstream;
    _RWD_cstream = link;
    _ReleaseIOB();
}


void __purgefp()
{
    __stream_link *     next;

    _AccessIOB();
    while( _RWD_cstream != NULL ) {
        next = _RWD_cstream->next;
        lib_free( _RWD_cstream );
        _RWD_cstream = next;
    }
    _ReleaseIOB();
}
