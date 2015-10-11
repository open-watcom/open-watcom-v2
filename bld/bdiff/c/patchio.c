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


#include "bdiff.h"
#include "patchio.h"

FILE            *PatchFile;
extern char     *PatchName;
extern void     PatchError( int, ... );
extern void     FilePatchError( int, ... );

PATCH_RET_CODE OpenPatch( void )
{
    PatchFile = fopen( PatchName, "rb" );
    if( PatchFile == NULL ) {
        FilePatchError( ERR_CANT_OPEN, PatchName );
        return( PATCH_CANT_OPEN_FILE );
    }
    if( setvbuf( PatchFile, NULL, _IOFBF, BUFFER_SIZE ) != 0 ) {
        FilePatchError( ERR_IO_ERROR, PatchName );
        return( PATCH_IO_ERROR );
    }
    return( PATCH_RET_OKAY );
}

void ClosePatch( void )
{
    fclose( PatchFile );
}

PATCH_RET_CODE InputPatch( byte *tmp, size_t len )
{
    if( fread( tmp, len, 1, PatchFile ) != 1 ) {
        FilePatchError( ERR_CANT_READ, PatchName );
        return( PATCH_CANT_READ );
    }
    return( PATCH_RET_OKAY );
}
