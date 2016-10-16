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
* Description:  Implementation of exit() and associated functions.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#if defined(__WINDOWS__) || defined(__WINDOWS_386__)
#include <windows.h>
#endif
#include "rtdata.h"
#include "exitwmsg.h"
#include "initfini.h"
#include "rtinit.h"
#include "defwin.h"
#include "initarg.h"
#include "_int23.h"
#include "procfini.h"
#include "_exit.h"


/*
  __int23_exit is used by OS/2 as a general termination routine which unhooks
  exception handlers.  A better name for this variable is __sig_exit.
  __sig_exit should be the system dependent signal termination routine and
  should replace the calls to __int23_exit and __FPE_handler_exit.
  Each OS should define its own __sig_exit and do the appropriate thing (for
  example, DOS version would call __int23_exit and __FPE_handler_exit)
*/

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__WINDOWS__) && defined(_M_I86)
void    __null_int23_exit( void ) {}              /* SIGNAL needs it */
void    (*__int23_exit)( void ) = __null_int23_exit;
static void _null_exit_rtn( void ) {}
void    (*__FPE_handler_exit)( void ) = _null_exit_rtn;
#endif

_WCRTLINK void exit( int status )
{
#ifdef DEFAULT_WINDOWING
    if( _WindowsExitRtn != NULL ) {      // JBS 27-JUL-98
        _WindowsExitRtn();
    }
#endif
#if defined(__WINDOWS_386__)
    if( __Is_DLL ) {
        _exit( status );
    }
#elif defined(__NT__) || defined(__WARP__)
    (*__int23_exit)();
    if( __Is_DLL ) {
        if( __process_fini != NULL ) {
            (*__process_fini)( FINI_PRIORITY_EXIT, 255 );
        }
        _exit( status );
    }
#elif defined(__DOS__) || defined(__OS2__) || defined(__WINDOWS__) && defined(_M_I86)
    (*__int23_exit)();
#endif
    __FiniRtns( FINI_PRIORITY_EXIT, 255 );
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


#if defined( __NETWARE__ )

/*
 * Netware has own _exit procedure
 */

#else

_WCRTLINK void _exit( int status )
{
#if defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__WINDOWS__) && defined(_M_I86)
    (*__int23_exit)();
    (*__FPE_handler_exit)();
#endif
    __exit( status );
    // never return
}

#endif
