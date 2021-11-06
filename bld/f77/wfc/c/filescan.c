/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
// FILESCAN     : file name processing
//

#include "ftnstd.h"
#include "filescan.h"


static char         ForExtn[] = { "for" };

char *SDFName( const char *fn )
//=============================
{
    char        *start;
    char        chr;

#if ! defined( __UNIX__ )
    if( fn[ 1 ] == ':' ) {
        fn += 2;
    }
#endif
    for(;;) {
        start = (char *)fn;
        for(;;) {
            chr = *fn;
            if( chr == NULLCHAR )
                return( start );
            fn++;
#if ! defined( __UNIX__ )
            if( chr == '\\' )
                break;
#endif
            if( chr == '/' ) {
                break;
            }
        }
    }
}


char *SDSplitExtn( char *fn, char *default_extn )
//===============================================
{
    char        *src_extn;
    char        chr;

    fn = SDFName( fn );
    src_extn = NULL;
    for(;;) {
        chr = *fn;
        if( chr == NULLCHAR )
            break;
        if( chr == '.' ) {
            src_extn = fn;
        }
        fn++;
    }
    if( src_extn == NULL )
        return( default_extn );
    *src_extn = NULLCHAR;
    ++src_extn;
    return( src_extn );
}


char *SDSplitSrcExtn( char *fn )
//==============================
{
    return( SDSplitExtn( fn, ForExtn ) );
}
