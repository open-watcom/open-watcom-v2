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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#define INCL_16
#define INCL_SUB
#include <wos2.h>
#include <conio.h>
#include "dosfuncx.h"
#include "tinyio.h"
#include "rtdata.h"
#include "defwin.h"


#if defined(__OS2_16BIT__)
    extern unsigned char    _dos( char );
    #pragma aux _dos = \
            __INT_21    \
        __parm __caller [__ah] \
        __value         [__al]
#endif

_WCRTLINK int getch( void )
{
    int         c;
    APIRET      rc;
    KBDKEYINFO  info;

    if( (c = _RWD_cbyte) != 0 ) {
        _RWD_cbyte = 0;
        return( c );
    }
#ifdef DEFAULT_WINDOWING
    if( _WindowsGetch != NULL ) {   // Default windowing
        LPWDATA     res;
        res = _WindowsIsWindowedHandle( STDIN_FILENO );
        return( _WindowsGetch( res ) );
    }
#endif
#if defined(__OS2_16BIT__)
    if( _osmode_REALMODE() ) {
        return( _dos( DOS_GET_CHAR_NO_ECHO_CHECK ) );
    }
#endif
    if( (c = _RWD_cbyte2) != 0 ) {
        _RWD_cbyte2 = 0;
        return( c );
    }
    rc = KbdCharIn( &info, 0, 0 );
    if( rc == ERROR_KBD_DETACHED )
        return( EOF );
    if( info.chChar == 0
      || info.chChar == 0xe0 ) {
        _RWD_cbyte2 = info.chScan;
    }
    return( info.chChar );
}
