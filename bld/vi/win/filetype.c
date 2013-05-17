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
* Description:  Handling of file type icons.
*
****************************************************************************/


#include "vi.h"
#include "iconids.h"

typedef struct filetype {
    const char  *extension;
    const char  *iconname;
} filetype;

#define NUM_FILE_TYPES  15
#ifdef __NT__
static const filetype fileTypes[NUM_FILE_TYPES] = {
    { ".c",     MAKEINTRESOURCE( IDI_CFILE ) },
    { ".cpp",   MAKEINTRESOURCE( IDI_CPPFILE ) },
    { ".cxx",   MAKEINTRESOURCE( IDI_CPPFILE ) },
    { ".h",     MAKEINTRESOURCE( IDI_HFILE ) },
    { ".hpp",   MAKEINTRESOURCE( IDI_HPPFILE ) },
    { ".hxx",   MAKEINTRESOURCE( IDI_HPPFILE ) },
    { ".for",   MAKEINTRESOURCE( IDI_FORFILE ) },
    { ".f",     MAKEINTRESOURCE( IDI_FORFILE ) },
    { ".fi",    MAKEINTRESOURCE( IDI_FIFILE ) },
    { ".mak",   MAKEINTRESOURCE( IDI_MAKFILE ) },
    { ".mk",    MAKEINTRESOURCE( IDI_MAKFILE ) },
    { ".mif",   MAKEINTRESOURCE( IDI_MAKFILE ) },
    { ".rc",    MAKEINTRESOURCE( IDI_RCFILE ) },
    { ".rh",    MAKEINTRESOURCE( IDI_RHFILE ) },
    { ".awk",   MAKEINTRESOURCE( IDI_AWKFILE ) }
};
#endif

typedef HANDLE (CALLBACK *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );

/*
 * UpdateFileTypeIcon - update a window's icon for a new file name
 */
void UpdateFileTypeIcon( HWND hwnd, const char *filename )
{
#ifdef __NT__
    const char  *extension;
    int         i;
    HICON       hicon = NULL;
    HICON       smallicon = NULL;
    HINSTANCE   hInstUser;
    PFNLI       pfnLoadImage;
    hInstUser = GetModuleHandle( "USER32.DLL" );
    pfnLoadImage = (PFNLI)GetProcAddress( hInstUser, "LoadImageA" );
    if( pfnLoadImage != NULL ) {
        extension = strrchr( filename, '.' );
        if( extension != NULL ) {
            for( i = 0; i < NUM_FILE_TYPES; i++ ) {
                if( strcmp( extension, fileTypes[i].extension) == 0 ) {
                    hicon = pfnLoadImage( InstanceHandle, fileTypes[i].iconname,
                                          IMAGE_ICON, 32, 32, 0 );
                    smallicon = pfnLoadImage( InstanceHandle, fileTypes[i].iconname,
                                              IMAGE_ICON, 16, 16, 0 );
                    break;
                }
            }
        }
        if( hicon == NULL ) {
            hicon = LoadIcon( InstanceHandle, MAKEINTRESOURCE( IDI_TXTFILE ) );
        }
        SendMessage( hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon );
        SendMessage( hwnd, WM_SETICON, ICON_SMALL, (LPARAM)smallicon );
    }
#endif

} /* UpdateFileTypeIcon */

