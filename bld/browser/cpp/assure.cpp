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


#include <wmsgdlg.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "death.h"

void Assure( int condition, char * msg, int fatal )
//-------------------------------------------------
{
    if( !condition ) {
        WMessageDialog::message( topWindow, MsgError, MsgOk, msg, "Internal Error" );
        if( fatal ) {
            throw( DEATH_BY_ASSERTION );
        }
    }
}

void NoDefault( char * file, int line )
//-------------------------------------
{
    WMessageDialog::messagef( topWindow, MsgError, MsgOk, "Internal Error",
                    "No default for switch in %s, line %d", file, line );
    throw( DEATH_BY_ASSERTION );
}

void Assertion( int condition, char * cond, char * file, int line )
//-----------------------------------------------------------------
{
    if( !condition ) {
        WMessageDialog::messagef( topWindow, MsgError, MsgOk, "Internal Error",
                    "Assertion Failed - \"%s\" in %s, line %d",
                    cond, file, line );
    }
}
