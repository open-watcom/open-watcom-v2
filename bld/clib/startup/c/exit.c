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
#include "rtdata.h"
#include "exitwmsg.h"
#include "initfini.h"
#include "rtinit.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(__WINDOWS__) || defined(__WINDOWS_386__)
#include <windows.h>
#endif

/*
  __int23_exit is used by OS/2 as a general termination routine which unhooks
  exception handlers.  A better name for this variable is __sig_exit.
  __sig_exit should be the system dependent signal termination routine and
  should replace the calls to __int23_exit and __FPE_handler_exit.
  Each OS should define its own __sig_exit and do the appropriate thing (for
  example, DOS version would call __int23_exit and __FPE_handler_exit)
*/

#ifdef __NETWARE__
extern  void            _exit( int );
#endif

#if defined(__WINDOWS_386__)
extern  char            __Is_DLL;
#endif

#if defined(__NT__) || defined(__WARP__)
extern  char            __Is_DLL;
_WCRTLINK extern void (*__process_fini)(unsigned,unsigned);
#endif


#if !defined(__QNX__) && !defined(__WINDOWS_386__)
void    __null_int23_exit() {}              /* SIGNAL needs it */
void    (*__int23_exit)() = __null_int23_exit;
static void _null_exit_rtn() {}
void    (*__FPE_handler_exit)() = _null_exit_rtn;
#endif

#if defined( __WINDOWS__ ) || defined( __WINDOWS_386__ ) || defined( __OS2__ ) || defined(__NT__)
#define DEFAULT_WINDOWING
extern  void            (*_WindowExitRtn)();
#endif


_WCRTLINK void exit( int status )
    {
#ifdef DEFAULT_WINDOWING
        if( _WindowExitRtn != NULL ) {      // JBS 27-JUL-98
            _WindowExitRtn();
        }
#endif
#if !defined(__QNX__) && !defined(__WINDOWS_386__)
        (*__int23_exit)();
#endif
#if defined(__QNX__)
        __FiniRtns( 0, 255 );
#elif defined(__WINDOWS_386__)
        if( !__Is_DLL ) {
            __FiniRtns( FINI_PRIORITY_EXIT, 255 );
        }
#elif defined(__NT__) || defined(__WARP__)
        if( __Is_DLL ) {
            if( __process_fini != 0 ) {
                (*__process_fini)( FINI_PRIORITY_EXIT, 255 );
            }
        } else {
            __FiniRtns( FINI_PRIORITY_EXIT, 255 );
        }
#else
        __FiniRtns( FINI_PRIORITY_EXIT, 255 );
#endif
        _exit( status );
    }


#if defined(__OS2_286__) && defined(__SW_BD)
_WCRTLINK void _UnloadCLib( void )
    {
        (*__int23_exit)();
        __FiniRtns( FINI_PRIORITY_EXIT, 255 );
        (*__int23_exit)();
        (*__FPE_handler_exit)();
        __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
    }
#endif


#ifndef __NETWARE__

_WCRTLINK void _exit( int status )
    {
#if !defined(__QNX__) && !defined(__WINDOWS_386__)
        (*__int23_exit)();
        (*__FPE_handler_exit)();
#endif
        __exit( status );
    }

#endif
