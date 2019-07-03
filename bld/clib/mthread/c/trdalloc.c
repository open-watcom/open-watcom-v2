/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Allocate and free thread data.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#if defined( __OS2__ )
    #define INCL_DOSSEMAPHORES
    #define INCL_DOSPROCESS
    #include <wos2.h>
#elif defined( __NT__ )
    #include <time.h>
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include "ntext.h"
#elif defined( __UNIX__ )
    #include <sys/types.h>
    #include <unistd.h>
  #if defined( __LINUX__ )
    #include <process.h>
  #endif
#elif defined( __RDOS__ )
    #include <rdos.h>
#elif defined( __RDOSDEV__ )
    #include <rdos.h>
    #include <rdosdev.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rtdata.h"
#include "thread.h"
#include "trdlist.h"
#include "liballoc.h"
#include "mthread.h"
#include "maxthrds.h"

#if defined( __OS2__ ) || defined( _NETWARE_CLIB )

#ifdef _NETWARE_CLIB
void                    **__ThreadIDs;
#endif

#if defined( _M_I86 )
    thread_data         **__ThreadData;
#else
    thread_data_vector  *__ThreadData;
#endif


void *__InitThreadProcessing( void )
/**********************************/
{
// Thread structures must be initialized to 0 so that if it's for a DLL
// _STACKLOW is 0.

    // allocate 1 more than we have to since thread 1 will
    // actually access 2nd element of __ThreadData

    __MaxThreads = __GetMaxThreads();
    __ThreadData = lib_calloc( __MaxThreads + 1, sizeof( *__ThreadData ) );
  #ifdef _NETWARE_CLIB
    if( __ThreadData != NULL ) {
        __ThreadIDs = lib_calloc( __MaxThreads + 1, sizeof( void * ) );
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


void __FiniThreadProcessing( void )
/*********************************/
{

#ifdef _NETWARE_CLIB
    if( __ThreadIDs != NULL ) {
        lib_free( __ThreadIDs );
    }
#endif
#if defined( __OS2__ ) || defined( _NETWARE_CLIB )
    if( __ThreadData != NULL ) {
        unsigned    i;
        thread_data *tdata;
    #ifdef __NETWARE__
        lib_free( __ThreadData[0].data ); /* for Netware, this is always allocated */
    #endif
        for( i = 1 ; i <= __MaxThreads; i++ ) {
    #if defined( _M_I86 )
            tdata = __ThreadData[i];
            if( tdata != NULL ) {
                if( tdata->__allocated ) lib_free( tdata );
            }
    #else
            tdata = __ThreadData[i].data;
            if( tdata != NULL ) {
                if( __ThreadData[i].allocated_entry ) lib_free( tdata );
            }
    #endif
        }
        lib_free( __ThreadData );
    }
#endif

#if !defined(_NETWARE_CLIB) && !defined(__RDOSDEV__) && !defined( _M_I86 )
    __FreeThreadDataList();
#endif
}
