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
#include "global.h"
#include "omodes.h"
#include "inout.h"
#include "iopath.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define LIBRARY_SEP ';'

extern  void            SDClose(file_handle);
extern  file_handle     SDOpen(char *,int);
extern  uint            SDRead(file_handle,char *,uint);
extern  bool            SDError(file_handle,char *);
extern  bool            SDEof(file_handle);
extern  char            *SDSrcExtn(char *);
extern  int             MakeName(char *,char *,char *);
extern  int             CopyMaxStr(char *,char *,int);

extern  char            ForExtn[];

static  char            LibEnvStr[] = { "FINCLUDE" };


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


static  int     Combine( char *path, char *name, char *buff, int buff_len ) {
//===========================================================================

// Combine the path and filename.

    int         len;

    len = CopyMaxStr( path, buff, buff_len );
    if( len > 0 && !IS_PATH_SEP( buff[len - 1] ) ) {
        buff[len++] = DIR_SEP;
    }
    len += CopyMaxStr( name, buff + len, buff_len - len );
    return( len );
}


static  lib_handle SearchDir( char *path, char *name ) {
//======================================================

// Search a directory for a source file.

    char        fname[_MAX_PATH];

    fname[Combine( path, name, fname, _MAX_PATH - 1 )] = NULLCHAR;
    return( FindSrcFile( fname ) );
}


static  bool    ExtractName( char **lib ) {
//=========================================

// Extract a file name from library specification.

    char        *ptr;
    bool        last;

    last = TRUE;
    ptr = *lib;
    while( *ptr != NULLCHAR ) {
        if( IS_INCL_SEP( *ptr ) ) {
            last = FALSE;
            break;
        }
        ptr++;
    }
    *lib = ptr;
    for(;;) {
        ptr--;
        if( *ptr != ' ' ) break;
    }
    ptr++;
    *ptr = NULLCHAR;
    return( last );
}


static lib_handle SearchPath( char *path, char *name ) {
//======================================================

    char        *ptr;
    lib_handle  lp;
    bool        last;

    lp = NULL;
    ptr = alloca( strlen( path ) + sizeof( char ) );
    if( ptr != NULL ) {
        strcpy( ptr, path );
        for(;;) {
            path = ptr;
            last = ExtractName( &ptr );
            lp = SearchDir( path, name );
            if( lp != NULL ) break;
            if( last ) break;
            ptr++; // skip the LIBRARY_SEP
        }
    }
    return( lp );
}


lib_handle      IncSearch( char *name ) {
//=======================================

// Search for a library member.

    lib_handle  lp;
    char        *path;

    lp = NULL;
    if( IncludePath != NULL ) {
        lp = SearchPath( IncludePath, name );
        if( lp != NULL ) return( lp );
    }
    path = getenv( LibEnvStr );
    if( path != NULL ) {
        lp = SearchPath( path, name );
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


