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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wfiledlg.hpp"

#include <string.h>

WFileDialog::WFileDialog( HWND hwnd, char* filter )
{
        memset( &_ofn, 0, sizeof( _ofn ) );
        _ofn.lStructSize = sizeof( OPENFILENAME );
        _ofn.hwndOwner = hwnd;
        _ofn.lpstrFilter = filter;
        _ofn.nFilterIndex = 1;
        _ofn.lpstrFile = _fileName;
        _ofn.nMaxFile = sizeof( _fileName );
        _fileName[0] = '\0';
        _ofn.lpstrFileTitle = _titleName;
        _ofn.nMaxFileTitle = sizeof( _titleName );
        _ofn.lpstrInitialDir = _dirName;
        _dirName[0] ='\0';
//      GetSystemDirectory( _dirName, sizeof( _dirName ) );
}

WFileDialog::~WFileDialog()
{
}

void WFileDialog::init( char* filename, char* title )
{
        ifptr( title ) {
                _ofn.lpstrTitle = title;
        }
        ifptr( filename ) {
                strcpy( _fileName, filename );
        }
}

char* WFileDialog::getOpenFileName( char* filename, char* title, int style )
{
        init( filename, title );
        _ofn.Flags = style;
        if( GetOpenFileName( &_ofn ) ) {
                return AnsiLower(_fileName);
        }
        return NIL;
}

char* WFileDialog::getSaveFileName( char* filename, char* title, int style )
{
        _ofn.Flags = style;
        init( filename, title );
        if( GetSaveFileName( &_ofn ) ) {
                return AnsiLower(_fileName);
        }
        return NIL;
}
