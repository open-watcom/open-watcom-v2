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
* Description:  Dummy routines for single-threaded programs.
*
****************************************************************************/


#include "variety.h"
#include "stacklow.h"
#include "liballoc.h"
#include "thread.h"

thread_data *__AllocInitThreadData( thread_data *tdata )
/******************************************************/
{
    if( tdata == NULL ) {
        tdata = lib_calloc( 1, __ThreadDataSize );
        if( tdata != NULL ) {
            tdata->__allocated = 1;
            tdata->__data_size = __ThreadDataSize;
        }
    }
    __InitThreadData( tdata );
    return( tdata );
}

int __NTThreadInit( void ) { return( 1 ); }
int __NTAddThread( thread_data *t ) { t = t; return( 1 ); }
void __NTRemoveThread( int c ) { c = c; }
void __InitMultipleThread( void ) {}
