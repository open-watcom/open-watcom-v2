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


#ifndef __BROWSE_H__
#define __BROWSE_H__

#include <string.hpp>

#include <wmdiwndw.hpp>
#include <wsyshelp.hpp>
#include <drsrchdf.h>

#include "menumgr.h"
#include "wbrdefs.h"
#include "wbrwin.h"

class WStatWindow;
class Symbol;
class WEditDLL;

class Browse : public ViewEventReceiver, public MenuHandler,
               public WBRWinBase, public WMdiWindow
{
public:
                            Browse();
                            Browse( char* cmdLine );
                            ~Browse();

    virtual void            event( ViewEvent ve, View * view );
    virtual ViewEvent       wantEvents() { return VEOptionValChange; }

            void            positionEditor( char * file, ulong line,
                                            uint col, int len = 0 );
            void            showSource( Symbol * sym );
            void            statusText( const char * text );

    virtual void            menuSelected( const MIMenuID & );
    virtual bool            reallyClose();

            void            postTitle();    // change the application title

            bool            canEdit();

            WSystemHelp *   helpInfo() { return( _helpInfo ); }
            bool            contextHelp( bool );
            bool            makeFileName( char *buff );

private:
            bool            loadModel( const char * fileName );
            void            setupMenus();
            void            setEditorDLL( const char * dll );

            // menu processing
            void            fileMenu( MISubMenu sub );
            void            helpMenu( MISubMenu sub );

            WStatWindow *   _status;
            WSystemHelp *   _helpInfo;
            String          _editorDLLName;
            WEditDLL *      _editorDLL;
            String          _searchPath;
};

extern Browse*  browseTop; //defined in browse.cpp
extern WWindow* topWindow; //defined in browse.cpp

#endif //__BROWSE_H__
