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
* Description:  Convert Watcom resources to Microsoft format.
*
****************************************************************************/


#include <string.h>
#include <unistd.h>
#include "trmemcvr.h"
#include "wresall.h"
#include "types.h"
#include "bincpy.h"
#include "w2mres.h"

static ResNameOrOrdinal * ConvertIDToNameOrOrd( WResID * inid )
/*************************************************************/
{
    ResNameOrOrdinal *  newname;

    if (inid->IsName) {
        /* inculdes space for the '\0' since WResID's don't have one */
        newname = TRMemAlloc( sizeof(ResNameOrOrdinal) +
                    inid->ID.Name.NumChars );
        if (newname != NULL) {
            memcpy( newname->name, inid->ID.Name.Name, inid->ID.Name.NumChars );
            newname->name[ inid->ID.Name.NumChars ] = '\0';
        }
    } else {
        newname = TRMemAlloc( sizeof(ResNameOrOrdinal) );
        if (newname != NULL) {
            newname->ord.fFlag = 0xff;
            newname->ord.wOrdinalID = inid->ID.Num;
        }
    }

    return( newname );
}

static int ConvertOneWResource( WResFileID infile, WResFileID outfile,
                            WResDirWindow wind )
/********************************************************************/
{
    WResTypeInfo        *typeinfo;
    WResResInfo         *resinfo;
    WResLangInfo        *langinfo;
    MResResourceHeader  outhead;
    int                 error;

    typeinfo = WResGetTypeInfo( wind );
    resinfo = WResGetResInfo( wind );
    langinfo = WResGetLangInfo( wind );
    outhead.Type = ConvertIDToNameOrOrd( &(typeinfo->TypeName) );
    outhead.Name = ConvertIDToNameOrOrd( &(resinfo->ResName) );
    outhead.MemoryFlags = langinfo->MemoryFlags;
    outhead.Size = langinfo->Length;

    //FIXME! The last argument should be TRUE for Win32 resources
    error = MResWriteResourceHeader( &outhead, outfile, FALSE );
    if (!error) {
        lseek( infile, langinfo->Offset, SEEK_SET );
        error = BinaryCopy( infile, outfile, outhead.Size );
    }

    TRMemFree( outhead.Type );
    TRMemFree( outhead.Name );

    return( error );
} /* ConvertOneWResource */

static int ConvertWResources( WResFileID infile, WResDir indir,
                    WResFileID outfile )
/*************************************************************/
{
    WResDirWindow       wind;
    int                 error;


    if (WResIsEmpty( indir )) {
        error = TRUE;
    } else {
        error = FALSE;
        wind = WResFirstResource( indir );
        while (!WResIsLastResource( wind, indir ) && !error ) {
            error = ConvertOneWResource( infile, outfile, wind );
            wind = WResNextResource( wind, indir );
        }
        if (!error) {
            /* convert the last resource */
            error = ConvertOneWResource( infile, outfile, wind );
        }
    }

    return( error );
} /* ConvertWResource */

int ConvertWResToMRes( WResFileID infile, WResFileID outfile )
/************************************************************/
{
    WResDir             indir;
    int                 error;

    indir = WResInitDir();
    if (WResDirInitError( indir ) ) {
        return( TRUE );
    }

    error = WResReadDir( infile, indir, NULL );

    if (!error) {
        error = ConvertWResources( infile, indir, outfile );
    }

    WResFreeDir( indir );

    return( error );
}
