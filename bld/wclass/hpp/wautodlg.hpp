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


#ifndef wautodialog_class
#define wautodialog_class

#include "wdialog.hpp"
#include "wstrlist.hpp"

WCLASS WText;
WCLASS WEditBox;
WCLASS WAutoDialog  : public WDialog {
    public:
        WEXPORT WAutoDialog( WWindow *parent, const char *text,
                             WStringList &prompts, unsigned editwidth = 32 );
        WEXPORT ~WAutoDialog();
        void WEXPORT okButton( WWindow * );
        void WEXPORT cancelButton( WWindow * );
        bool WEXPORT getInput( WStringList &replys );
    private:
        void initialize( void );
        void updateExtents( const char *, int *, int * );
        void browseButton( WWindow * );
    private:
        WStringList*    _prompts;
        WStringList*    _replys;
        WVList          _inputs;
        unsigned        _editwidth;
};

#endif
