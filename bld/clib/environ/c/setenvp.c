/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#if defined( _M_IX86 )
    #include <i86.h>
#endif
#if defined(__OS2__)
    #define INCL_DOSMISC
    #include <wos2.h>
#elif defined( __NT__ )
    #include <windows.h>
#elif defined( __WINDOWS__ )
    #include <windows.h>
#elif defined( __RDOS__ )
    #include <rdos.h>
#elif defined( __RDOSDEV__ )
    #include <rdos.h>
    #include <rdosdev.h>
#endif
#include "liballoc.h"
#include "filestr.h"
#include "rtdata.h"
#include "_environ.h"


#if !defined(__NETWARE__) && !defined(__LINUX__)
static char *_free_ep;

#if defined(__REAL_MODE__) && defined(__BIG_DATA__)
static void *(_WCI86NEAR allocate)( size_t amt )
{
    void                *ptr;
    void _WCI86NEAR     *nptr;

  #if defined(__OS2_286__)
    if( _osmode_REALMODE() ) {
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
    while( *argep != NULL )
        argep++;
    count = argep - _Envptr;
    _RWD_environ = lib_malloc( ENVARR_SIZE( count ) );
    if( _RWD_environ != NULL ) {
        _RWD_env_mask = (char *)&_RWD_environ[count + 1];
        memcpy( _RWD_environ, _Envptr, ( count + 1 ) * sizeof( char * ) );
        memset( _RWD_env_mask, 0, count );
    }
#else
  #if defined(__WINDOWS_386__) || defined(__DOS_386__)
    char    _WCFAR *startp;
    char    _WCFAR *p;
  #elif defined(__RDOS__) || defined(__RDOSDEV__)
    char    *startp;
    char    *p;
  #else
    char    _WCI86FAR *startp;
    char    _WCI86FAR *p;
  #endif
    char    *ep;
    char    **my_environ;
    int     count;
    size_t  ep_size;

    /* if we are already initialized, then return */
    if( _RWD_environ != NULL )
        return;
  #if defined(__OS2__)
    #if defined(_M_I86)

    {
        unsigned short  seg;

        DosGetEnv( (PUSHORT)&seg, (PUSHORT)&count );
        startp = _MK_FP( seg, 0 );
    }

    #else

    startp = _Envptr;

    #endif
  #elif defined(__WINDOWS__)
    {
        unsigned long tmp;

        tmp = (unsigned long)GetDOSEnvironment();
        startp = _MK_FP( (unsigned short)( tmp >> 16 ), (unsigned long)( tmp & 0xFFFF ) );
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
            while( *src != '\0' ) {
                src++;
                size++;
            }
            src++;
            size++;

            if( *src == '\0' ) {
                break;
            }
        }
        size++;

        startp = lib_malloc( size );
        memcpy( startp, ptr, size );
        RdosUnlockSysEnv();
    }
  #elif defined( __NT__ )
    startp = _Envptr;
    if( startp == NULL ) {
        startp = "";
    }
  #elif defined( _M_I86 )
    startp = _MK_FP( *(unsigned short _WCI86FAR *)( _MK_FP( _RWD_psp, 0x2c ) ), 0 );
  #else
    startp = _Envptr;
  #endif
    count = 0;
    p = startp;
    while( *p != '\0' ) {
        while( *++p != '\0' )
            ;
        ++count;
        ++p;
    }
    ep_size = p - startp;
    if( ep_size == 0 ) {
        ep_size = 1;
    }
    ep = allocate( ep_size );
    if( ep != NULL ) {
        my_environ = allocate( ENVARR_SIZE( count ) );
        if( my_environ != NULL ) {
            _RWD_environ = my_environ;
            p = startp;
            _free_ep = ep;
            while( *p != '\0' ) {
                *my_environ++ = ep;
                while( (*ep++ = *p++) != '\0' ) {
                    ;
                }
            }
            *my_environ++ = NULL;
            _RWD_env_mask = (char *)my_environ;
            memset( _RWD_env_mask, 0, count );
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
    if( _RWD_environ != NULL ) {
        lib_free( _RWD_environ );
        _RWD_environ = NULL;
    }
  #if defined( CLIB_USE_OTHER_ENV )
    if( _RWD_wenviron != NULL ) {
        lib_free( _RWD_wenviron );
        _RWD_wenviron = NULL;
    }
  #endif
  #if !defined(__LINUX__)
    if( _free_ep != NULL ) {
        lib_free( _free_ep );
        _free_ep = NULL;
    }
  #endif
}

#endif
