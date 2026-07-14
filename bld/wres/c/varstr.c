/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include "layer0.h"
#include "varstr.h"
#include "reserr.h"
#include "wresrtns.h"


VarString *VarStringStart( void )
/*******************************/
{
    VarString       *varstr;

    varstr = WRESALLOC( sizeof( VarString ) );
    if( varstr == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        varstr->len = 0;
        varstr->next = NULL;
    }

    return( varstr );
} /* VarStringStart */

void VarStringAddChar( VarString *varstr, char chr )
/**************************************************/
{
    if( varstr != NULL ) {
        /* skip the parts that are already full */
        while( varstr->next != NULL ) {
            varstr = varstr->next;
        }
        /* if the current part is full */
        if( varstr->len == VAR_STR_PART_SIZE ) {
            varstr->next = VarStringStart();
            if( varstr->next == NULL ) {
                return;
            }
            varstr = varstr->next;
        }
        /* add the new char to the next spot */
        varstr->partString[varstr->len] = chr;
        varstr->len += 1;
    }
} /* VarStringAddChar */

static size_t ComputeVarStringLen( VarString *varstr )
{
    size_t  len;

    len = 0;
    while( varstr != NULL ) {
        len += varstr->len;
        varstr = varstr->next;
    }

    return( len );
} /* ComputeVarStringLen */

char *VarStringEnd( VarString *varstr, size_t *plen )
/****************************************************
 * allocated a continous string for list, copies the string, and free's list/
 * if retlength is not NULL the lenght of the string (excluding the '\0') is
 * returned there
 */
{
    VarString       *tmp;
    char            *str;
    char            *p;
    size_t          len;

    len = ComputeVarStringLen( varstr );
    /* +1 is for the '\0' */
    str = WRESALLOC( len + 1 );
    if( str == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        p = str;
        while( varstr != NULL ) {
            /* copy the current string part */
            memcpy( p, varstr->partString, varstr->len );
            p += varstr->len;
            /* free the current string part */
            tmp = varstr;
            varstr = varstr->next;
            WRESFREE( tmp );
        }
        /* write the '\0' character */
        str[len] = '\0';

        if( plen != NULL ) {
            *plen = len;
        }
    }
    return( str );
} /* EndStringList */
