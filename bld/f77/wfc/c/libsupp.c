/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


//
// LIBSUPP   : Library support
//

#include "ftnstd.h"
#include "global.h"
#include "inout.h"
#include "iopath.h"
#include "mkname.h"
#include "filescan.h"
#include "sdcio.h"
#include "libsupp.h"

#include "clibext.h"


static  file_handle FindSrcFile( char *fname ) {
//=============================================

// Find a source file.

    file_handle  fp;

    MakeName( fname, SDSplitSrcExtn( fname ), fname );
    fp = SDOpenText( fname, "rt" );
    if( fp != NULL ) {
        SrcInclude( fname );
    }
    return( fp );
}


static file_handle doSearchPath( char *path_list, const char *name )
//==================================================================
{
    char        *p;
    file_handle fp;
    char        buff[2 * _MAX_PATH];
    char        c;

    fp = NULL;
    while( (c = *path_list) != NULLCHAR ) {
        p = buff;
        do {
            ++path_list;
            if( IS_PATH_LIST_SEP( c ) )
                break;
            *p++ = c;
        } while( (c = *path_list) != NULLCHAR );
        c = p[-1];
        if( !IS_PATH_SEP( c ) ) {
            *p++ = DIR_SEP;
        }
        strcpy( p, name );
        fp = FindSrcFile( buff );
        if( fp != NULL ) {
            break;
        }
    }
    return( fp );
}

file_handle IncSearch( const char *name )
//=======================================
// Search for a library member.
{
    file_handle fp;

    fp = NULL;
    if( IncludePath != NULL ) {
        fp = doSearchPath( IncludePath, name );
    }
    if( fp == NULL && FIncludePath != NULL ) {
        fp = doSearchPath( FIncludePath, name );
    }
    return( fp );
}
