/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WIN16/WIN386 MakeProcInstance.../FreeProcInstance...
*               for callback functions implementation
*
****************************************************************************/


#if defined( __WINDOWS__ )

#define INCLUDE_COMMDLG_H
#include <wwindows.h>
#include "wclbproc.h"

#ifdef __cplusplus
extern "C" {
#endif

DLGPROC MakeProcInstance_DLG( DLGPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (DLGPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (DLGPROC)(long)fn );
#endif
}
FONTENUMPROC MakeProcInstance_FONTENUM( FONTENUMPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (FONTENUMPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (FONTENUMPROC)(long)fn );
#endif
}
OLDFONTENUMPROC MakeProcInstance_OLDFONTENUM( OLDFONTENUMPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (OLDFONTENUMPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (OLDFONTENUMPROC)(long)fn );
#endif
}
HOOKPROC MakeProcInstance_HOOK( HOOKPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (HOOKPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (HOOKPROC)(long)fn );
#endif
}
LPOFNHOOKPROC MakeProcInstance_OFNHOOK( LPOFNHOOKPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (LPOFNHOOKPROC)(long)fn );
#endif
}
WNDENUMPROC MakeProcInstance_WNDENUM( WNDENUMPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (WNDENUMPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (WNDENUMPROC)(long)fn );
#endif
}
WNDPROC MakeProcInstance_WND( WNDPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (WNDPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    /* unused parameters */ (void)instance;

    return( (WNDPROC)(long)fn );
#endif
}
WNDPROC GetWndProc( WNDPROCx fn )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (WNDPROC)fn );
#else
    return( (WNDPROC)(long)fn );
#endif
}

#if defined( __WINDOWS__ ) && defined( _M_I86 )

void FreeProcInstance_DLG( DLGPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_FONTENUM( FONTENUMPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_OLDFONTENUM( OLDFONTENUMPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_HOOK( HOOKPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_OFNHOOK( LPOFNHOOKPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_WNDENUM( WNDENUMPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}
void FreeProcInstance_WND( WNDPROC fn )
{
    FreeProcInstance( (FARPROC)fn );
}

#endif

#ifdef __cplusplus
}
#endif

#endif
