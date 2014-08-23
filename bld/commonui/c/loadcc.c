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
* Description:  Dynamic loading of COMCTL32.DLL
*
****************************************************************************/


#include "precomp.h"
#include "bool.h"
#include "loadcc.h"

#ifdef __NT__
typedef VOID (WINAPI *PFNICC)( VOID );
#endif

static bool checked = false;
static bool loaded = false;

/*
 * LoadCommCtrl - attempt to load COMCTL32.DLL
 */
bool LoadCommCtrl( void )
{
    if( !checked ) {
#ifdef __NT__
        HINSTANCE hinst = LoadLibrary( "COMCTL32.DLL" );
        if( hinst != NULL ) {
            PFNICC pfn = (PFNICC)GetProcAddress( hinst, "InitCommonControls" );
            if( pfn != NULL ) {
                pfn();
                loaded = true;
            }
        }
#endif
        checked = true;
    }
    return( loaded );

} /* LoadCommCtrl */

/*
 * IsCommCtrlLoaded - indicate whether COMCTL32.DLL has been loaded
 */
bool IsCommCtrlLoaded( void )
{
    return( loaded );

} /* IsCommCtrlLoaded */
