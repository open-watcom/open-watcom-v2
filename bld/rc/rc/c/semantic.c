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
* Description:  Semantic actions called by the YACC generated driver
*               (for both grammars).
*
****************************************************************************/


#include <stdio.h>      /* remove call for MS format stuff */
#include <errno.h>
#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "rcrtns.h"
#include "rccore.h"
#include "cpyfdata.h"
#include "wres.h"


/*
 * used in the work around for MS format RES files
 */
static FILE         *tmpResFile = NULL;     /* holding place for the RES file handle */
static char         *tmpResFileName = TMPFILE1;

static FILE         *save_fp;               /* holding place for the RES file handle */
static char         *save_name;

/*
 * Modified from WINNT.H
 */
#ifndef MAKELANGID
#define MAKELANGID(p, s)       ((((uint_16)(s)) << 10) | (uint_16)(p))
#endif

SemOffset SemStartResource( void )
/********************************/
{
    if( StopInvoked ) {
        RcFatalError( ERR_STOP_REQUESTED );
        /* never return */
    }
    if( !CurrResFile.IsWatcomRes ) {
        tmpResFile = ResOpenFileTmp( NULL );
        if( tmpResFile == NULL ) {
            RCCloseFile( &(CurrResFile.fp) );
            RcFatalError( ERR_OPENING_TMP, tmpResFileName, LastWresErrStr() );
            /* never return */
        }
        /*
         * save current values
         */
        save_fp = CurrResFile.fp;
        save_name = CurrResFile.filename;
        CurrResFile.fp = tmpResFile;
        CurrResFile.filename = tmpResFileName;
    }
    return( ResTell( CurrResFile.fp ) );
}

SemLength SemEndResource( SemOffset start )
/*****************************************/
{
    SemLength   len;

    /*
     * the length of the resource
     */
    len = ResTell( CurrResFile.fp ) - start;
    if( !CurrResFile.IsWatcomRes ) {
        /*
         * restore previous values
         */
        CurrResFile.fp = save_fp;
        CurrResFile.filename = save_name;
    }
    return( len );
}

void SemAddResourceAndFree( WResID *res_id, WResID *type_id, ResMemFlags flags, ResLocation loc )
/******************************************************************************************/
{
    SemAddResource2( res_id, type_id, flags, loc, NULL );
    MemFree( res_id );
    MemFree( type_id );
}

static void copyMSFormatRes( WResID *res_id, WResID *type_id, ResMemFlags flags,
                ResLocation loc, const WResLangType *lang, bool iswin32 )
/*************************************************************************/
{
    MResResourceHeader  msheader;
    bool                error;
    char                buffer[512];

    /*
     * fill in and output a MS format resource header
     */
    msheader.Type = WResIDToNameOrOrdinal( type_id );
    msheader.Name = WResIDToNameOrOrdinal( res_id );
    msheader.MemoryFlags = flags;
    msheader.Size = loc.len;
    msheader.LanguageId = MAKELANGID( lang->lang, lang->sublang );
    msheader.Version = 0L; /* Currently Unsupported */
    msheader.DataVersion = 0L;
    msheader.Characteristics = 0L; /* Currently Unsupported */
    /*
     * OS/2 resource header happens to be identical to Win16
     */
    error = MResWriteResourceHeader( &msheader, iswin32, CurrResFile.fp );
    MemFree( msheader.Type );
    MemFree( msheader.Name );
    ErrorHasOccured = true;
    if( error ) {
        RcError( ERR_WRITTING_RES, CurrResFile.filename, LastWresErrStr() );
    } else {
        if( tmpResFile == NULL ) {
            RcError( ERR_READING_TMP, tmpResFileName, LastWresErrStr() );
        } else {
            /*
             * copy the data from the temporary file to the RES file
             */
            if( ResSeek( tmpResFile, loc.start, SEEK_SET ) ) {
                RcError( ERR_READING_TMP, tmpResFileName, LastWresErrStr() );
            } else {
                RcStatus    ret;

                ret = CopyFilesData( tmpResFile, CurrResFile.fp, loc.len, buffer, sizeof( buffer ) );
                if( ret == RS_READ_ERROR ) {
                    ret = RS_READ_ERROR_TMP;
                } else if( ret == RS_WRITE_ERROR ) {
                    ret = RS_WRITE_ERROR_RES;
                }
                ErrorHasOccured = RcIOError( ret, tmpResFileName, CurrResFile.filename, errno );
            }
        }
    }
}

void SemAddResource( WResID *res_id, WResID *type_id, ResMemFlags flags, ResLocation loc )
/****************************************************************************************/
{
    SemAddResource2( res_id, type_id, flags, loc, NULL );
}

void SemAddResource2( WResID *res_id, WResID *type_id, ResMemFlags flags,
                ResLocation loc, const char *filename )
/******************************************************************/
{
    bool                error;
    bool                duplicate;
    char                *namestr;
    const WResLangType  *lang;

    lang = SemGetResourceLanguage();

    // Windows 95 is currently unable to load an exe that contains a resource
    // with numeric type or numeric identifier greater than 0x7FFF
    // so we warn the user
    if( CmdLineParms.iswin32 ) {
        if( !type_id->IsName
          && type_id->ID.Num > 0x7FFF ) {
            namestr = WResIDToStr( type_id );
            RcWarning( ERR_TYPE_GT_7FFF, namestr );
            MemFree( namestr );
        }
        if( !res_id->IsName
          && res_id->ID.Num > 0x7FFF ) {
            namestr = WResIDToStr( res_id );
            RcWarning( ERR_NAME_GT_7FFF, namestr );
            MemFree( namestr );
        }
    }
    error = WResAddResource( type_id, res_id, flags, loc.start, loc.len, CurrResFile.dir, lang, &duplicate );

    if( duplicate ) {
        if( filename == NULL ) {
            ReportDupResource( res_id, type_id, NULL, NULL, true );
        } else {
            ReportDupResource( res_id, type_id, filename, CmdLineParms.InFileName, true );
        }
        /*
         * The resource has already been written but we can't add it to
         * directory. This will make the .RES file larger but will otherwise
         * not affect it since there will be no references to the resource in
         * the directory.
         */
    } else if( error ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
    }

    if( !CurrResFile.IsWatcomRes ) {
        if( !duplicate ) {
            copyMSFormatRes( res_id, type_id, flags, loc, lang, CmdLineParms.iswin32 );
        }
        /*
         * erase the temporary RES file
         */
        RCCloseFile( &tmpResFile );
    }
}

void SemanticInitStatics( void )
/******************************/
{
    save_fp = NULL;
    save_name = NULL;
    tmpResFile = NULL;
}
