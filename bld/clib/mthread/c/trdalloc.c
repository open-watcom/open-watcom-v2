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
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "trdlist.h"
#include "liballoc.h"

#ifdef __NETWARE__
void                    **__ThreadIDs;
#endif

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
    #if !defined(__NT__) && !defined(__QNX__)
        thread_data_vector      *__ThreadData;
    #endif
#else
    thread_data         **__ThreadData;
#endif


#if !defined(__NT__) && !defined(__QNX__)
void *__InitThreadProcessing()
/****************************/
{
// Thread structures must be initialized to 0 so that if it's for a DLL
// _STACKLOW is 0.

    // allocate 1 more than we have to since thread 1 will
    // actually access 2nd element of __ThreadData

    __MaxThreads = __GetMaxThreads();
    __ThreadData = lib_calloc( (__MaxThreads + 1), sizeof( *__ThreadData ) );
    #ifdef __NETWARE__
        if( __ThreadData != NULL ) {
            __ThreadIDs = lib_calloc( __MaxThreads + 1, sizeof( int ) );
            if( __ThreadIDs == NULL ) {
                lib_free( __ThreadData );
            }
            return( __ThreadIDs );
        }
    #endif
    return( __ThreadData );
}
#endif


#if defined(__OS2_286__)
// only needed by 16 bit OS/2
void __SetupThreadProcessing( int i ) {
/*************************************/
    thread_data *tdata;

    tdata = calloc( 1, sizeof( thread_data ) );
    tdata->__allocated = TRUE;
    tdata->__randnext = 1;
    __ThreadData[i] = tdata;
}
#endif


void __FiniThreadProcessing()
/***************************/
{

    #ifdef __NETWARE__
        if( __ThreadIDs != NULL ) {
            lib_free( __ThreadIDs );
        }
    #endif
    #if !defined(__NT__) && !defined(__QNX__)
        if( __ThreadData != NULL ) {
            unsigned    i;
            thread_data *tdata;
            #ifdef __NETWARE__
                lib_free( __ThreadData[ 0 ].data ); /* for Netware, this is always allocated */
            #endif
            for( i = 1 ; i <= __MaxThreads; i++ ) {
                #if defined(__386__) || defined(__AXP__) || defined(__PPC__)
                    tdata = __ThreadData[i].data;
                    if( tdata != NULL ) {
                        if( __ThreadData[i].allocated_entry ) lib_free( tdata );
                    }
                #else
                    tdata = __ThreadData[i];
                    if( tdata != NULL ) {
                        if( tdata->__allocated ) lib_free( tdata );
                    }
                #endif
            }
            lib_free( __ThreadData );
        }
    #endif
    #if !defined(__NETWARE__) && (defined(__386__) || defined(__AXP__) || defined(__PPC__) )
        __FreeThreadDataList();
    #endif
}
