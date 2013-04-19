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
* Description:  3D dialogs setup.
*
****************************************************************************/


#include "precomp.h"

#ifdef __WINDOWS_386__

static HANDLE   dllCtl3d;

HINDIR DLL32Ctl3dAutoSubclass;
HINDIR DLL32Ctl3dSubclassDlg;
HINDIR DLL32Ctl3dRegister;
HINDIR DLL32Ctl3dUnregister;
HINDIR DLL32Ctl3dColorChange;
HINDIR DLL32Ctl3dSubclassCtl;
HINDIR DLL32Ctl3dCtlColorEx;

/*
 * Init32Ctl3d - load CTL3D.DLL and get entry
 */
BOOL Init32Ctl3d( void )
{
    LPVOID      ptr;

    dllCtl3d = LoadLibrary( "ctl3dv2.dll" );
    if( dllCtl3d < (HANDLE)32 ) {
        return( FALSE );
    }

    ptr = GetProcAddress( dllCtl3d, "Ctl3dSubclassDlg" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dSubclassDlg = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_WORD,
                                                       INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dAutoSubclass" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dAutoSubclass = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dRegister" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dRegister = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dUnregister" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dUnregister = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dColorChange" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dColorChange = GetIndirectFunctionHandle( ptr, INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dSubclassCtl" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dSubclassCtl = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_ENDLIST );

    ptr = GetProcAddress( dllCtl3d, "Ctl3dCtlColorEx" );
    if( ptr == NULL ) {
        FreeLibrary( dllCtl3d );
        return( FALSE );
    }
    DLL32Ctl3dCtlColorEx = GetIndirectFunctionHandle( ptr, INDIR_WORD, INDIR_WORD, INDIR_DWORD, INDIR_ENDLIST );

    return( TRUE );

} /* Init32Ctl3d */


/*
 * Fini32Ctl3d - done with CTL3D.DLL
 */
void Fini32Ctl3d( void )
{
    if( dllCtl3d >= (HANDLE)32 ) {
        FreeLibrary( dllCtl3d );
    }

} /* Fini32Ctl3d */

#endif
