/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
vi_rc SrcNextWord( const char *data, vars_list *vl )
{
    char        name1[MAX_SRC_LINE], name2[MAX_SRC_LINE], str[MAX_STR];
    vars        *v;
    char        *ptr;

    /*
     * get syntax :
     * NEXTWORD src res
     */
    if( !ReadVarName( &data, name1, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( !ReadVarName( &data, name2, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    v = VarFind( name1, vl );
    data = v->value;
    SKIP_SPACES( data );
    if( *data == '"' ) {
        data = GetNextWord( data, str, SingleDQuote );
        if( *data == '"' ) {
            SKIP_CHAR_SPACES( data );
        }
    } else {
        data = GetNextWord1( data, str );
    }
    // remove next word from src variable
    for( ptr = v->value; (*ptr = *data) != '\0'; ++ptr ) {
        ++data;
    }
    VarAddStr( name2, str, vl );
    return( ERR_NO_ERR );

} /* SrcNextWord */
