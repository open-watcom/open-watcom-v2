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
#include "bincpy.h"
#include "param.h"
#include "m2wres.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "wres.h"


static WResID * ConvertNameOrOrdToID( ResNameOrOrdinal * name )
/*************************************************************/
{
    if (name->ord.fFlag == 0xff) {
        return( WResIDFromNum( name->ord.wOrdinalID ) );
    } else {
        return( WResIDFromStr( name->name ) );
    }
}

static bool ConvertMResources( WResFileID infile, WResFileID outfile, WResDir outdir )
/************************************************************************************/
{
    MResResourceHeader *    mheader;
    WResID *                name;
    WResID *                type;
    bool                    error;
    bool                    lastheader; /* true if lastheader has been read */
    uint_32                 offset;
    bool                    duplicate;

    mheader = MResReadResourceHeader( infile );
    /* assume that any error reading here means end of file */
    lastheader = ( mheader == NULL );
    error = false;

    while( !lastheader && !error ) {
        name = ConvertNameOrOrdToID( mheader->Name );
        type = ConvertNameOrOrdToID( mheader->Type );
        offset = RCTELL( outfile );

        /* copy the resource if it isn't a name table or if the user */
        /* requested that name tables be copied */
        if( type->IsName || type->ID.Num != RT_NAMETABLE ||
                        CmdLineParms.KeepNameTable ) {
            error = WResAddResource( type, name, mheader->MemoryFlags, offset,
                        mheader->Size, outdir, NULL, &duplicate );
            if( duplicate ) {
                /* print message and continue */
                puts( "Error: duplicate entry" );
                error = false;
            } else {
                error = BinaryCopy( infile, outfile, mheader->Size );
            }
        } else {
            RCSEEK( infile, mheader->Size, SEEK_CUR );
        }

        WResIDFree( name );
        WResIDFree( type );
        MResFreeResourceHeader( mheader );

        mheader = MResReadResourceHeader( infile );
        /* assume that any error reading here means end of file */
        lastheader = ( mheader == NULL );
    }

    return( error );
} /* ConvertMResources */

bool ConvertMResToWRes( WResFileID infile, WResFileID outfile )
/*************************************************************/
{
    WResDir                 outdir;
    bool                    error;

    outdir = WResInitDir();
    if( WResDirInitError( outdir ) ) {
        return( true );
    }

    error = ConvertMResources( infile, outfile, outdir );

    if( !error ) {
        error = WResWriteDir( outfile, outdir );
    }

    WResFreeDir( outdir );

    return( error );
} /* ConvertMResToWres */
