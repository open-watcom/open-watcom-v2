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


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#ifndef __WIDECHAR__
    #include <mbstring.h>
#endif

#if defined(__QNX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows */
  #define PC '\\'
  #define ALT_PC '/'
#endif

// include <sys/types.h>
// include <limits.h>

#if defined(__NETWARE__)
// this should be in novh\stdlib.h
#define _MAX_PATH2 (_MAX_PATH+3)
#endif

/* split full QNX path name into its components */

/* Under QNX we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */


static CHAR_TYPE *pcopy( CHAR_TYPE **pdst, CHAR_TYPE *dst, const CHAR_TYPE *b_src, const CHAR_TYPE *e_src ) {
/*========================================================================*/

    unsigned    len;

    if( pdst == NULL ) return( dst );
    *pdst = dst;
    len = e_src - b_src;
    if( len >= _MAX_PATH2 ) {
        len = _MAX_PATH2 - 1;
    }
    #ifdef __WIDECHAR__
        memcpy( dst, b_src, len * CHARSIZE );
        dst[ len ] = NULLCHAR;
        return( dst + len + 1 );
    #else
        #ifdef __QNX__
            memcpy( dst, b_src, len*CHARSIZE );
            dst[len] = NULLCHAR;
            return( dst + len + 1 );
        #else
            len = _mbsnccnt( b_src, len );          /* # chars in len bytes */
            _mbsncpy( dst, b_src, len );            /* copy the chars */
            dst[ _mbsnbcnt(dst,len) ] = NULLCHAR;
            return( dst + _mbsnbcnt(dst,len) + 1 );
        #endif
    #endif
}

_WCRTLINK void  __F_NAME(_splitpath2,_wsplitpath2)( CHAR_TYPE const *inp, CHAR_TYPE *outp,
                     CHAR_TYPE **drive, CHAR_TYPE **path, CHAR_TYPE **fn, CHAR_TYPE **ext ) {
/*=====================================================================*/

    CHAR_TYPE const *dotp;
    CHAR_TYPE const *fnamep;
    CHAR_TYPE const *startp;
#ifndef __WIDECHAR__
    unsigned        ch;
#else
    CHAR_TYPE       ch;
#endif

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> \\disk2\fred\filename.ext for UNC names */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

    /* process node/drive/UNC specification */
    startp = inp;
    #ifdef __QNX__
        if( inp[0] == PC  &&  inp[1] == PC )
    #else
        if( (inp[0] == PC || inp[0] == ALT_PC)
         && (inp[1] == PC || inp[1] == ALT_PC) )
    #endif
    {
        inp += 2;
        for( ;; ) {
            if( *inp == NULLCHAR ) break;
            if( *inp == PC ) break;
            #ifndef __QNX__
                if( *inp == ALT_PC ) break;
            #endif
            if( *inp == '.' ) break;
            #ifdef __WIDECHAR__
                ++inp;
            #else
                #ifdef __QNX__
                    inp++;
                #else
                    inp = _mbsinc( inp );
                #endif
            #endif
        }
        outp = pcopy( drive, outp, startp, inp );
#if !defined(__QNX__)
    /* processs drive specification */
    } else if( inp[0] != NULLCHAR && inp[1] == ':' ) {
        if( drive != NULL ) {
            *drive = outp;
            outp[0] = inp[0];
            outp[1] = ':';
            outp[2] = NULLCHAR;
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

    for(;;) {
        #ifdef __WIDECHAR__
            ch = *inp;
        #else
            #ifdef __QNX__
                ch = *inp;
            #else
                ch = _mbsnextc( inp );
            #endif
        #endif
        if( ch == 0 ) break;
        if( ch == '.' ) {
            dotp = inp;
            ++inp;
            continue;
        }
        #ifdef __WIDECHAR__
            inp++;
        #else
            #ifdef __QNX__
                inp++;
            #else
                inp = _mbsinc( inp );
            #endif
        #endif
#if defined(__QNX__)
        if( ch == PC ) {
#else /* DOS, OS/2, Windows */
        if( ch == PC  ||  ch == ALT_PC ) {
#endif
            fnamep = inp;
            dotp = NULL;
        }
    }
    outp = pcopy( path, outp, startp, fnamep );
    if( dotp == NULL ) dotp = inp;
#if defined(__QNX__)
    if( ext == NULL )  dotp = inp;
#endif
    outp = pcopy( fn, outp, fnamep, dotp );
    outp = pcopy( ext, outp, dotp, inp );
}
