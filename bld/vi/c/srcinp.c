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
 * srcGenericInput - input a value to a variable
 */
static int srcGenericInput( char *data, vlist *vl, bool input )
{
    int         i;
    char        tmp[MAX_SRC_LINE],v1[MAX_SRC_LINE],str[MAX_STR];
    vars        *v;

    /*
     * get input syntax :
     * INPUT v1
     */
    if( NextWord1( data, v1 ) <= 0 ) {
        return( ERR_SRC_INVALID_INPUT );
    }
    if( !VarName( v1, vl ) ) {
        return( ERR_SRC_INVALID_INPUT );
    }
    if( input ) {
        v = VarFind( v1, vl );
        if( v != NULL ) {
            strcpy( str, v->value );
        } else {
            strcpy( str,"Enter value:");
        }
        i = GetResponse( str, tmp );
    } else {
        i = GOT_RESPONSE;
        tmp[0] = GetKey( FALSE );
        tmp[1] = 0;
    }
    if( i == GOT_RESPONSE ) {
        VarAdd( v1, tmp, vl );
        return( ERR_NO_ERR );
    }
    return( NO_VALUE_ENTERED );

} /* srcGenericInput */

int SrcInput( char *tmp, vlist *vl )
{
    return( srcGenericInput( tmp, vl, TRUE ) );

} /* SrcInput */

void SrcGet( char *tmp, vlist *vl )
{
    srcGenericInput( tmp, vl, FALSE );

} /* SrcGet */
