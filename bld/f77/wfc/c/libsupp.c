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


//
// LIBSUPP   : Library support
//

#include "ftnstd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "global.h"
#include "omodes.h"
#include "inout.h"
#include "iopath.h"
#include "mkname.h"
#include "filescan.h"
#include "sdcio.h"
#include "libsupp.h"

#include "clibext.h"


extern  char            ForExtn[];

static  lib_handle FindSrcFile( char *fname ) {
//=============================================

// Find a source file.

    lib_handle  lp;

    MakeName( fname, SDSrcExtn( fname ), fname );
    lp = SDOpen( fname, READ_FILE );
    if( lp != NULL ) {
        SrcInclude( fname );
    }
    return( lp );
}


static lib_handle SearchPath( char *path_list, char *name ) {
//===========================================================

    char        *p;
    lib_handle  lp;
    char        buff[2 * _MAX_PATH];
    char        c;

    lp = NULL;
    while( (c = *path_list) != '\0' ) {
        p = buff;
        do {
            ++path_list;
            if( IS_PATH_LIST_SEP( c ) )
                break;
            *p++ = c;
        } while( (c = *path_list) != '\0' );
        c = p[-1];
        if( !IS_PATH_SEP( c ) ) {
            *p++ = DIR_SEP;
        }
        strcpy( p, name );
        lp = FindSrcFile( buff );
        if( lp != NULL ) {
            break;
        }
    }
    return( lp );
}

lib_handle      IncSearch( char *name ) {
//=======================================

// Search for a library member.

    lib_handle  lp;

    lp = NULL;
    if( IncludePath != NULL ) {
        lp = SearchPath( IncludePath, name );
    }
    if( lp == NULL && FIncludePath != NULL ) {
        lp = SearchPath( FIncludePath, name );
    }
    return( lp );
}


int     LibRead( lib_handle lp ) {
//================================

// Read a record from a library member (source only).

    return( SDRead( lp, SrcBuff, SRCLEN ) );
}


bool    LibEof( lib_handle lp ) {
//===============================

// Check for EOF on library read (source only).

    return( SDEof( lp ) );
}


bool    LibError( lib_handle lp, char *buff ) {
//=============================================

// Check for error on library read (source only).

    return( SDError( lp, buff ) );
}


void    IncMemClose( lib_handle lp ) {
//====================================

// Close a library member that was included (source only).

    SDClose( lp );
}
