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
// FILESCAN     : file name processing
//

#include "ftnstd.h"
#include "sdfile.h"

extern  char            ForExtn[];


char    *SDFName( char *fn ) {
//============================

    char        *start;
    char        chr;

#if ! defined( __UNIX__ )
    if( fn[ 1 ] == ':' ) {
        fn += 2;
    }
#endif
    for(;;) {
        start = fn;
        for(;;) {
            chr = *fn;
            if( chr == NULLCHAR ) return( start );
            fn++;
#if ! defined( __UNIX__ )
            if( chr == '\\' ) break;
#endif
            if( chr == '/' ) break;
        }
    }
}


char    *SDExtn( char *fn, char *default_extn ) {
//===============================================

    char        *src_extn;
    char        chr;

    src_extn = NULL;
    fn = SDFName( fn );
    for(;;) {
        chr = *fn;
        if( chr == NULLCHAR ) break;
        if( chr == EXTN_MARKER ) {
            src_extn = fn;
        }
        fn++;
    }
    if( src_extn == NULL ) return( default_extn );
    *src_extn = NULLCHAR;
    ++src_extn;
    return( src_extn );
}


char    *SDSrcExtn( char *fn ) {
//==============================

    return( SDExtn( fn, ForExtn ) );
}
