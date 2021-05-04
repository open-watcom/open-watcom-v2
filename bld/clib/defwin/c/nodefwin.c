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
* Description:  None Default Windowing support
*
****************************************************************************/


#include "_defwin.h"
#include <stddef.h>


#if defined(_M_IX86)
#pragma aux __init_default_win "*";
#endif
char __init_default_win;

#if ( defined(__OS2__) && ( defined(__386__) || defined(__PPC__) ) ) || defined(__NT__) || \
    defined(__WINDOWS_386__) || defined(__WINDOWS__)

#define __SW_BW
#include "variety.h"
#include <wdefwin.h>

_WCRTLINK int   _dwDeleteOnClose( int handle )
{
    /* unused parameters */ (void)handle;

    return( 0 );
}

_WCRTLINK int   _dwSetAboutDlg( const char *title, const char *text )
{
    /* unused parameters */ (void)text; (void)title;

    return( 0 );
}

_WCRTLINK int   _dwSetAppTitle( const char *title )
{
    /* unused parameters */ (void)title;

    return( 0 );
}

_WCRTLINK int   _dwSetConTitle( int handle, const char *title )
{
    /* unused parameters */ (void)handle; (void)title;

    return( 0 );
}

_WCRTLINK int   _dwYield( void )
{
    return( 0 );
}

_WCRTLINK int   _dwShutDown( void )
{
    return( 0 );
}

#endif
