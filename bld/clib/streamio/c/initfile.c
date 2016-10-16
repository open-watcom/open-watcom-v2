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
* Description:  Stream I/O initializer.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include "liballoc.h"
#include "rtdata.h"
#include "exitwmsg.h"
#include "ioctrl.h"


void __InitFiles( void )
{
    __stream_link _WCI86NEAR    *ptr;
    __stream_link               *link;
    FILE                        *fp;

    fp = _RWD_iob;
#if defined( __NETWARE__ ) || defined( __RDOS__ ) || defined( __RDOSDEV__ )
    stdout->_flag &= ~(_IONBF | _IOLBF | _IOFBF);
    stdout->_flag |= _IONBF;
#endif
    stderr->_flag &= ~(_IONBF | _IOLBF | _IOFBF);
    stderr->_flag |= _IONBF;
    for( fp = _RWD_iob; fp->_flag != 0; ++fp ) {
#ifdef __NETWARE__
        ptr = lib_malloc( sizeof( __stream_link ) );
#else
        ptr = lib_nmalloc( sizeof( __stream_link ) );
#endif
        if( ptr == NULL ) {
            link = lib_malloc( sizeof( __stream_link ) );
            if( link == NULL ) {
                __fatal_runtime_error( "Not enough memory to allocate file structures", 1 );
                // never return
            }
        } else {
            link = ptr;
        }
        link->stream = fp;
        link->next = _RWD_ostream;
        _RWD_ostream = link;
        fp->_link = link;
        fp->_link->_base = NULL;
        fp->_link->_tmpfchar = 0;
        fp->_link->_orientation = _NOT_ORIENTED;
    }
    _RWD_cstream = NULL;
}
