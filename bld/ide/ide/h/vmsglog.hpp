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


#ifndef message_class
#define message_class

#include "wmdichld.hpp"
#include "wstrlist.hpp"
#include "wfilenam.hpp"
#include "vlistbox.hpp"

WCLASS VpeMain;
WCLASS WListBox;
WCLASS WMenuItem;
WCLASS WTimer;
WCLASS WSystemHelp;

WCLASS VMsgLog : public WMdiChild
{
    Declare( VMsgLog )
    public:
        WEXPORT VMsgLog( VpeMain* parent );
        WEXPORT ~VMsgLog();
        void startConnect();
        void WEXPORT setDirectory( const WFileName& dir ) { _cwd = dir; }
        void WEXPORT runCommand( const char* cmd );
        bool WEXPORT running() { return _running; }
        void editRequest( WMenuItem* );
        void helpRequest( WMenuItem* );
        void stopRequest( WMenuItem* );
        void killBatcher();
        void getState( bool& editOk, bool& helpOk );
        bool logExists() const { return _data.count() > 0; }
        bool saveLogAs();
        bool WEXPORT keyDown( WKeyCode, WKeyState );
        bool WEXPORT scrollNotify( WScrollNotification, int );
    private:
        VpeMain*        _parent;
        WVList          _data;
        VListBox*       _batcher;
        int             _maxLength;

        WVList          _helpList;      //<WSystemHelp>

        bool kAccelKey( gui_key key );

        WString         _command;
        WFileName       _cwd;
        void            doRun();
        void            scanLine( const char* buff, int len );
        void            runCmd( WString &cmd );
        bool            _running;

        void            loadConfig();
        void            loadHelpList();
        const char*     findHelpFile( const char* file, WSystemHelp** hobj, int* offset );
        bool            matchPattern( const char* p, int index, char* file, int& line, int& offset, char* help );
        bool            matchLine( int index, char* file, int& line, int& offset, char* help );
        void            selected( WWindow* );
        void            clearData();
        void            addLine( const WString& str, bool newline=TRUE );
        void            focusHandoff();

        bool            _serverConnected;
        bool            _batserv;
        bool            _localBatserv;

        bool            _connecting;
        bool            _runQueued;
        WTimer*         _connectTimer;
        void            connectTimer( WTimer* timer, DWORD );
//only required for VXD version----------------------------------
        bool            _vxdPresent;
//----------------------------------------------------------------
};
#endif
