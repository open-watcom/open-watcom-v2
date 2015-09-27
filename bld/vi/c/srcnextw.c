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

/*
 * SrcNextWord - get next word in a variable, putting result into another
 *               variable
 */
vi_rc SrcNextWord( const char *data, vlist *vl )
{
    char        v1[MAX_SRC_LINE], v2[MAX_SRC_LINE], str[MAX_STR];
    vars        *v;
    char        *ptr;

    /*
     * get syntax :
     * NEXTWORD src res
     */
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( !VarName( v1, str, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( !VarName( v2, str, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    v = VarFind( v1, vl );
    data = SkipLeadingSpaces( v->value );
    if( *data == '"' ) {
        data = GetNextWord( data, str, SingleQuote );
        if( *data == '"' ) {
            ++data;
        }
    } else {
        data = GetNextWord1( data, str );
    }
    // remove next word from src variable
    for( ptr = v->value; (*ptr = *data) != '\0'; ++ptr ) {
        ++data;
    }
    VarAddStr( v2, str, vl );
    return( ERR_NO_ERR );

} /* SrcNextWord */
