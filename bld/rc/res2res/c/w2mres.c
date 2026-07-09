/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "wresall.h"
#include "bincpy.h"
#include "w2mres.h"
#include "rcrtns.h"
#include "trmemcvr.h"


static ResNameOrOrdinal *ConvertIDToNameOrOrdinal( WResID *id )
/*************************************************************/
{
    ResNameOrOrdinal    *name;

    if (id->IsName) {
        /* inculdes space for the '\0' since WResID's don't have one */
        name = TRMemAlloc( sizeof(ResNameOrOrdinal) +
                    id->ID.Name.NumChars );
        if (name != NULL) {
            memcpy( name->name, id->ID.Name.Name, id->ID.Name.NumChars );
            name->name[id->ID.Name.NumChars] = '\0';
        }
    } else {
        name = TRMemAlloc( sizeof(ResNameOrOrdinal) );
        if (name != NULL) {
            name->ord.fFlag = 0xff;
            name->ord.wOrdinalID = id->ID.Num;
        }
    }

    return( name );
}

static bool ConvertOneWResource( FILE *in_fp, FILE *out_fp, WResDirWindow wind )
/******************************************************************************/
{
    WResTypeInfo        *typeinfo;
    WResResInfo         *resinfo;
    WResLangInfo        *langinfo;
    MResResourceHeader  msheader;
    bool                error;
    bool                iswin32;

    iswin32 = false;
    typeinfo = WResGetTypeInfo( wind );
    resinfo = WResGetResInfo( wind );
    langinfo = WResGetLangInfo( wind );
    msheader.Type = ConvertIDToNameOrOrdinal( &(typeinfo->TypeName) );
    msheader.Name = ConvertIDToNameOrOrdinal( &(resinfo->ResName) );
    msheader.MemoryFlags = langinfo->MemoryFlags;
    msheader.Size = langinfo->Length;

    //FIXME! The last argument should be true for Win32 resources
    error = MResWriteResourceHeader( &msheader, out_fp, iswin32 );
    if( !error ) {
        RESSEEK( in_fp, langinfo->Offset, SEEK_SET );
        error = BinaryCopy( in_fp, out_fp, msheader.Size );
    }

    TRMemFree( msheader.Type );
    TRMemFree( msheader.Name );

    return( error );
} /* ConvertOneWResource */

static bool ConvertWResources( FILE *in_fp, WResDir indir, FILE *out_fp )
/***********************************************************************/
{
    WResDirWindow       wind;
    bool                error;


    if( WResIsEmpty( indir ) ) {
        error = true;
    } else {
        error = false;
        wind = WResFirstResource( indir );
        while( !WResIsLastResource( wind, indir ) && !error ) {
            error = ConvertOneWResource( in_fp, out_fp, wind );
            wind = WResNextResource( wind, indir );
        }
        if( !error ) {
            /* convert the last resource */
            error = ConvertOneWResource( in_fp, out_fp, wind );
        }
    }

    return( error );
} /* ConvertWResource */

bool ConvertWResToMRes( FILE *in_fp, FILE *out_fp )
/*************************************************/
{
    WResDir             indir;
    bool                error;

    if( WResReadDir( in_fp, &indir, NULL ) )
        return( true );

    error = ConvertWResources( in_fp, indir, out_fp );

    WResFreeDir( indir );

    return( error );
}
