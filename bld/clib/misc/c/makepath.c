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

#undef _makepath

#if defined(__PENPOINT__)
  #define PC '\\'
#elif defined(__QNX__)
  #define PC '/'
#else   /* DOS, OS/2, Windows, Netware */
  #define PC '\\'
  #define ALT_PC '/'
#endif


#if defined(__PENPOINT__) || defined(__QNX__)

/* create full PENPOINT/QNX path name from the components */

_WCRTLINK void __F_NAME(_makepath,_wmakepath)(
        CHAR_TYPE           *path,
        const CHAR_TYPE  *node,
        const CHAR_TYPE  *dir,
        const CHAR_TYPE  *fname,
        const CHAR_TYPE  *ext )
{
#if !defined(__WIDECHAR__) && !defined(__QNX__)
    char    *pathstart = path;
#endif
    *path = '\0';

    if( node != NULL ) {
        if( *node != '\0' ) {
            __F_NAME(strcpy,wcscpy)( path, node );
            path = __F_NAME(strchr,wcschr)( path, NULLCHAR );

            /* if node did not end in '/' then put in a provisional one */
            #ifdef __WIDECHAR__
                if( path[-1] == PC )
                    path--;
                else
                    *path = PC;
            #else
                #ifdef __QNX__
                    if( path[-1] == PC ) {
                        path--;
                    } else {
                        *path = PC;
                    }
                #else
                    if( *(_mbsdec(pathstart,path)) == PC )
                        path--;
                    else
                        *path = PC;
                #endif
            #endif
        }
    }
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            /*  if dir does not start with a '/' and we had a node then
                    stick in a separator
            */
            if( (*dir != PC) && (*path == PC) ) path++;

            __F_NAME(strcpy,wcscpy)( path, dir );
            path = __F_NAME(strchr,wcschr)( path, NULLCHAR );

            /* if dir did not end in '/' then put in a provisional one */
            #ifdef __WIDECHAR__
                if( path[-1] == PC )
                    path--;
                else
                    *path = PC;
            #else
                #ifdef __QNX__
                    if( path[-1] == PC ) {
                        path--;
                    } else {
                        *path = PC;
                    }
                #else
                    if( *(_mbsdec(pathstart,path)) == PC )
                        path--;
                    else
                        *path = PC;
                #endif
            #endif
        }
    }

    if( fname != NULL ) {
        if( (*fname != PC) && (*path == PC) ) path++;

        __F_NAME(strcpy,wcscpy)( path, fname );
        path = __F_NAME(strchr,wcschr)( path, NULLCHAR );

    } else {
        if( *path == PC ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            __F_NAME(strcpy,wcscpy)( path, ext );
            path = __F_NAME(strchr,wcschr)( path, NULLCHAR );
        }
    }
    *path = '\0';
}

#elif defined( __NETWARE__ )

/*
    For silly two choice DOS path characters / and \,
    we want to return a consistent path character.
*/

static char pickup( char c, char *pc_of_choice )
{
    if( c == PC || c == ALT_PC ) {
        if( *pc_of_choice == '\0' ) *pc_of_choice = c;
        c = *pc_of_choice;
    }
    return( c );
}

_WCRTLINK extern void _makepath( char *path, const char *volume,
                const char *dir, const char *fname, const char *ext )
{
    char first_pc = '\0';

    if( volume != NULL ) {
        if( *volume != '\0' ) {
            do {
                *path++ = *volume++;
            } while( *volume != '\0' );
            if( path[ -1 ] != ':' ) {
                *path++ = ':';
            }
        }
    }
    *path = '\0';
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            do {
                *path++ = pickup( *dir++, &first_pc );
            } while( *dir != '\0' );
            /* if no path separator was specified then pick a default */
            if( first_pc == '\0' ) first_pc = PC;
            /* if dir did not end in path sep then put in a provisional one */
            if( path[-1] == first_pc ) {
                path--;
            } else {
                *path = first_pc;
            }
        }
    }
    /* if no path separator was specified thus far then pick a default */
    if( first_pc == '\0' ) first_pc = PC;
    if( fname != NULL ) {
        if( (pickup( *fname, &first_pc ) != first_pc)
            && (*path == first_pc) ) path++;
        while( *fname != '\0' ) *path++ = pickup( *fname++, &first_pc );
    } else {
        if( *path == first_pc ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            while( *ext != '\0' ) *path++ = *ext++;
        }
    }
    *path = '\0';
}

#else

/*
    For silly two choice DOS path characters / and \,
    we want to return a consistent path character.
*/

static unsigned pickup( unsigned c, unsigned *pc_of_choice )
{
    if( c == PC || c == ALT_PC ) {
        if( *pc_of_choice == '\0' ) *pc_of_choice = c;
        c = *pc_of_choice;
    }
    return( c );
}

/* create full MS-DOS path name from the components */

_WCRTLINK void __F_NAME(_makepath,_wmakepath)( CHAR_TYPE *path, const CHAR_TYPE *drive,
                const CHAR_TYPE *dir, const CHAR_TYPE *fname, const CHAR_TYPE *ext )
{
    unsigned            first_pc = '\0';
#ifndef __WIDECHAR__
    char *              pathstart = path;
    unsigned            ch;
#endif

    if( drive != NULL ) {
        if( *drive != '\0' ) {
            if ((*drive == '\\') && (drive[1] == '\\')) {
                __F_NAME(strcpy, wcscpy)(path, drive);
                path += __F_NAME(strlen, wcslen)(drive);
            } else {
                *path++ = *drive;                               /* OK for MBCS */
                *path++ = ':';
            }
        }
    }
    *path = '\0';
    if( dir != NULL ) {
        if( *dir != '\0' ) {
            do {
                #ifdef __WIDECHAR__
                    *path++ = pickup( *dir++, &first_pc );
                #else
                    #ifdef __QNX__
                        *path++ = pickup( *dir++, &first_pc );
                    #else
                        ch = pickup( _mbsnextc(dir), &first_pc );
                        _mbvtop( ch, path );
                        path[_mbclen(path)] = '\0';
                        path = _mbsinc( path );
                        dir = _mbsinc( dir );
                    #endif
                #endif
            } while( *dir != '\0' );
            /* if no path separator was specified then pick a default */
            if( first_pc == '\0' ) first_pc = PC;
            /* if dir did not end in '/' then put in a provisional one */
            #ifdef __WIDECHAR__
                if( path[-1] == first_pc )
                    path--;
                else
                    *path = first_pc;
            #else
                #ifdef __QNX__
                    if( path[-1] == PC ) {
                        path--;
                    } else {
                        *path = first_pc;
                    }
                #else
                    if( *(_mbsdec(pathstart,path)) == first_pc )
                        path--;
                    else
                        *path = first_pc;
                #endif
            #endif
        }
    }

    /* if no path separator was specified thus far then pick a default */
    if( first_pc == '\0' ) first_pc = PC;
    if( fname != NULL ) {
        #ifdef __WIDECHAR__
            if( pickup(*fname,&first_pc) != first_pc  &&  *path == first_pc )
                path++;
        #else
            #ifdef __QNX__
                if( pickup(*fname,&first_pc) != first_pc  &&  *path == first_pc )
                    path++;
            #else
                ch = _mbsnextc( fname );
                if( pickup(ch,&first_pc) != first_pc  &&  *path == first_pc )
                    path++;
            #endif
        #endif

        while (*fname != '\0')
        {
        //do {
            #ifdef __WIDECHAR__
                *path++ = pickup( *fname++, &first_pc );
            #else
                #ifdef __QNX__
                    *path++ = pickup( *fname++, &first_pc );
                #else
                    ch = pickup( _mbsnextc(fname), &first_pc );
                    _mbvtop( ch, path );
                    path[_mbclen(path)] = '\0';
                    path = _mbsinc( path );
                    fname = _mbsinc( fname );
                #endif
            #endif
        } //while( *fname != '\0' );
    } else {
        if( *path == first_pc ) path++;
    }
    if( ext != NULL ) {
        if( *ext != '\0' ) {
            if( *ext != '.' )  *path++ = '.';
            while( *ext != '\0' ) *path++ = *ext++;     /* OK for MBCS */
        }
    }
    *path = '\0';
}
#endif
