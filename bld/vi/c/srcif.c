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


#include "vi.h"
#include <setjmp.h>
#include "source.h"
#include "expr.h"

/*
 * SrcIf - process if statement
 */
vi_rc SrcIf( sfile **sf, vlist *vl )
{
    char        v1[MAX_SRC_LINE];
    long        val;
    int         i;
    jmp_buf     jmpaddr;

    strcpy( v1, (*sf)->arg1 );
    if( (*sf)->hasvar ) {
        Expand( v1, vl );
    }
    i = setjmp( jmpaddr );
    if( i != 0 ) {
        return( (vi_rc)i );
    }
    StartExprParse( v1, jmpaddr );
    val = GetConstExpr();
    (*sf)->u.branchres = (val != 0);
    return( ERR_NO_ERR );

} /* SrcIf */

/*
 * GetErrorTokenValue
 */
vi_rc GetErrorTokenValue( int *value, char *str )
{
    int     i;
    vi_rc   rc;

    rc = ReadErrorTokens();
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    i = Tokenize( ErrorTokens, str, TRUE );
    if( i >= 0 ) {
        *value = ErrorValues[i];
    } else {
        *value = atoi( str );
    }
    return( ERR_NO_ERR );

} /* GetErrorTokenValue */


static bool err_alloc( int cnt )
{
    ErrorValues = MemAlloc( cnt * sizeof( int ) );
    return( TRUE );
}

static bool err_save( int i, char *buff )
{
    ErrorValues[i] = atoi( buff );
    return( TRUE );
}

static bool errorRead = FALSE;
/*
 * ReadErrorTokens - do just that
 */
vi_rc ReadErrorTokens( void )
{
    vi_rc       rc;

    if( errorRead ) {
        return( ERR_NO_ERR );
    }

    rc = ReadDataFile( "error.dat", &ErrorTokens, err_alloc, err_save );
    if( rc != ERR_NO_ERR ) {
        if( rc == ERR_FILE_NOT_FOUND ) {
            return( ERR_SRC_NO_ERROR_DATA );
        }
        return( rc );
    }
    errorRead = TRUE;

    return( ERR_NO_ERR );

} /* ReadErrorTokens */
