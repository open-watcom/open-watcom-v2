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


#include "plusplus.h"

#include <ctype.h>

#include "memmgr.h"
#include "hfile.h"
#include "iosupp.h"
#include "initdefs.h"
#include "iopath.h"

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
    char    c;

    if( hfile_ptr != NULL ) {
        while( (c = *hfile_ptr) != '\0' ) {
            ++hfile_ptr;
            if( IS_PATH_LIST_SEP( c ) ) 
                break;
            *prefix++ = c;
        }
    }
    *prefix = '\0';
}


void HFileAppend(               // APPEND HFILE TO LIST
    const char *path_list )     // - list of path names
{
    int old_len;                // - length of old H list
    char *p;                    // - points into H list
    char *old_list;             // - old H list
    int len;

    len = strlen( path_list );
    if( len != 0 ) {
        if( hfile_list != NULL ) {
            old_list = hfile_list;
            old_len = strlen( old_list );
            hfile_list = CMemAlloc( old_len + 1 + len + 1 );
            memcpy( hfile_list, old_list, old_len );
            CMemFree( old_list );
            p = hfile_list + old_len;
        } else {
            p = hfile_list = CMemAlloc( len + 1 );
        }
        while( *path_list != '\0' ) {
            if( p != hfile_list )
                *p++ = PATH_LIST_SEP;
            path_list = IoSuppIncPathElement( path_list, p );
            p += strlen( p );
        }
    }
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
