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
* Description:  Platform independent _splitpath() implementation.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#if !defined( __WIDECHAR__ ) && !defined( __UNIX__ )
    #include <mbstring.h>
#endif

#undef _splitpath

#if defined(__UNIX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows, Netware */
  #define PC '\\'
  #define ALT_PC '/'
#endif

#ifdef __NETWARE__
  #undef _MAX_PATH
  #undef _MAX_SERVER
  #undef _MAX_VOLUME
  #undef _MAX_DRIVE
  #undef _MAX_DIR
  #undef _MAX_FNAME
  #undef _MAX_EXT
  #undef _MAX_NAME

  #define _MAX_PATH    255 /* maximum length of full pathname */
  #define _MAX_SERVER  48  /* maximum length of server name */
  #define _MAX_VOLUME  16  /* maximum length of volume component */
  #define _MAX_DRIVE   3   /* maximum length of drive component */
  #define _MAX_DIR     255 /* maximum length of path component */
  #define _MAX_FNAME   9   /* maximum length of file name component */
  #define _MAX_EXT     5   /* maximum length of extension component */
  #define _MAX_NAME    13  /* maximum length of file name (with extension) */
#endif


static void copypart( CHAR_TYPE *buf, const CHAR_TYPE *p, int len, int maxlen )
{
    if( buf != NULL ) {
        if( len > maxlen )
            len = maxlen;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        memcpy( buf, p, len * CHARSIZE );
        /*strncpy( buf, p, len ); */
        buf[ len ] = NULLCHAR;
#else
        len = _mbsnccnt( (unsigned char *)p, len );                 /* # chars in len bytes */
        _mbsncpy( (unsigned char *)buf, (unsigned char *)p, len );  /* copy the chars */
        buf[_mbsnbcnt( (unsigned char *)buf, len )] = NULLCHAR;
#endif
    }
}

// include <sys/types.h>
// include <limits.h>

#if !defined(_MAX_NODE)
#define _MAX_NODE   _MAX_DRIVE  /*  maximum length of node name w/ '\0' */
#endif

/* split full QNX path name into its components */

/* Under QNX we will map drive to node, dir to dir, and
 * filename to (filename and extension)
 *          or (filename) if no extension requested.
 */

/* Under Netware, 'drive' maps to 'volume' */

_WCRTLINK void __F_NAME(_splitpath,_wsplitpath)( const CHAR_TYPE *path,
    CHAR_TYPE *drive, CHAR_TYPE *dir, CHAR_TYPE *fname, CHAR_TYPE *ext )
{
    const CHAR_TYPE *dotp;
    const CHAR_TYPE *fnamep;
    const CHAR_TYPE *startp;
    UINT_WC_TYPE    ch;
#ifdef __NETWARE__
    const CHAR_TYPE *ptr;
#endif

    /* take apart specification like -> //0/hd/user/fred/filename.ext for QNX */
    /* take apart specification like -> c:\fred\filename.ext for DOS, OS/2 */

#if defined(__UNIX__)

    /* process node/drive specification */
    startp = path;
    if( path[ 0 ] == PC && path[ 1 ] == PC ) {
        path += 2;
        for( ;; ) {
            if( *path == NULLCHAR )
                break;
            if( *path == PC )
                break;
            if( *path == '.' )
                break;
            ++path;
        }
    }
    copypart( drive, startp, path - startp, _MAX_NODE );

#elif defined(__NETWARE__)

  #ifdef __WIDECHAR__
        ptr = wcschr( path, ':' );
  #else
        ptr = _mbschr( (unsigned char *)path, ':' );
  #endif
    if( ptr != NULL ) {
        if( drive != NULL ) {
            copypart( drive, path, ptr - path + 1, _MAX_SERVER +
                      _MAX_VOLUME + 1 );
        }
  #if defined( __WIDECHAR__ )
        path = ptr + 1;
  #else
        path = _mbsinc( ptr );
  #endif
    } else if( drive != NULL ) {
        *drive = '\0';
    }

#else

    /* processs drive specification */
    if( path[ 0 ] != NULLCHAR  &&  path[ 1 ] == ':' ) {
        if( drive != NULL ) {
            drive[ 0 ] = path[ 0 ];
            drive[ 1 ] = ':';
            drive[ 2 ] = NULLCHAR;
        }
        path += 2;
    } else if( drive != NULL ) {
        drive[ 0 ] = NULLCHAR;
    }

#endif

    /* process /user/fred/filename.ext for QNX */
    /* process /fred/filename.ext for DOS, OS/2 */
    dotp = NULL;
    fnamep = path;
    startp = path;

    for( ;; ) {         /* 07-jul-91 DJG -- save *path in ch for speed */
        if( *path == NULLCHAR )
            break;
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        ch = *path;
#else
        ch = _mbsnextc( (unsigned char *)path );
#endif
        if( ch == '.' ) {
            dotp = path;
            ++path;
            continue;
        }
#if defined( __WIDECHAR__ ) || defined( __UNIX__ )
        ++path;
#else
        path = (char *)_mbsinc( (unsigned char *)path );
#endif
#if defined(__UNIX__)
        if( ch == PC ) {
#else /* DOS, OS/2, Windows, Netware */
        if( ch == PC || ch == ALT_PC ) {
#endif
            fnamep = path;
            dotp = NULL;
        }
    }
    copypart( dir, startp, fnamep - startp, _MAX_DIR - 1 );
    if( dotp == NULL )
        dotp = path;
    copypart( fname, fnamep, dotp - fnamep, _MAX_FNAME - 1 );
    copypart( ext,   dotp,   path - dotp,   _MAX_EXT - 1);
}
