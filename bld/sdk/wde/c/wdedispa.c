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
* Description:  In-memory Dialog manipulation functions
*
****************************************************************************/


#include "wdeglbl.h"
#include "wdedispa.h"


#if defined( __WINDOWS__ )

DISPATCHERPROC MakeProcInstance_DISPATCHER( DISPATCHERPROCx fn, HINSTANCE instance )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    return( (DISPATCHERPROC)MakeProcInstance( (FARPROC)fn, instance ) );
#else
    instance = instance;
    return( (DISPATCHERPROC)fn );
#endif
}

#if defined( __WINDOWS__ ) && defined( _M_I86 )

void FreeProcInstance_DISPATCHER( DISPATCHERPROC fn )
{
#if defined( __WINDOWS__ ) && defined( _M_I86 )
    FreeProcInstance( (FARPROC)fn );
#else
    fn = fn;
#endif
}

#endif

#endif
