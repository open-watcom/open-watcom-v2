/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "specio.h"

/*
 * ReadDataFile - do just that
 */
#ifdef VICOMP
vi_rc ReadDataFile( const char *file, char **buffer, bool (*fn_alloc)(int), bool (*fn_save)(int, const char *) )
#else
vi_rc ReadDataFile( const char *file, char **buffer, bool (*fn_alloc)(int), bool (*fn_save)(int, const char *), bool bounddata )
#endif
{
    GENERIC_FILE        gf;
    int                 i, dcnt, len, size;
    char                token[MAX_STR], buff[MAX_STR];
    char                *buffdata;
    bool                hasvals;
    const char          *ptr;

    /*
     * get file and buffer
     */
#ifdef VICOMP
    if( !SpecialOpen( file, &gf ) ) {
#else
    if( !SpecialOpen( file, &gf, bounddata ) ) {
#endif
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * get counts
     */
    if( SpecialFgets( buff, sizeof( buff ) - 1, &gf ) ) {
        SpecialFclose( &gf );
        return( ERR_INVALID_DATA_FILE );
    }
    dcnt = atoi( buff );
    hasvals = fn_alloc( dcnt );
    buffdata = NULL;
    ptr = NULL;
    size = 0;

    /*
     * read all tokens
     */
    for( i = 0; i < dcnt; i++ ) {
        if( SpecialFgets( buff, sizeof( buff ) - 1, &gf ) ) {
            SpecialFclose( &gf );
            return( ERR_INVALID_DATA_FILE );
        }
        if( hasvals ) {
            ptr = GetNextWord1( buff, token );
            if( *token == '\0' ) {
                SpecialFclose( &gf );
                return( ERR_INVALID_DATA_FILE );
            }
        } else {
            strcpy( token, buff );
        }
        len = strlen( token );
        buffdata = MemReAlloc( buffdata, size + len + 1 );
        memcpy( buffdata + size, token, len );
        size += len;
        buffdata[size] = '\0';
        if( hasvals ) {
            ptr = GetNextWord1( ptr, token );
            if( *token == '\0' ) {
                SpecialFclose( &gf );
                return( ERR_INVALID_DATA_FILE );
            }
            fn_save( i, token );
        }
    }
    SpecialFclose( &gf );
    *buffer = buffdata;
    return( ERR_NO_ERR );

} /* ReadDataFile */
