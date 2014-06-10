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


#include "wcombox.hpp"
#include "wcmdlist.hpp"


WCommandList::WCommandList( WWindow* parent, const WRect& r,
                            const char *text )
        : WEditComboBox( parent, r, text ) {
/******************************************/

}

WCommandList::WCommandList( WStatDialog* parent, unsigned control_id )
        : WEditComboBox( parent, control_id ) {
/*********************************************/

}


WString& WCommandList::getCommand( WString& command ) {
/*****************************************************/

    getText( command );
    // make sure that we don't insert empty strings
    if( *((const char *)command) != '\0' && selected() == -1 ) {
        int pos = insertString( command );
        select( pos );
    }
    return( command );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WCommandList::~WCommandList() {
/*****************************/

}
