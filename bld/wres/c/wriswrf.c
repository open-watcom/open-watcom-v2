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


#include "layer0.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

extern ResTypeInfo WResFindResType( WResFileID handle )
/*****************************************************/
{
    ResTypeInfo     type;
    long            savepos;
    uint_32         magic[ 2 ];
    int             error;

    savepos = WRESSEEK( handle, 0, SEEK_SET );
    if( savepos == -1L ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    }
    error = ResReadUint32( magic, handle );
    if( !error ) {
        error = ResReadUint32( magic + 1, handle );
    }
    savepos = WRESSEEK( handle, savepos, SEEK_SET );
    if( savepos == -1L ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    }

    type = RT_WIN16; /* what to return if (error) ? */
    if( !error ) {
        if( magic[0] == WRESMAGIC0 && magic[1] == WRESMAGIC1 ) {
            type = RT_WATCOM;
        } else if( magic[0] == 0L ) {
            type = RT_WIN32;
        } else {
            type = RT_WIN16;
        }
    }
    return( type );
}

int WResIsWResFile( WResFileID handle )
/*************************************/
/* Checks the start of the file identified by fp for the Magic number then */
/* resets the postion in the file. Returns true is this is a WRes file */
{
   return( WResFindResType( handle ) == RT_WATCOM );
} /* WResIsWResFile */
