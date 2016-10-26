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
* Description:  OS/2 kbhit() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#define INCL_16
#define INCL_SUB
#include <wos2.h>
#include <conio.h>
#include "rtdata.h"
#include "defwin.h"

#if defined(__OS2_286__)
    extern  signed char _os_kbhit( void );
    #pragma aux  _os_kbhit = "mov ah,0bh"   \
                             "int 21h"      \
                             value [al];
#endif

_WCRTLINK int kbhit( void )
{
    KBDKEYINFO  info;

    if( _RWD_cbyte != 0 )
        return( 1 );
#ifdef DEFAULT_WINDOWING
    if( _WindowsKbhit != 0 ) {      // Default windowing
        LPWDATA     res;
        res = _WindowsIsWindowedHandle( (int) STDIN_FILENO );
        return( _WindowsKbhit( res ) );
    }
#endif
#if defined(__OS2_286__)
    if( _RWD_osmode == DOS_MODE ) {
        return( _os_kbhit() );
    }
    KbdPeek( &info, 0 );
    return( ( info.fbStatus & 0xe0 ) != 0 );
#else
    KbdPeek( &info, 0 );
    return( ( info.fbStatus & 0xe0 ) != 0 );
#endif
}
