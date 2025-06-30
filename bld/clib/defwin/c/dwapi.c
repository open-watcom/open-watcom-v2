/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


#define __SW_BW
#include "variety.h"
#include <stddef.h>
#include <wdefwin.h>
#include "defwin.h"


_WCRTLINK int   _dwDeleteOnClose( int handle )
//============================================
{
#ifdef DEFAULT_WINDOWING
    LPWDATA     res;

    if( _WindowsDestroyOnClose != NULL ) {
        res = _WindowsIsWindowedHandle( handle );
        if( res != NULL ) {
            return( _WindowsDestroyOnClose( res ) );
        }
    }
#else
    (void)handle;
#endif
    return( 0 );
}


_WCRTLINK int   _dwSetAboutDlg( const char *title, const char *text )
//===================================================================
{
#ifdef DEFAULT_WINDOWING

    if( _WindowsSetAbout != NULL ) {
        return( _WindowsSetAbout( title, text ) );
    }
#else
    (void)text;
    (void)title;
#endif
    return( 0 );
}


_WCRTLINK int   _dwSetAppTitle( const char *title )
//=================================================
{
#ifdef DEFAULT_WINDOWING

    if( _WindowsSetAppTitle != NULL ) {
        return( _WindowsSetAppTitle( title ) );
    }
#else
    (void)title;
#endif
    return( 0 );
}


_WCRTLINK int   _dwSetConTitle( int handle, const char *title )
//=============================================================
{
#ifdef DEFAULT_WINDOWING

    LPWDATA     res;

    if( _WindowsSetConTitle != NULL ) {
        res = _WindowsIsWindowedHandle( handle );
        if( res != NULL ) {
            return( _WindowsSetConTitle( res, title ) );
        }
    }
#else
    (void)handle;
    (void)title;
#endif
    return( 0 );
}


_WCRTLINK int   _dwYield( void )
//==============================
{
#ifdef DEFAULT_WINDOWING

    if( _WindowsYieldControl != NULL ) {
        return( _WindowsYieldControl() );
    }
#endif
    return( 0 );
}


_WCRTLINK int   _dwShutDown( void )
//=================================
{
#ifdef DEFAULT_WINDOWING

    if( _WindowsShutDown != NULL ) {
        return( _WindowsShutDown() );
    }
#endif
    return( 0 );
}
