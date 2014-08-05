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


#include "veditdll.hpp"

extern char _viperError[];

#if defined( __WINDOWS__ )
#define EDITCONNECT     "EDITCONNECT"
#define EDITFILE        "EDITFILE"
#define EDITLOCATE      "EDITLOCATE"
#define EDITLOCATEERROR "EDITLOCATEERROR"
#define EDITSHOWWINDOW  "EDITSHOWWINDOW"
#define EDITDISCONNECT  "EDITDISCONNECT"
#define EDITSAVEALL     "EDITSAVEFILES"
#define EDITSAVETHIS    "EDITSAVETHISFILE"
#define EDITISFILEINBUF "EDITQUERYTHISFILE"
#elif defined( __NT__ ) && defined( __386__ )
#define EDITCONNECT     "_EDITConnect@0"
#define EDITFILE        "_EDITFile@8"
#define EDITLOCATE      "_EDITLocate@12"
#define EDITLOCATEERROR "_EDITLocateError@20"
#define EDITSHOWWINDOW  "_EDITShowWindow@4"
#define EDITDISCONNECT  "_EDITDisconnect@0"
#define EDITSAVEALL     "_EDITSaveFiles@0"
#define EDITSAVETHIS    "_EDITSaveThisFile@4"
#define EDITISFILEINBUF "_EDITQueryThisFile@4"
#else
#define EDITCONNECT     "EDITConnect"
#define EDITFILE        "EDITFile"
#define EDITLOCATE      "EDITLocate"
#define EDITLOCATEERROR "EDITLocateError"
#define EDITSHOWWINDOW  "EDITShowWindow"
#define EDITDISCONNECT  "EDITDisconnect"
#define EDITSAVEALL     "EDITSaveFiles"
#define EDITSAVETHIS    "EDITSaveThisFile"
#define EDITISFILEINBUF "EDITQueryThisFile"
#endif

VEditDLL::VEditDLL() {
    _hdl = 0;
    resetPointers();
}

VEditDLL::~VEditDLL() {
    resetPointers();
}

void VEditDLL::resetPointers( void ) {
    _connect = NULL;
    _file = NULL;
    _locate = NULL;
    _locateError = NULL;
    _showWindow = NULL;
    _disconnect = NULL;
    _saveAll = NULL;
    _saveThis = NULL;
    _isFileInBuf = NULL;
    if( _hdl != 0 ) WSystemService::freeLibrary( _hdl );
    _hdl = 0;
}

void VEditDLL::UnloadDll( void ) {
    EDITDisconnect();
    resetPointers();
}

void VEditDLL::LoadDll( const char *dllname, WString *errmsg ) {
    _hdl = WSystemService::loadLibrary( dllname );
    if( _hdl == 0 ) {
        errmsg->printf( "Unable to find editor dll '%s'.  Editor functionality will be unavailable.",
                     dllname );
    } else {
        _connect = (ConnectFn)WSystemService::getProcAddr( _hdl, EDITCONNECT );
        _file = (FileFn)WSystemService::getProcAddr( _hdl, EDITFILE );
        _locate = (LocFn)WSystemService::getProcAddr( _hdl, EDITLOCATE );
        _locateError = (LocErrFn)WSystemService::getProcAddr( _hdl, EDITLOCATEERROR );
        _showWindow = (ShowFn)WSystemService::getProcAddr( _hdl, EDITSHOWWINDOW );
        _disconnect = (DisconnectFn)WSystemService::getProcAddr( _hdl, EDITDISCONNECT );
        _saveAll = (SaveAllFn)WSystemService::getProcAddr( _hdl, EDITSAVEALL );
        _saveThis = (SaveThisFileFn)WSystemService::getProcAddr( _hdl, EDITSAVETHIS );
        _isFileInBuf = (IsFileInBufFn)WSystemService::getProcAddr( _hdl, EDITISFILEINBUF );
        if( _connect == NULL || _file == NULL || _locate == NULL
                || _locateError == NULL || _showWindow == NULL
                || _disconnect == NULL ) {
            errmsg->printf( "Invalid editor DLL '%s' found.  Editor functionality will be unavailable.",
                         dllname );
            resetPointers();
        }
    }
}

int VEditDLL::EDITConnect( void ) {
    if( _connect == NULL ) return( (int)true );
    return( _connect() );
}

int VEditDLL::EDITFile( editstring filename , editstring helpfile ) {
    if( _file == NULL ) return( (int)true );
    return( _file( filename, helpfile ) );
}

int VEditDLL::EDITLocate( long row, int col, int len ) {
    if( _locate == NULL ) return( (int)true );
    return( _locate( row, col, len ) );
}

int VEditDLL::EDITLocateError( long row, int col, int len, int idres,
                               editstring errmsg )
{
    if( _locateError == NULL ) return( (int)true );
    return( _locateError( row, col, len, idres, errmsg ) );
}

int VEditDLL::EDITShowWindow( show_method cmdshow ) {
    if( _showWindow == NULL ) return( (int)true );
    return( _showWindow( cmdshow ) );
}

int VEditDLL::EDITDisconnect( void ) {
    if( _disconnect == NULL ) return( (int)true );
    return( _disconnect() );
}

int VEditDLL::EDITSaveAll( void ) {
    if( _saveAll == NULL ) return( (int)true );
    return( _saveAll() );
}

int VEditDLL::EDITSaveThis( WFileName *fn ) {
    if( _saveThis == NULL ) return( (int)true );
    return( _saveThis( fn->gets() ) );
}

int VEditDLL::EDITIsFileInBuf( WFileName *fn ) {
    if( _isFileInBuf == NULL ) return( (int)false );
    return( _isFileInBuf( fn->gets() ) );
}
