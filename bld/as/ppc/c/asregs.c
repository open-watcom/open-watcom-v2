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


#include "as.h"

#ifdef AS_DEBUG_DUMP

static char buffer[ 10 ];

char *AsRegName( reg r ) {

    char        *s;

    memset( buffer, 0, sizeof( buffer ) );
    switch( RegClass( r ) ) {
    case RC_GPR:
        strcpy( buffer, "r" );
        break;
    case RC_FPR:
        strcpy( buffer, "fr" );
        break;
    case RC_CRF:
        strcpy( buffer, "cr" );
        break;
    case RC_CRB:
        strcpy( buffer, "crb" );
        break;
    default:
        return( "EUR" );        /* ERROR: UNKNOWN REGISTER */
    }
    for( s = buffer; *s; s++ );
    sprintf( s, "%d", RegIndex( r ) );
    return( buffer );
}

#endif

extern uint RegCrfToBI( reg r ) {

    assert( RegClass( r ) == RC_CRF );
    return( RegIndex( r ) << 2 );
}
