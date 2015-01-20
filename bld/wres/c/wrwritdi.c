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
#include "layer1.h"
#include "wres.h"
#include "reserr.h"
#include "wresrtns.h"

static bool writeLangList( WResFileID handle, WResLangNode *curlang )
{
    bool        error;

    error = false;
    for( ; curlang != NULL && !error; curlang = curlang->Next ) {
        error = WResWriteLangRecord( &(curlang->Info), handle );
    }
    return( error );
}

static bool writeResList( WResFileID handle, WResResNode *currres )
{
    bool        error;

    error = false;
    for( ; currres != NULL && !error; currres = currres->Next ) {
        error = WResWriteResRecord( &(currres->Info), handle );
        if( !error ) {
            error = writeLangList( handle, currres->Head );
        }
    }

    return( error );
}

static bool writeTypeList( WResFileID handle, WResTypeNode *currtype )
{
    bool        error;

    error = false;
    for( ; currtype != NULL && !error; currtype = currtype->Next ) {
        error = WResWriteTypeRecord( &(currtype->Info), handle );
        if( !error ) {
            error = writeResList( handle, currtype->Head );
        }
    }

    return( error );
}

bool WResWriteDir( WResFileID handle, WResDir currdir )
/****************************************************/
{
    WResHeader      head;
    WResExtHeader   ext_head;
    bool            error;
    WResFileOffset  diroffset;

    /* get the offset of the start of the directory */
    diroffset = WRESTELL( handle );
    error = (diroffset == -1);

    if( error ) {
        WRES_ERROR( WRS_TELL_FAILED );
    } else {
        error = writeTypeList( handle, currdir->Head );
    }
    /* write out the file header */
    if( !error ) {
        head.Magic[0] = WRESMAGIC0;
        head.Magic[1] = WRESMAGIC1;
        head.DirOffset = diroffset;
        head.NumResources = currdir->NumResources;
        head.NumTypes = currdir->NumTypes;
        head.WResVer = WRESVERSION;
        error = WResWriteHeaderRecord( &head, handle );
    }
    if( !error ) {
        memset( &ext_head, 0, sizeof( WResExtHeader ) );
        ext_head.TargetOS = currdir->TargetOS;
        error = WResWriteExtHeader( &ext_head, handle );
    }

    /* leave the handle at the start of the file */
    if( !error ) {
        error = ( WRESSEEK( handle, 0L, SEEK_SET ) == -1 );
        if( error ) {
            WRES_ERROR( WRS_SEEK_FAILED );
        }
    }

    return( error );
}
