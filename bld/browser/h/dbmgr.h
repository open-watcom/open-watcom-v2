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


#ifndef __DBMGR_H__
#define __DBMGR_H__

#include <string.hpp>

#include "wbrdefs.h"
#include "menumgr.h"

class WFileName;
class Module;
template <class T> class WCValSList;

class DatabaseManager : public MenuHandler {
public:

                            DatabaseManager();
                            ~DatabaseManager();

    virtual void            menuSelected( const MIMenuID & id );

            void            setMenus( MenuManager * mgr );
            void            unSetMenus( MenuManager * mgr );

            const char *    fileName();

            void            setModule( const char * file,
                                       WCValSList<String> * enabled,
                                       WCValSList<String> * disabled=NULL );
            void            editModule( const char * f, bool create = FALSE );
            Module *        module() { return _module; }

protected:
            void            databaseChanged();
            void            editModule();
            void            newDatabase();
            void            openDatabase();

private:
            MenuManager *   _menuManager;

            Module *        _module;
};

#endif  // __DBMGR_H__
