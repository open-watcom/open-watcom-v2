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


#include <stdio.h>
#include "wresall.h"
#include "types.h"
#include "dmpmenu.h"
#include "dmpdiag.h"
#include "dmpicogr.h"
#include "dmpcurgr.h"
#include "dmpstr.h"
#include "dmpcont.h"
#include "wresdefn.h"

bool DumpContents( WResTypeInfo *type, WResResInfo *res,
                    WResLangInfo *lang, WResFileID handle, uint_16 os )
/*********************************************************************/
{
    bool    error;

    res = res;
    fprintf( stdout, "\tOffset: 0x%08lx  Length: 0x%08lx\n",
            lang->Offset, lang->Length );

    if (type->TypeName.IsName) {
        /* it is not one of the predefined types */
        error = false;
    } else {
        switch (type->TypeName.ID.Num) {
        case RESOURCE2INT( RT_MENU ):
            if( os == WRES_OS_WIN16 ) {
                error = DumpMenu( lang->Offset, lang->Length, handle );
            } else {
                error = false;
            }
            break;
        case RESOURCE2INT( RT_DIALOG ):
            if( os == WRES_OS_WIN16 ) {
                error = DumpDialog( lang->Offset, lang->Length, handle );
            } else {
                error = false;
            }
            break;
        case RESOURCE2INT( RT_GROUP_ICON ):
            error = DumpIconGroup( lang->Offset, lang->Length, handle );
            break;
        case RESOURCE2INT( RT_GROUP_CURSOR ):
            error = DumpCursorGroup( lang->Offset, lang->Length, handle );
            break;
        case RESOURCE2INT( RT_STRING ):
            if( os == WRES_OS_WIN16 ) {
                error = DumpString( lang->Offset, lang->Length, handle );
            } else {
                error = false;
            }
            break;
        default:
            error = false;
            break;
        }
    }

    return( error );
}
