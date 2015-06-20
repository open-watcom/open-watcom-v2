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
* Description:  I/O streams shutdown.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include "rtdata.h"
#include "ioctrl.h"

extern void     __purgefp(void);
extern int      __shutdown_stream( FILE *fp, int );

static int docloseall( unsigned dont_close )
{
    FILE            *fp;
    int             number_of_files_closed;
    __stream_link   *link;
    __stream_link   *next;
    FILE            *bottom;
    FILE            *standards;
    int             close_handle;

    bottom = &_RWD_iob[dont_close];
    standards = &_RWD_iob[NUM_STD_STREAMS];
    number_of_files_closed = 0;
    for( link = _RWD_ostream; link != NULL; link = next ) {
        next = link->next;
        fp = link->stream;
        close_handle = 1;
        if ((fp->_flag & _DYNAMIC) || (fp->_flag & _TMPFIL))
        {
            __shutdown_stream( fp, close_handle );
            ++number_of_files_closed;
        }
        else if( fp >= bottom ) {
#ifndef __NETWARE__
            /* close the file, but leave the handle open */
            if( fp < standards ) {
                close_handle = 0;
            }
#endif
            __shutdown_stream( fp, close_handle );
            ++number_of_files_closed;
        }
    }
    return( number_of_files_closed );
}

_WCRTLINK int fcloseall( void )
{
    return( docloseall( NUM_STD_STREAMS ) );
}

void __full_io_exit( void )
{
    docloseall( 0 );
    __purgefp();
}
