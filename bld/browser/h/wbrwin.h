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


#ifndef __WBRWIN_H__
#define __WBRWIN_H__

#include <wmdichld.hpp>

class Symbol;
class ViewManager;
class MenuManager;
class OptionManager;
class DatabaseManager;
class WSystemHelp;

const WStyle WBRWinStyleDefault = WStyleDefault;

class WBRWinBase {
    friend  class Browse;

public:
    static  ViewManager *       viewManager();
    static  MenuManager *       menuManager();
    static  OptionManager *     optManager();
    static  DatabaseManager *   dbManager();

    static  void                popDetail( const Symbol * item );

    static  WSystemHelp *       helpInfo();

protected:

    static  OptionManager *     _optManager;
    static  ViewManager *       _viewManager;
    static  MenuManager *       _menuManager;
    static  DatabaseManager *   _dbManager;
};

class WBRWindow : public WMdiChild, public virtual WBRWinBase
{
public:
                            WBRWindow( const char *t,
                                        WStyle s = WBRWinStyleDefault );
                            WBRWindow( const WRect &r, const char *t,
                                        WStyle s = WBRWinStyleDefault );

protected:
            void            popObject( WObject * o ) { popDetail( (Symbol *) o ); }
};

#endif //__WBRWIN_H__
