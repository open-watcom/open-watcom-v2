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

#if defined( __NETWARE__ ) \
  && defined( _THIN_LIB )
    /*
     * not used by OW thin libraries
     */
#elif defined( __NETWARE__ ) \
  && !defined( _THIN_LIB )
    /*
     * OW fat libraries
     */
  #if defined( _NETWARE_LIBC )
    #undef errno
    /*
     * get LibC errno
     */
    #define _RWD_errno      (*___errno())
  #else
    /*
     * get CLib errno
     */
    #define _RWD_errno      errno
  #endif
#elif defined( __QNX__ )
    /*
     * QNX errno is magically multithread aware
     */
    #define _RWD_errno      errno
#elif defined( __RDOSDEV__ )
    #define _RWD_errno      errno
#elif defined( __MT__ )
    #undef errno
    #define _RWD_errno      (__THREADDATAPTR->__errnoP)
#else
    #define _RWD_errno      errno
#endif

/*
 * defined in _dos\c\dosret.c
 */
#if defined( __DOS__ ) \
  || defined( __WINDOWS__ ) \
  || defined( __NT__ ) \
  || defined( __OS2__ )
extern int _WCNEAR __set_errno_dos( unsigned int );
extern int _WCNEAR __set_errno_dos_reterr( unsigned int );
#endif
#if defined( __NT__ )
extern int _WCNEAR __set_errno_nt( void );
extern int _WCNEAR __set_errno_nt_reterr( void );
#endif

/*
 * defined in startup\c\seterrno.c
 */
extern int  _WCNEAR __set_EINVAL( void );

#endif
