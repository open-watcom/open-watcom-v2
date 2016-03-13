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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <direct.h>
#include "rterrno.h"
#include "tinyio.h"
#include "ostype.h"
#include "liballoc.h"
#include "pathmac.h"


extern void __GetFullPathName( char *buff, const char *path, size_t size );
#pragma aux __GetFullPathName = \
        "mov    AH,60h" \
        _INT_21         \
        parm [ebx] [edx] [ecx];

#define _WILL_FIT( c )  if(( (c) + 1 ) > size ) {       \
                            _RWD_errno = ERANGE;        \
                            return( NULL );             \
                        }                               \
                        size -= (c);

_WCRTLINK char *_fullpath( char *buff, const char *path, size_t size )
{
    const char  *p;
    char        *q;
    size_t      len;
    tiny_ret_t  rc;
    unsigned    path_drive_idx;
    char        curr_dir[_MAX_PATH];

    if( buff == NULL ) {
        size = _MAX_PATH;
        buff = lib_malloc( size );
        if( buff == NULL ) {
            _RWD_errno = ENOMEM;
            return( NULL );
        }
    }
    if( path == NULL || path[0] == NULLCHAR ) {
        return( getcwd( buff, size ) );
    }
    if( __OS == OS_OS2 || __OS == OS_NT ) {
        __GetFullPathName( buff, path, size );
    } else {
        p = path;
        q = buff;
        _WILL_FIT( 2 );
        if( HAS_DRIVE( p ) ) {
            path_drive_idx = ( tolower( (unsigned char)p[0] ) - 'a' ) + 1;
            q[0] = p[0];
            q[1] = p[1];
            p += 2;
        } else {
            path_drive_idx = TinyGetCurrDrive() + 1;
            q[0] = 'A' + ( path_drive_idx - 1 );
            q[1] = DRV_SEP;
        }
        q += 2;
        if( !IS_DIR_SEP( p[0] ) ) {
            rc = TinyGetCWDir( curr_dir, path_drive_idx );
            if( TINY_ERROR( rc ) ) {
                _RWD_errno = ENOENT;
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
                if( p[0] == '.' && IS_DIR_SEP( p[1] ) ) {
                    /* go up a directory for a "../" */
                    p += 2;
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
    }
    return( buff );
}
