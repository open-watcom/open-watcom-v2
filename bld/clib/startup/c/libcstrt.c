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
* Description:  This file provides the hooks that NetWare's LibC
*               requires to initialize and deinitialize the runtime
*               environment.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <io.h>
#include "rtdata.h"
#include "rterrno.h"
#include "liballoc.h"
#include "exitwmsg.h"
#include "fileacc.h"
#include "initfini.h"
#include "thread.h"
#include "trdlist.h"
#include "nw_libc.h"
#include "snglthrd.h"
#include "mthread.h"
#include "trdlstac.h"
#include "wprelude.h"


/*****************************************************************************
//  TLS slot key
*****************************************************************************/
NXKey_t     __NXSlotID;

#ifdef __cplusplus
extern "C" {
#endif
    extern int      __CreateFirstThreadData( void );
    /*
    //  Called from LibC startup / termination code in libcpre.obj
    */
    extern int      __init_environment( void *reserved );
    extern int      __deinit_environment( void *reserved );

    /*
    //  LibC exports
    */
    char *      getnlmloadpath( char *loadpath );
    void *      getnlmhandle( void );
    char *      getnlmname( void *handle, char *name );

    /*
    //  NW386 Server exported functions
    */
    #define     AllocSignature  0x54524C41
    extern long AllocateResourceTag( void *__NLMHandle, 
                    char *__descriptionString, long __resourceType );
    extern void *Alloc( long __numberOfBytes, long __resourceTag );
    extern long SizeOfAllocBlock( void * );
    extern void Free( void *__address );

    /*
    //  module level functions
    */
    static void __NullSema4Rtn( semaphore_object *p );
#if !defined (_THIN_LIB)
    static void __NullAccessRtn( int hdl );
#endif
    static void __NullRtn( void );

    /*
    //  global library support functions
    */
    extern unsigned short __DS( void );

#ifdef __cplusplus
}
#endif

#pragma aux __DS =  \
    "mov ax, ds"    \
    value [ ax ];

/*****************************************************************************
//  Multi-thread barriers. See mthread\c\mthread.c
*****************************************************************************/
#if !defined (_THIN_LIB)
void    (*_AccessFileH)( int )      =   &__NullAccessRtn;
void    (*_ReleaseFileH)( int )     =   &__NullAccessRtn;
void    (*_AccessIOB)( void )       =   &__NullRtn;
void    (*_ReleaseIOB)( void )      =   &__NullRtn;
#endif
void    (*_AccessTDList)( void )    =   &__NullRtn;
void    (*_ReleaseTDList)( void )   =   &__NullRtn;

/*****************************************************************************
//  Module level globals
*****************************************************************************/
static unsigned short       _saved_DS = 0;      /* Selector on NetWare */
static int                  InitFiniLevel = 0;  /* Initialisation level */
static long                 AllocRTag = 0;
static void *               NLMHandle = NULL;

/*****************************************************************************
//  These are essentially NULL functions setup before initialising
//  multithreading support is enabled
*****************************************************************************/
static void __NullSema4Rtn(semaphore_object *p)
{
    p = p;
}

#if !defined (_THIN_LIB)
static void __NullAccessRtn( int hdl )
{
    hdl = hdl;
}
#endif

static void __NullRtn( void )
{
}

/*****************************************************************************
//  Restore NULL functions
*****************************************************************************/
static void __FiniMultipleThread(void)
{
    #if !defined (_THIN_LIB)
    _AccessFileH   = &__NullAccessRtn;
    _ReleaseFileH  = &__NullAccessRtn;
    _AccessIOB     = &__NullRtn;
    _ReleaseIOB    = &__NullRtn;
    #endif
    __AccessSema4  = &__NullSema4Rtn;
    __ReleaseSema4 = &__NullSema4Rtn;
    __CloseSema4   = &__NullSema4Rtn;

    /*
    //  we need to close down so get hold of __FirstThreadData when any
    //  global destuctors request per thread data
    */
    __RestoreSingleThreading();

} /* FiniMultipleThread() */

/*****************************************************************************
//  Initialise runtime environemnt. Equivalent of old prelude
*****************************************************************************/
int     __init_environment( void *  reserved )
{
    int rc = -1;

    if(NULL == (NLMHandle = getnlmhandle()))
        return -1;

    if(NULL == (AllocRTag = AllocateResourceTag( NLMHandle, "OpenWatcom CLIB Memory", AllocSignature )))
        return -1;

    _saved_DS = __DS();

    /*
    //  Call initialisation routines where priority is <= 1 and set the
    //  initialisation finish level to 1
    */
    __InitRtns( INIT_PRIORITY_THREAD );
    InitFiniLevel = INIT_PRIORITY_THREAD;
    /*
    //  Initialise multiple thread support
    */
    if(__CreateFirstThreadData())
    {
        __InitMultipleThread();
        /*
        //  Call initiliation routines at priority 255 (all) and the set the
        //  initialisation finish level to 255
        */
        __InitRtns( 255 );
        InitFiniLevel = 255;
        /*
        //  Environment initialised.
        */
        rc = 0;
    }
    return( rc );
}

/*****************************************************************************
//  free runtime environemnt.
*****************************************************************************/
int __deinit_environment( void *  reserved )
{
    /*
    //  a. clear the multiple thread handlers
    //  b. call registered library cleanup routines
    //  c. free up all remaining per thread data
    //  d. free up first thread data
    */
    __FiniMultipleThread();
    __FiniRtns( 0, InitFiniLevel);
    //__RemoveAllThreadData();

    return 0;
}

/*****************************************************************************
//  __exit should ensure that __deinit_environment is
//  called at termination.
*****************************************************************************/
_NORETURN void __exit( unsigned rc )
{
    __FiniRtns( 0, InitFiniLevel );
/*
 * Netware has own _exit procedure
 */
    _exit( rc );
    // never return
}

/*#define INTERCEPT_ALLOCATIONS */
#ifdef INTERCEPT_ALLOCATIONS
void * calloc(size_t num,size_t size )
{
    return _NW_calloc(num, size);
}
void * malloc(size_t size )
{
    return _NW_malloc(size);
}
void free(void * p)
{
    _NW_free(p);
}
char * strdup(const char *in)
{
    char * newstr = NULL;
    if(NULL != in)
    {
        size_t sz = strlen(in) + 1;
        if(NULL != (newstr = malloc(sz)))
            strcpy(newstr, in);
    }
    return newstr;
}

void *realloc(void *old, size_t size)
{
    return(_NW_realloc(old,size));
}
#endif

/*
//  These are the lib_* allocation functions as defined in
//  $(BLD)\lib_misc\liballoc.h. They use the NetWare server allocation
//  routines directly (not LibC or CLIB) as the heap will not be fully ready
//  yet.
*/

/*****************************************************************************
//  void * calloc(size_t,size_t);
*****************************************************************************/
void *_NW_calloc( size_t num,size_t size )
{
    size_t  toalloc = num * size;
    void * p = _NW_malloc(toalloc);
    if(NULL != p)
        memset(p, 0, toalloc);
    return p;
}

/*****************************************************************************
//  void * malloc(size_t);
*****************************************************************************/
void *_NW_malloc( size_t size )
{
    void * p = Alloc( size, AllocRTag);
    return p;
}

/*****************************************************************************
//  void * realloc(void *,size_t);
*****************************************************************************/
void *_NW_realloc( void *old, size_t size)
{
    void *  new_blk;
    size_t  mem_size = 0;

    if( old == NULL )
       return( _NW_malloc( size ) );

    if( size == 0 )
    {
       _NW_free( old );
       return( NULL );
    }

    if( size <= (mem_size = SizeOfAllocBlock(old)))
        return( old );

    if(NULL == (new_blk = _NW_malloc( size )))
       return( NULL );

    if( size < mem_size )
        mem_size = size;

    memcpy( new_blk, old, mem_size );
    _NW_free( old );
    return( new_blk );
}

/*****************************************************************************
//  void free(void *);
*****************************************************************************/
void _NW_free( void *chunk )
{
    Free( chunk );
}

/*****************************************************************************
//  void (size_t,size_t );
*****************************************************************************/
void SetLastError(int error)
{
    *___errno() = error;
}

/*****************************************************************************
//  Automatically inserted reference by the compiler to force this object
//  file to be linked in from the library.
*****************************************************************************/
void __WATCOM_Prelude( void )
{
}
