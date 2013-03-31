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

#include <stdlib.h>
#include <string.h>
#include "layer0.h"
#include "util.h"
#include "reserr.h"

#define UINT16_MAXDIGITS    5
#define UINT32_MAXDIGITS    9

char * WResIDToStr( const WResID * name )
/***************************************/
/* return the value in an ID if it is a string, NULL otherwise */
{
    char *      string;

    if ( name != NULL && name->IsName) {
        /* alloc space for the string and a \0 char at the end */
        string = WRESALLOC( name->ID.Name.NumChars + 1 );
        if (string == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            /* copy the string */
            memcpy( string, name->ID.Name.Name, name->ID.Name.NumChars );
            string[ name->ID.Name.NumChars ] = '\0';
        }
    } else {
        string = WRESALLOC( UINT16_MAXDIGITS + 1 );
        if( string == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            itoa( name->ID.Num, string, 10 );
        }
    }

    return( string );
} /* WResIDToStr */


char * WResHelpIDToStr( const WResHelpID * name )
/***************************************/
/* return the value in a Help ID if it is a string, NULL otherwise */
{
    char *string;

    if ( name != NULL && name->IsName) {
        /* alloc space for the string and a \0 char at the end */
        string = WRESALLOC( name->ID.Name.NumChars + 1 );
        if (string == NULL) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            /* copy the string */
            memcpy( string, name->ID.Name.Name, name->ID.Name.NumChars );
            string[ name->ID.Name.NumChars ] = '\0';
        }
    } else {
        string = WRESALLOC( UINT32_MAXDIGITS + 1 );
        if( string == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            itoa( name->ID.Num, string, 10 );
        }
    }

    return( string );
} /* WResHelpIDToStr */
