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
#include <setjmp.h>
#include "vi.h"
#include "source.h"
#include "expr.h"

/*
 * SrcIf - process if statement
 */
int SrcIf( sfile **sf, vlist *vl )
{
    char        v1[MAX_SRC_LINE];
    long        val;
    int         rc;
    jmp_buf     jmpaddr;

    strcpy( v1, (*sf)->arg1 );
    if( (*sf)->hasvar ) {
        Expand( v1, vl );
    }
    rc = setjmp( jmpaddr );
    if( rc == 0 ) {
        StartExprParse( v1, jmpaddr );
        val = GetConstExpr();
    } else {
        return( rc );
    }
    (*sf)->branchres = (val != 0);
    return( ERR_NO_ERR );

} /* SrcIf */

/*
 * GetErrorTokenValue
 */
int GetErrorTokenValue( int *value, char *str )
{
    int i;

    i = ReadErrorTokens();
    if( i ) {
        return( i );
    }
    i = Tokenize( ErrorTokens, str, TRUE );
    if( i >= 0 ) {
        *value = ErrorValues[ i ];
    } else {
        *value = atoi( str );
    }
    return( ERR_NO_ERR );

} /* GetErrorTokenValue */

static bool errorRead = FALSE;
/*
 * ReadErrorTokens - do just that
 */
int ReadErrorTokens( void )
{
    int         *vals;
    int         rc,cnt;
    char        *buff;

    if( errorRead ) {
        return( ERR_NO_ERR );
    }

    rc = ReadDataFile( "error.dat", &cnt, &buff, &vals, TRUE );
    if( rc ) {
        if( rc == ERR_FILE_NOT_FOUND ) {
            return( ERR_SRC_NO_ERROR_DATA );
        }
        return( rc );
    }
    ErrorTokens = buff;
    ErrorValues = vals;
    errorRead = TRUE;

    return( ERR_NO_ERR );

} /* ReadErrorTokens */
