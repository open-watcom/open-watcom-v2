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
#include <string.h>
#include <stdlib.h>
#include "vi.h"
#include "source.h"

/*
 * SrcOpen - open a file
 */
int SrcOpen( sfile *curr, vlist *vl, files *fi, char *data )
{
    int         i;
    char        name[MAX_SRC_LINE],id[MAX_SRC_LINE],type[MAX_SRC_LINE],t;
    ftype       ft;

    /*
     * validate open statement:
     * OPEN name,id,type
     */
    if( NextWord1( data, name ) <= 0 ) {
        return( ERR_SRC_INVALID_OPEN );
    }
    if( NextWord1( data, id ) <= 0 ) {
        return( ERR_SRC_INVALID_OPEN );
    }
    if( NextWord1( data, type ) <= 0 ) {
        return( ERR_SRC_INVALID_OPEN );
    }
    if( curr->hasvar ) {
        Expand( name, vl );
        Expand( id, vl );
        Expand( type, vl );
    }
    if( id[1] != 0 || (id[0] < '1' || id[0] > '9') ) {
        return( ERR_SRC_INVALID_OPEN );
    }
    i = id[0] - '1';
    t = type[0];
    if( type[1] != 0 || ( t != 'x' && t != 'r' && t != 'a' && t != 'w' ) ) {
        return( ERR_SRC_INVALID_OPEN );
    }

    if( fi->ft[i] != SRCFILE_NONE ) {
        return( ERR_SRC_INVALID_OPEN );
    }
    if( t == 'x' ) {
        type[0] = 'r';
    }
    if( name[0] == '@' ) {
        info    *cinfo;

        if( !stricmp( &name[1], "." ) ) {
            cinfo = CurrentInfo;
        } else {
            cinfo = InfoHead;
            while( cinfo != NULL ) {
                if( !strcmp( cinfo->CurrentFile->name, &name[1] ) ) {
                    break;
                }
                cinfo = cinfo->next;
            }
            if( cinfo == NULL ) {
                return( ERR_FILE_NOT_FOUND );
            }
        }
        ft = SRCFILE_BUFF;
        fi->buffer[i].cinfo = cinfo;
        fi->buffer[i].line = 1L;
    } else {
        ft = SRCFILE_FILE;
        fi->f[i] = fopen( name, type );
        if( fi->f[i] == NULL ) {
            return( ERR_FILE_NOT_FOUND );
        }
        if( t == 'x' ) {
            fclose( fi->f[i] );
        }
    }
    if( t == 'x' ) {
        ft = SRCFILE_NONE;
    }

    fi->ft[i] = ft;
    return( ERR_NO_ERR );

} /* SrcOpen */

/*
 * SrcRead - read file
 */
int SrcRead( sfile *curr, files *fi, char *data, vlist *vl )
{
    int         i;
    char        id[MAX_SRC_LINE],v1[MAX_SRC_LINE];

    /*
     * validate read statement:
     * READ id,variable
     */
    if( NextWord1( data, id ) <= 0 ) {
        return( ERR_SRC_INVALID_READ );
    }
    if( NextWord1( data, v1 ) <= 0 ) {
        return( ERR_SRC_INVALID_READ );
    }
    if( curr->hasvar ) {
        Expand( id, vl );
    }
    if( id[1] != 0 || (id[0] < '1' || id[0] > '9') ) {
        return( ERR_SRC_INVALID_READ );
    }
    i = id[0] - '1';
    if( !VarName( v1, vl ) ) {
        return( ERR_SRC_INVALID_READ );
    }
    if( fi->ft[i] == SRCFILE_NONE ) {
        return( ERR_SRC_FILE_NOT_OPEN );
    }
    if( fi->ft[i] == SRCFILE_FILE ) {
        if( fgets( id,MAX_SRC_LINE-1,fi->f[i] ) != NULL ) {
            id[ MAX_SRC_LINE-1 ] = 0;
            id[ strlen(id) - 1] = 0;
            VarAdd( v1, id, vl );
        } else {
            fclose( fi->f[i] );
            fi->ft[i] = SRCFILE_NONE;
            return( END_OF_FILE );
        }
    } else {
        fcb     *cfcb;
        line    *cline;
        int     rc;

        rc = GimmeLinePtr( fi->buffer[i].line,
                                fi->buffer[i].cinfo->CurrentFile,
                                &cfcb, &cline );
        if( rc ) {
            fi->ft[i] = SRCFILE_NONE;
            return( END_OF_FILE );
        }
        fi->buffer[i].line++;
        VarAdd( v1, cline->data, vl );
    }
    return( ERR_NO_ERR );

} /* SrcRead */

/*
 * SrcWrite - write file
 */
int SrcWrite( sfile *curr, files *fi, char *data, vlist *vl )
{
    int         i;
    char        id[MAX_SRC_LINE],v1[MAX_SRC_LINE];

    /*
     * validate write statement:
     * WRITE id "string"
     */
    if( NextWord1( data, id ) <= 0 ) {
        return( ERR_SRC_INVALID_WRITE );
    }
    if( GetStringWithPossibleQuote( data, v1 ) ) {
        return( ERR_SRC_INVALID_WRITE );
    }
    if( curr->hasvar ) {
        Expand( id, vl );
        Expand( v1, vl );
    }
    if( id[1] != 0 || (id[0] < '1' || id[0] > '9') ) {
        return( ERR_SRC_INVALID_WRITE );
    }
    i = id[0] - '1';

    switch( fi->ft[i] ) {
    case SRCFILE_NONE:
        return( ERR_SRC_FILE_NOT_OPEN );
    case SRCFILE_FILE:
        MyFprintf( fi->f[i], "%s\n", v1 );
        break;
    }
    return( ERR_NO_ERR );

} /* SrcWrite */

/*
 * SrcClose - close a work file
 */
int SrcClose( sfile *curr, vlist *vl, files *fi, char *data )
{
    int         i;
    char        id[MAX_SRC_LINE];

    /*
     * validate close statement:
     * CLOSE id
     */
    if( NextWord1( data, id ) <= 0 ) {
        return( ERR_SRC_INVALID_CLOSE );
    }
    if( curr->hasvar ) {
        Expand( id, vl );
    }
    if( id[1] != 0 || (id[0] < '1' || id[0] > '9') ) {
        return( ERR_SRC_INVALID_CLOSE );
    }
    i = id[0] - '1';

    if( fi->ft[i] == SRCFILE_FILE ) {
        fclose( fi->f[i] );
    }
    fi->ft[i] = SRCFILE_NONE;
    return( ERR_NO_ERR );

} /* SrcClose */
