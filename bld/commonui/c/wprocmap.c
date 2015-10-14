/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014-2014 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WIN386 specialized Make...ProcInstance functions
*               for far callback function argument.
*
****************************************************************************/


#include "commonui.h"
#include "wprocmap.h"

#if defined( __WINDOWS_386__ )
FARPROC MakeDlgProcInstance( DLGPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
FARPROC MakeFontEnumProcInstance( FONTENUMPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
FARPROC MakeHookProcInstance( HOOKPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
FARPROC MakeOpenFileHookProcInstance( LPOFNHOOKPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
FARPROC MakeWndEnumProcInstance( WNDENUMPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
FARPROC MakeWndProcInstance( WNDPROCx fn, HINSTANCE instance )
{
    instance = instance;
    return( (FARPROC)fn );
}
WNDPROC GetWndProc( WNDPROCx fn )
{
    return( (WNDPROC)fn );
}
#endif
