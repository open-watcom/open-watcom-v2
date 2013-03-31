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
#include "filefmt.h"
#include "write.h"
#include "opcl.h"
#include "reserr.h"

int WResFileInit( WResFileID handle )
/***********************************/
/* Writes the initial file header out to the file. Later, when WResWriteDir */
/* is called the real header will be written out */
{
    WResHeader  head;
    int         error;
    long        seek_rc;

    head.Magic[0] = WRESMAGIC0;
    head.Magic[1] = WRESMAGIC1;
    head.DirOffset = 0;
    head.NumResources = 0;
    head.NumTypes = 0;
    head.WResVer = WRESVERSION;

    /* write the empty record out at the begining of the file */
    seek_rc = WRESSEEK( handle, 0, SEEK_SET );
    error = (seek_rc == -1L);
    if (error) {
        WRES_ERROR( WRS_SEEK_FAILED );
    } else {
        error = WResWriteHeaderRecord( &head, handle );
    }

    /* leave room for the extended header */
    if( error ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    } else {
        seek_rc = WRESSEEK( handle, sizeof(WResExtHeader), SEEK_CUR );
        error = (seek_rc == -1L);
    }
    if( error ) WRES_ERROR( WRS_SEEK_FAILED );

    return( error );
} /* WResFileInit */
