/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  errno related CLIB internal declarations.
*
****************************************************************************/


#ifndef _SETERNO_H_INCLUDED
#define _SETERNO_H_INCLUDED

#include <errno.h>

#if defined( __NETWARE__ )
#if !defined( _THIN_LIB )
  #if defined( _NETWARE_LIBC )
    #undef errno
  #else
  #endif
    extern int                  _WCNEAR __get_errno( void );
    extern void                 _WCNEAR __set_errno( int err );
    #define lib_get_errno()     __get_errno()
    #define lib_set_errno(x)    __set_errno(x)
#endif
#elif defined( __QNX__ )
    /*
     * QNX errno is magically multithread aware
     */
    #define lib_get_errno()     errno
    #define lib_set_errno(x)    errno = (x)
#elif defined( __RDOSDEV__ )
    #undef errno
    extern _WCRTDATA int        errno;
    #define lib_get_errno()     errno
    #define lib_set_errno(x)    errno = (x)
#elif defined( __MT__ )
    #undef errno
    extern int                  _WCNEAR __get_errno( void );
    extern void                 _WCNEAR __set_errno( int err );
    #define lib_get_errno()     __get_errno()
    #define lib_set_errno(x)    __set_errno(x)
#else
    #undef errno
    extern _WCRTDATA int        errno;
    #define lib_get_errno()     errno
    #define lib_set_errno(x)    errno = (x)
#endif

/*
 * defined in startup\c\seterrno.c
 */
extern int                  _WCNEAR __set_EINVAL( void );
#define lib_set_EINVAL()    __set_EINVAL()

/*
 * defined in _dos\c\dosret.c
 */
#if defined( __DOS__ ) \
  || defined( __WINDOWS__ ) \
  || defined( __NT__ ) \
  || defined( __OS2__ )
extern int  _WCNEAR __set_errno_dos( unsigned int );
extern int  _WCNEAR __set_errno_dos_reterr( unsigned int );
#endif
#if defined( __NT__ )
extern int  _WCNEAR __set_errno_nt( void );
extern int  _WCNEAR __set_errno_nt_reterr( void );
#endif

#endif
