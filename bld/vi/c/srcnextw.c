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
#include "rtns.h"
#include "source.h"

/*
 * SrcNextWord - get next word in a variable, putting result into another
 *               variable
 */
vi_rc SrcNextWord( char *data, vlist *vl )
{
    char        v1[MAX_SRC_LINE], v2[MAX_SRC_LINE], str[MAX_STR];
    vars        *v;

    /*
     * get syntax :
     * NEXTWORD src res
     */
    if( NextWord1( data, v1 ) <= 0 ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( !VarName( v1, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( NextWord1( data, v2 ) <= 0 ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    if( !VarName( v2, vl ) ) {
        return( ERR_SRC_INVALID_NEXTWORD );
    }
    v = VarFind( v1, vl );
    RemoveLeadingSpaces( v->value );
    if( v->value[0] == '"' ) {
        NextWord( v->value, str, SingleQuote );
        EliminateFirstN( v->value, 1 );
    } else {
        NextWord1( v->value, str );
    }
    VarAddStr( v2, str, vl );
    return( ERR_NO_ERR );

} /* SrcNextWord */
