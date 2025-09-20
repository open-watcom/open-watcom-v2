/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  _fullpath implementation for RDOS.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <direct.h>
#include <rdos.h>
#include "liballoc.h"
#include "thread.h"
#include "pathmac.h"


#define _WILL_FIT( c )  if(( (c) + 1 ) > size ) {       \
                            lib_set_errno( ERANGE );        \
                            return( NULL );             \
                        }                               \
                        size -= (c);


_WCRTLINK char *_fullpath( char *buff, const char *path, size_t size )
{
    const char  *p;
    char        *q;
    size_t      len;
    unsigned    path_drive_idx;
    char        curr_dir[_MAX_PATH];

    if( buff == NULL ) {
        size = _MAX_PATH;
        buff = lib_malloc( size );
        if( buff == NULL ) {
            lib_set_errno( ENOMEM );
            return( NULL );
        }
    }
    if( path == NULL || path[0] == NULLCHAR ) {
        return( getcwd( buff, size ) );
    }

    p = path;
    q = buff;
    _WILL_FIT( 2 );
    if( HAS_DRIVE( p ) ) {
        path_drive_idx = ( tolower( (unsigned char)p[0] ) - 'a' );
        q[0] = p[0];
        q[1] = p[1];
        p += 2;
    } else {
        path_drive_idx = RdosGetCurDrive();
        q[0] = 'A' + path_drive_idx;
        q[1] = DRV_SEP;
    }
    q += 2;
    if( !IS_DIR_SEP( p[0] ) ) {
        if( !RdosGetCurDir( path_drive_idx, curr_dir ) ) {
            lib_set_errno( ENOENT );
            return( NULL );
        }
        len = strlen( curr_dir );
        if( curr_dir[0] != DIR_SEP ) {
            _WILL_FIT( 1 );
            *(q++) = DIR_SEP;
        }
        _WILL_FIT( len );
        strcpy( q, curr_dir );
        q += len;
        if( q[-1] != DIR_SEP ) {
            _WILL_FIT( 1 );
            *(q++) = DIR_SEP;
        }
        for( ; p[0] != NULLCHAR; ) {
            if( p[0] != '.' ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;
            if( IS_DIR_SEP( p[0] ) ) {
                /* ignore "./" in directory specs */
                if( !IS_DIR_SEP( q[-1] ) ) {
                    *q++ = DIR_SEP;
                }
                ++p;
                continue;
            }
            if( p[0] == NULLCHAR )
                break;
            if( p[0] == '.' ) {
                if( IS_DIR_SEP( p[1] ) )
                    p += 2;
                else {
                    p += 1;
                    if( p[0] != NULLCHAR )
                        return( NULL );
                }

                /* go up a directory for a "../" */
                if( !IS_DIR_SEP( q[-1] ) ) {
                    return( NULL );
                }
                q -= 2;
                for( ;; ) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( IS_DIR_SEP( *q ) )
                        break;
                    if( *q == DRV_SEP ) {
                        ++q;
                        *q = DIR_SEP;
                        break;
                    }
                    --q;
                }
                ++q;
                *q = NULLCHAR;
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = '.';
        }
        *q = NULLCHAR;
    } else {
        len = strlen( p );
        _WILL_FIT( len );
        strcpy( q, p );
    }
    /* force to all backslashes */
    for( q = buff; *q != NULLCHAR; ++q ) {
        if( *q == ALT_DIR_SEP ) {
            *q = DIR_SEP;
        }
    }
    return( buff );
}
