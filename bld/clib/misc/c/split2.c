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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#if !defined( __WIDECHAR__ ) && !defined( __UNIX__ )
    #include <mbstring.h>
#endif

#if defined(__UNIX__)
  #define IS_PC(x) (x=='/')
#else   /* DOS, OS/2, Windows */
  #define IS_PC(x) (x=='\\'||x=='/')
#endif

/* split full Unix path name into its components */

/* Under Unix we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */


static CHAR_TYPE *pcopy( CHAR_TYPE **pdst, CHAR_TYPE *dst, const CHAR_TYPE *b_src, const CHAR_TYPE *e_src ) {
/*========================================================================*/

    unsigned    len;

    if( pdst == NULL )
        return( dst );
    *pdst = dst;
    len = e_src - b_src;
    if( len >= _MAX_PATH2 ) {
        len = _MAX_PATH2 - 1;
    }
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
    memcpy( dst, b_src, len * CHARSIZE );
    dst[ len ] = NULLCHAR;
    return( dst + len + 1 );
#else
    len = _mbsnccnt( b_src, len );          /* # chars in len bytes */
    _mbsncpy( dst, b_src, len );            /* copy the chars */
    dst[ _mbsnbcnt( dst,len ) ] = NULLCHAR;
    return( dst + _mbsnbcnt( dst, len ) + 1 );
#endif
}

_WCRTLINK void  __F_NAME(_splitpath2,_wsplitpath2)( CHAR_TYPE const *inp, CHAR_TYPE *outp,
                     CHAR_TYPE **drive, CHAR_TYPE **path, CHAR_TYPE **fn, CHAR_TYPE **ext ) {
/*=====================================================================*/

    CHAR_TYPE const *dotp;
    CHAR_TYPE const *fnamep;
    CHAR_TYPE const *startp;
    UINT_WC_TYPE    ch;

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> \\disk2\fred\filename.ext for UNC names */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

    /* process node/drive/UNC specification */
    startp = inp;
    if( IS_PC( inp[ 0 ] ) && IS_PC( inp[ 1 ] ) )
    {
        inp += 2;
        for( ;; ) {
            if( *inp == NULLCHAR )
                break;
            if( IS_PC( *inp ) )
                break;
            if( *inp == '.' )
                break;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
            ++inp;
#else
            inp = _mbsinc( inp );
#endif
        }
        outp = pcopy( drive, outp, startp, inp );
#if !defined(__UNIX__)
    /* process drive specification */
    } else if( inp[ 0 ] != NULLCHAR && inp[ 1 ] == ':' ) {
        if( drive != NULL ) {
            *drive = outp;
            outp[ 0 ] = inp[ 0 ];
            outp[ 1 ] = ':';
            outp[ 2 ] = NULLCHAR;
            outp += 3;
        }
        inp += 2;
#endif
    } else if( drive != NULL ) {
        *drive = outp;
        *outp = NULLCHAR;
        ++outp;
    }

    /* process /user/fred/filename.ext for QNX */
    /* process \fred\filename.ext for DOS, OS/2 */
    /* process /fred/filename.ext for DOS, OS/2 */
    dotp = NULL;
    fnamep = inp;
    startp = inp;

    for( ;; ) {
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        ch = *inp;
#else
        ch = _mbsnextc( inp );
#endif
        if( ch == 0 )
            break;
        if( ch == '.' ) {
            dotp = inp;
            ++inp;
            continue;
        }
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        inp++;
#else
        inp = _mbsinc( inp );
#endif
        if( IS_PC( ch ) ) {
            fnamep = inp;
            dotp = NULL;
        }
    }
    outp = pcopy( path, outp, startp, fnamep );
    if( dotp == NULL )
        dotp = inp;
    outp = pcopy( fn, outp, fnamep, dotp );
    outp = pcopy( ext, outp, dotp, inp );
}
