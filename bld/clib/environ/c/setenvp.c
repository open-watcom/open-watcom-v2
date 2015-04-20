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
* Description:  Routines to create/destroy clib copy of OS environment.
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <env.h>
#include <string.h>
#if !defined(__UNIX__)
    #include <dos.h>
#endif
#if defined(__OS2__)
    #define INCL_DOSMISC
    #include <wos2.h>
#elif defined(__NT__)
    #include <windows.h>
#elif defined( __RDOS__)
    #include <rdos.h>
#elif defined( __RDOSDEV__)
    #include <rdos.h>
    #include <rdosdev.h>
#endif
#include "liballoc.h"
#include "filestr.h"
#include "rtdata.h"
#include "_environ.h"


#if defined(__WINDOWS__)
    extern char _WCI86FAR * _WCI86FAR __pascal GetDOSEnvironment( void );
#endif

#if !defined(__NETWARE__) && !defined(__LINUX__)
static char *_free_ep;

#if defined(__REAL_MODE__) && defined(__BIG_DATA__)
static void *(_WCI86NEAR allocate)( size_t amt )
{
    void                *ptr;
    void _WCI86NEAR     *nptr;

  #if defined(__OS2_286__)
    if( _RWD_osmode == DOS_MODE ) {
        ptr = nptr = lib_nmalloc( amt );
        if( nptr == NULL ) {
            ptr = lib_malloc( amt );
        }
    } else {
        ptr = lib_malloc( amt );
    }
  #else
    ptr = nptr = lib_nmalloc( amt );
    if( nptr == NULL ) {
        ptr = lib_malloc( amt );
    }
  #endif
    return( ptr );
}
#else
#define allocate lib_malloc
#endif
#endif

void __setenvp( void )
{
#if defined(__NETWARE__)
    // no environment support
#elif defined(__LINUX__)
    char    **argep;
    int     count;

    if( _RWD_environ != NULL )
        return;
    argep = _Envptr;
    while ( *argep != NULL )
        argep++;
    count = argep - _Envptr;
    argep = lib_malloc( ( count + 1 ) * sizeof( char * ) + count * sizeof( char ) );
    memcpy( argep, _Envptr, ( count + 1 ) * sizeof( char * ) );
    _RWD_env_mask = (char *)&argep[count + 1];
    memset( _RWD_env_mask, 0, count * sizeof( char ) );
    _RWD_environ = argep;
#else
  #if defined(__WINDOWS_386__) || defined(__DOS_386__)
    char    _WCFAR *startp;
    char    _WCFAR *p;
  #else
    char    _WCI86FAR *startp;
    char    _WCI86FAR *p;
  #endif
    char    *ep;
    char    *my_env_mask;
    char    **my_environ;
    int     count;
    size_t  ep_size;
    size_t  env_size;

    /* if we are already initialized, then return */
    if( _RWD_environ != NULL ) return;           /* 10-jun-90 */
  #if defined(__WARP__)
    startp = _RWD_Envptr;
  #elif defined(__OS2_286__)
    {
        unsigned short  seg;

        DosGetEnv( (PUSHORT)&seg, (PUSHORT)&count );
        startp = MK_FP( seg, 0 );
    }
  #elif defined(__WINDOWS__)
    {
        unsigned long tmp;

        tmp = (unsigned long)GetDOSEnvironment();
        startp = MK_FP( (unsigned short)( tmp >> 16 ), (unsigned long)( tmp & 0xFFFF ) );
    }
  #elif defined(__RDOS__)
    {
        int handle;
        int size;

        handle = RdosOpenProcessEnv();
        size = RdosGetEnvSize( handle );
        startp = lib_malloc( size );
        RdosGetEnvData( handle, startp );
        RdosCloseEnv( handle );
    }
  #elif defined(__RDOSDEV__)
    {
        int sel;
        char *ptr;
        char *src;
        int size;

        sel = RdosLockSysEnv();
        ptr = RdosSelectorToPointer( sel);

        size = 0;
        src = ptr;
        for( ;; ) {
            while( *src ) {
                src++;
                size++;
            }        
            src++;
            size++;

            if( *src == 0)
                break;
        }
        size++;
        
        startp = lib_malloc( size );
        memcpy( startp, ptr, size );
        RdosUnlockSysEnv();
    }
  #elif defined( __NT__ )
    startp = _RWD_Envptr;
    if( startp == NULL ) {
        startp = "";
    }
  #elif defined( _M_I86 )
    startp = MK_FP( *(unsigned short _WCI86FAR *)( MK_FP( _RWD_psp, 0x2c ) ), 0 );
  #else
    startp = _RWD_Envptr;
  #endif
    count = 0;
    p = startp;
    while( *p ) {
        while( *++p );
        ++count;
        ++p;
    }
    ep_size = p - startp;
    if( ep_size == 0 ) {
        ep_size = 1;
    }
    ep = allocate( ep_size );
    if( ep != NULL ) {
        env_size = ( count + 1 ) * sizeof( char * ) + count * sizeof( char );
        my_environ = allocate( env_size );
        if( my_environ != NULL ) {
            _RWD_environ = my_environ;
            p = startp;
            _free_ep = ep;
            while( *p ) {
                *my_environ++ = ep;
                while( *ep++ = *p++ )
                    ;
            }
            *my_environ++ = NULL;
            _RWD_env_mask = my_env_mask = (char *)my_environ;
            for( ; count; count-- ) {
                *my_env_mask++ = 0;
            }
        } else {
            lib_free( ep );
        }
    }

  #if defined( __RDOS__ ) || defined( __RDOSDEV__ )
    lib_free( startp );
  #endif

    /*** Handle the C_FILE_INFO entry ***/
  #ifdef __USE_POSIX_HANDLE_STRINGS
    __ParsePosixHandleStr();
  #endif
#endif
}

#if !defined(__NETWARE__)

void __freeenvp( void )
{
    clearenv();
    if( _RWD_environ ) {
        lib_free( _RWD_environ );
        _RWD_environ = NULL;
    }
  #if !defined(__LINUX__)
    if( _free_ep ) {
        lib_free( _free_ep );
        _free_ep = NULL;
    }
  #endif
}

#endif
