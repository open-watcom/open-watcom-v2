/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  prototypes and definitions for iomode array manipulation
*
****************************************************************************/


#ifndef _IOMODE_H_INCLUDED
#define _IOMODE_H_INCLUDED

#if defined(__NT__)

#define DUMMY_HANDLE ((HANDLE)(LONG_PTR)-2)

extern  void        __initPOSIXHandles( void );
extern  unsigned    __growPOSIXHandles( unsigned num );
extern  int         __allocPOSIXHandle( HANDLE osfh );
extern  void        __freePOSIXHandle( int handle );
extern  HANDLE      __getOSHandle( int handle );
extern  int         __setOSHandle( int handle, HANDLE osfh );
extern  HANDLE      __NTGetFakeHandle( void );

#define __allocPOSIXHandleDummy()   __allocPOSIXHandle( DUMMY_HANDLE )

#define NT_STDIN_FILENO  (GetStdHandle( STD_INPUT_HANDLE ))
#define NT_STDOUT_FILENO (GetStdHandle( STD_OUTPUT_HANDLE ))
#define NT_STDERR_FILENO (GetStdHandle( STD_ERROR_HANDLE ))

#endif

#if !defined(__NETWARE__)

extern  unsigned    __NHandles;
extern  unsigned    __NFiles;              /* maximum # of files we can open */

extern  unsigned    _WCNEAR __GetIOMode( int handle );
extern  void        _WCNEAR __SetIOMode( int handle, unsigned value );
extern  int         _WCNEAR __SetIOMode_grow( int handle, unsigned value );
extern  void        _WCNEAR __ChkTTYIOMode( int handle );

#endif

#endif
