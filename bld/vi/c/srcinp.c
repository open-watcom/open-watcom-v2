/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
 * srcGenericInput - input a value to a variable
 */
static vi_rc srcGenericInput( const char *data, vars_list *vl, bool input )
{
    vi_rc       resp;
    char        tmp[MAX_SRC_LINE], name[MAX_SRC_LINE], str[MAX_STR];
    vars        *v;

    /*
     * get input syntax :
     * INPUT name
     */
    if( !ReadVarName( &data, name, vl ) ) {
        return( ERR_SRC_INVALID_INPUT );
    }
    if( input ) {
        v = VarFind( name, vl );
        if( v != NULL ) {
            strcpy( str, v->value );
        } else {
            strcpy( str, "Enter value:" );
        }
        resp = GetResponse( str, tmp, sizeof( tmp ) );
    } else {
        resp = GOT_RESPONSE;
        tmp[0] = GetKey( false );
        tmp[1] = '\0';
    }
    if( resp == GOT_RESPONSE ) {
        VarAddStr( name, tmp, vl );
        return( ERR_NO_ERR );
    }
    return( NO_VALUE_ENTERED );

} /* srcGenericInput */

vi_rc SrcInput( const char *tmp, vars_list *vl )
{
    return( srcGenericInput( tmp, vl, true ) );

} /* SrcInput */

void SrcGet( const char *tmp, vars_list *vl )
{
    srcGenericInput( tmp, vl, false );

} /* SrcGet */
