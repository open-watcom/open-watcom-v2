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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "vi.h"

/*
 * ReadDataFile - do just that
 */
int ReadDataFile( char *file, int *cnt, char **buffer, int **vallist,
                        bool hasvals )
{
    GENERIC_FILE        gf;
    int                 i,dcnt,len,size;
    char                token[MAX_STR],buff[MAX_STR];
    char                *buffdata;
    int                 *valdata;

    /*
     * get file and buffer
     */
    if( !SpecialOpen( file, &gf ) ) {
        return( ERR_FILE_NOT_FOUND );
    }

    /*
     * get counts
     */
    if( SpecialFgets( buff,MAX_STR-1,&gf ) < 0 ) {
        SpecialFclose( &gf );
        return( ERR_INVALID_DATA_FILE );
    }
    dcnt = atoi( buff );
    if( hasvals ) {
        valdata = MemAlloc( dcnt * sizeof( int ) );
    } else {
        valdata = NULL;
    }
    buffdata = NULL;
    size = 0;

    /*
     * read all tokens
     */
    for( i=0;i<dcnt;i++ ) {

        len = SpecialFgets( buff, sizeof( buff ) - 1, &gf );
        if( len < 0 ) {
            SpecialFclose( &gf );
            return( ERR_INVALID_DATA_FILE );
        }
        if( hasvals ) {
            len = NextWord1( buff, token );
            if( len <= 0 ) {
                SpecialFclose( &gf );
                return( ERR_INVALID_DATA_FILE );
            }
        } else {
            memcpy( token, buff, len+1 );
        }
        len++;
        buffdata = MemReAlloc( buffdata, size + len + 1);
        memcpy( &buffdata[size], token, len );
        size += len;
        buffdata[size] = 0;
        if( hasvals ) {
            if( NextWord1( buff, token ) <= 0 ) {
                SpecialFclose( &gf );
                return( ERR_INVALID_DATA_FILE );
            }
            valdata[i] = atoi( token );
        }

    }

    SpecialFclose( &gf );

    *buffer = buffdata;
    *vallist = valdata;
    *cnt = dcnt;

    return( ERR_NO_ERR );

} /* ReadDataFile */
