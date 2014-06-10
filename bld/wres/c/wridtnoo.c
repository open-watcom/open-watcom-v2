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


#include <string.h>
#include "layer0.h"
#include "util.h"
#include "reserr.h"
#include "wresrtns.h"

extern ResNameOrOrdinal * WResIDToNameOrOrd( WResID * id )
/********************************************************/
{
    ResNameOrOrdinal *  newname;

    if (id->IsName) {
        /* the one char in the ResNameOrOrdinal gives room for the '\0' */
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) + id->ID.Name.NumChars );
        if (newname == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            memcpy( newname->name, id->ID.Name.Name, id->ID.Name.NumChars );
            newname->name[ id->ID.Name.NumChars ] = '\0';
        }
    } else {
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) );
        if (newname == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            newname->ord.fFlag = 0xff;
            newname->ord.wOrdinalID = id->ID.Num;
        }
    }

    return( newname );
}


extern ResNameOrOrdinal * WResHelpIDToNameOrOrd( WResHelpID * id )
/********************************************************/
{
    ResNameOrOrdinal *  newname;

    if (id->IsName) {
        /* the one char in the ResNameOrOrdinal gives room for the '\0' */
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) + id->ID.Name.NumChars );
        if (newname == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            memcpy( newname->name, id->ID.Name.Name, id->ID.Name.NumChars );
            newname->name[ id->ID.Name.NumChars ] = '\0';
        }
    } else {
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) );
        if (newname == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            newname->ord.fFlag = 0xff;
            newname->ord.wOrdinalID = id->ID.Num;
        }
    }

    return( newname );
}
