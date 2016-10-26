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
#include "rtdata.h"
#include "liballoc.h"
#include "fileacc.h"
#include "streamio.h"

/*
    NOTE: This routine does not actually free the link/FILE structures.
    That is because code assumes that it can fclose the file and then
    freopen is a short time later. The __purgefp routine can be called
    to actually release the storage.
*/

void __freefp( FILE * fp )
{
    __stream_link       **owner;
    __stream_link       *link;

    _AccessIOB();
    owner = &_RWD_ostream;
    for( ;; ) {
        link = *owner;
        if( link == NULL )
            return;
        if( link->stream == fp )
            break;
        owner = &link->next;
    }
    fp->_flag |= _READ | _WRITE;
    (*owner) = link->next;
    link->next = _RWD_cstream;
    _RWD_cstream = link;
    _ReleaseIOB();
}


void __purgefp( void )
{
    __stream_link       *next;

    _AccessIOB();
    while( _RWD_cstream != NULL ) {
        next = _RWD_cstream->next;
        lib_free( _RWD_cstream );
        _RWD_cstream = next;
    }
    _ReleaseIOB();
}
