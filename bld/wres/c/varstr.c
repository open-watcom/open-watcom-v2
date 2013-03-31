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


#include <string.h>
#include "layer0.h"
#include "varstr.h"
#include "reserr.h"

VarString * VarStringStart( void )
/********************************/
{
    VarString *    newlist;

    newlist = WRESALLOC( sizeof( VarString ) );
    if (newlist == NULL) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newlist->lastLoc = -1;
        newlist->next = NULL;
    }

    return(newlist);
} /* VarStringStart */

void VarStringAddChar( VarString * list, int newchar )
/****************************************************/
{
    if (list != NULL) {
        /* skip the parts that are already full */
        while (list->next != NULL) {
            list = list->next;
        }
        /* if the current part is full */
        if (list->lastLoc == VAR_STR_PART_SIZE - 1) {
            list->next = VarStringStart();
            if (list->next == NULL) {
                return;
            }
            list = list->next;
        }
        /* add the new char to the next spot */
        list->lastLoc += 1;
        list->partString[list->lastLoc] = newchar;
    }
} /* VarStringAddChar */

static int ComputeVarStringLen( VarString * string )
{
    int     length;

    length = 0;
    while (string != NULL) {
        length += string->lastLoc + 1;
        string = string->next;
    }

    return( length );
} /* ComputeVarStringLen */

char * VarStringEnd( VarString * list, int * retlength )
/******************************************************/
/* allocated a continous string for list, copies the string, and free's list */
/* if retlength is not NULL the lenght of the string (excluding the '\0') is */
/* returned there */
{
    VarString *     oldpart;
    char *          newstring;
    char *          stringpart;
    int             length;

    length = ComputeVarStringLen( list );
    /* +1 is for the '\0' */
    newstring = WRESALLOC( length + 1 );

    if (newstring == NULL) {
        WRES_ERROR( WRS_MALLOC_FAILED )
    } else {
        stringpart = newstring;
        while ( list != NULL ) {
            /* copy the current string part */
            memcpy( stringpart, list->partString, list->lastLoc + 1 );
            stringpart += list->lastLoc + 1;

            /* free the current string part */
            oldpart = list;
            list = list->next;
            WRESFREE( oldpart );
        }

        /* write the '\0' character */
        newstring[ length ] = '\0';

        if (retlength != NULL) {
            *retlength = length;
        }
    }

    return( newstring );
} /* EndStringList */
