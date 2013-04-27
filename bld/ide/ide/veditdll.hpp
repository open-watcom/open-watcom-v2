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


#ifndef veditdll_class
#define veditdll_class

#include "wfilenam.hpp"
#include "wsystem.hpp"

extern "C" {
#include "wedit.h"
};

typedef int (EDITAPI *ConnectFn)( void );
typedef int (EDITAPI *FileFn)( editstring, editstring );
typedef int (EDITAPI *LocFn)( long, int, int );
typedef int (EDITAPI *LocErrFn)( long, int, int, int, editstring );
typedef int (EDITAPI *ShowFn)( show_method );
typedef int (EDITAPI *DisconnectFn)( void );
typedef int (EDITAPI *SaveAllFn)( void );
typedef int (EDITAPI *SaveThisFileFn)( const char * );
typedef int (EDITAPI *IsFileInBufFn)( const char * );

WCLASS VEditDLL : public WObject
{
    public:
        VEditDLL();
        ~VEditDLL();
        bool isInitialized(void ) { return( _hdl != 0 ); }
        void LoadDll( const char *dllname, WString *errmsg );
        void UnloadDll( void );
        int EDITConnect( void );
        int EDITFile( editstring, editstring );
        int EDITLocate( long, int, int );
        int EDITLocateError( long, int, int, int, editstring );
        int EDITShowWindow( show_method );
        int EDITDisconnect( void );
        int EDITSaveAll( void );
        int EDITSaveThis( WFileName *fn );
        int EDITIsFileInBuf( WFileName *fn );
    private:
        WModuleHandle           _hdl;
        ConnectFn               _connect;
        FileFn                  _file;
        LocFn                   _locate;
        LocErrFn                _locateError;
        ShowFn                  _showWindow;
        SaveAllFn               _saveAll;
        SaveThisFileFn          _saveThis;
        IsFileInBufFn           _isFileInBuf;
        DisconnectFn            _disconnect;

        void resetPointers( void );
};

#endif
