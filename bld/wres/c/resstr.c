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


#include <limits.h>
#include "layer0.h"
#include "write.h"
#include "resstr.h"
#include "wresrtns.h"

void ResInitStringTableBlock( StringTableBlock * currblock )
/**********************************************************/
{
    int     stringid;

    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK; stringid++ ) {
        currblock->String[stringid] = NULL;
    }
} /* ResInitStringTableBlock */

void ResFreeStringTableBlock( StringTableBlock * currblock )
/**********************************************************/
{
    int     stringid;

    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK; stringid++ ) {
        if( currblock->String[stringid] != NULL ) {
            WRESFREE( currblock->String[stringid] );
        }
    }
} /* ResFreeStringTableBlock */

bool ResWriteStringTableBlock( StringTableBlock *currblock,
                            bool use_unicode, WResFileID handle )
/***************************************************************/
{
    int     stringid;
    bool    error;

    error = false;
    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK && !error; stringid++ ) {
        error = WResWriteWResIDNameUni( currblock->String[stringid],
                                    use_unicode, handle );
    }

    return( error );
} /* ResWriteStringTableBlock */
