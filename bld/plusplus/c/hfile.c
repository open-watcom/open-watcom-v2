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
#include <ctype.h>
#include "plusplus.h"
#include "errdefns.h"
#include "memmgr.h"
#include "hfile.h"
#include "iosupp.h"
#include "initdefs.h"

static char *hfile_list;        // list of H files
static char *hfile_ptr;         // current position in list


int HFileListSize(              // RETURN AN APPROXIMATE SIZE OF THE H-FILE LIST
    void )
{
    if( hfile_ptr != NULL ) {
        return strlen( hfile_ptr ) + 2;
    }
    return 0;
}


void HFileListStart(            // START PROCESSING H-FILE LIST
    void )
{
    hfile_ptr = hfile_list;
}


void HFileListNext(             // GET NEXT H-FILE PREFIX
    char *prefix )              // - prefix location
{
    if( hfile_ptr != NULL ) {
        while( isspace( *hfile_ptr ) ) {
            ++hfile_ptr;
        }
        hfile_ptr = IoSuppIncPathElement( hfile_ptr, prefix );
    } else {
        *prefix = '\0';
    }
}


void HFileAppend(               // APPEND HFILE TO LIST
    const char *filename,       // - file name
    size_t len )                // - size of name
{
    int old_len;                // - length of old H list
    char *p;                    // - points into H list
    char *old_list;             // - old H list

    if( hfile_list != NULL ) {
        old_list = hfile_list;
        old_len = strlen( old_list );
        p = (char *) CMemAlloc( len + old_len + 2 );
        hfile_list = p;
        p = stpcpy( p, old_list );
        p = IoSuppAddIncPathSep( p );
        CMemFree( old_list );
    } else {
        p = (char *) CMemAlloc( len + 1 );
        hfile_list = p;
    }
    p = stvcpy( p, filename, len );
}


static void hfileInit(          // INITIALIZE FOR H-FILE SUPPORT
    INITFINI* defn )            // - definition
{
    defn = defn;
    hfile_list = NULL;
}


static void hfileFini(          // FREE HFILE LIST
    INITFINI* defn )            // - definition
{
    defn = defn;
    CMemFreePtr( &hfile_list );
}


INITDEFN( h_files, hfileInit, hfileFini )
